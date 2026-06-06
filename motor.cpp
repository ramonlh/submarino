#include "motor.h"
#include "config.h"
#include <Arduino.h>

namespace {

  enum SentidoMotor {
    MOTOR_PARADO,
    MOTOR_AVANCE,
    MOTOR_ATRAS
  };

  // Sentido en el que está trabajando el motor ahora
  SentidoMotor sentido = MOTOR_PARADO;

  // Sentido que queremos usar después de parar.
  // Sirve para cambiar de avance a atrás sin invertir de golpe.
  SentidoMotor sentidoPendiente = MOTOR_PARADO;

  int velocidadMotor = VELOCIDAD_INICIAL;

  int pwmActual = 0;
  int pwmObjetivo = 0;

  unsigned long ultimoPasoRampa = 0;


  // -------------------------------------------------------
  // Escribe el PWM real en el driver HW-039 / BTS7960
  // -------------------------------------------------------
  void escribirPWM() {
    // Si el PWM actual es cero, apagamos las dos entradas.
    if (pwmActual == 0) {
      ledcWrite(PIN_RPWM, 0);
      ledcWrite(PIN_LPWM, 0);
      return;
    }

    // Avance: PWM en RPWM y LPWM apagado.
    if (sentido == MOTOR_AVANCE) {
      ledcWrite(PIN_RPWM, pwmActual);
      ledcWrite(PIN_LPWM, 0);
    }

    // Atrás: PWM en LPWM y RPWM apagado.
    else if (sentido == MOTOR_ATRAS) {
      ledcWrite(PIN_RPWM, 0);
      ledcWrite(PIN_LPWM, pwmActual);
    }

    // Seguridad: si no hay sentido válido, apagamos todo.
    else {
      ledcWrite(PIN_RPWM, 0);
      ledcWrite(PIN_LPWM, 0);
    }
  }


  // -------------------------------------------------------
  // Pide un sentido de giro.
  //
  // Si el motor está parado:
  //   arranca suave.
  //
  // Si ya va en ese sentido:
  //   mantiene el sentido y ajusta velocidad.
  //
  // Si va en sentido contrario:
  //   primero baja suave hasta cero.
  //   luego cambia de sentido.
  //   luego arranca suave.
  // -------------------------------------------------------
  void pedirSentido(SentidoMotor nuevoSentido) {
    digitalWrite(PIN_ENABLE_MOTOR, HIGH);

    // Si está parado de verdad, arrancamos directamente suave.
    if (pwmActual == 0 && sentido == MOTOR_PARADO) {
      sentido = nuevoSentido;
      sentidoPendiente = MOTOR_PARADO;
      pwmObjetivo = velocidadMotor;

      Serial.println("Motor: arranque suave desde parado.");
      return;
    }

    // Si ya está en ese sentido, no paramos.
    // Solo ajustamos el objetivo de velocidad.
    if (sentido == nuevoSentido) {
      sentidoPendiente = MOTOR_PARADO;
      pwmObjetivo = velocidadMotor;

      Serial.println("Motor: mismo sentido, velocidad actualizada.");
      return;
    }

    // Si está girando en el sentido contrario,
    // no cambiamos el sentido todavía.
    // Guardamos el nuevo sentido y bajamos el PWM a cero.
    sentidoPendiente = nuevoSentido;
    pwmObjetivo = 0;

    Serial.println("Motor: cambio de sentido pedido.");
    Serial.println("Motor: primero paro suave hasta PWM 0.");
  }


  // -------------------------------------------------------
  // Cuando el motor ya ha bajado a cero, si había un
  // sentido pendiente, lo activamos y empezamos otra rampa.
  // -------------------------------------------------------
  void arrancarSentidoPendienteSiProcede() {
    if (pwmActual != 0) {
      return;
    }

    if (sentidoPendiente == MOTOR_PARADO) {
      // No hay cambio pendiente. Si el objetivo también es cero,
      // el motor queda parado.
      if (pwmObjetivo == 0) {
        sentido = MOTOR_PARADO;
      }
      return;
    }

    Serial.println("Motor: PWM ya es 0.");
    Serial.println("Motor: cambiando sentido y arrancando suave.");

    sentido = sentidoPendiente;
    sentidoPendiente = MOTOR_PARADO;
    pwmObjetivo = velocidadMotor;
  }
}


namespace Motor {

  void begin() {
    pinMode(PIN_ENABLE_MOTOR, OUTPUT);

    // Al arrancar, motor deshabilitado.
    digitalWrite(PIN_ENABLE_MOTOR, LOW);

    // Canales PWM fijos para evitar conflictos con el servo.
    // Arduino-ESP32 3.x soporta ledcAttachChannel().
    ledcAttachChannel(PIN_RPWM, PWM_FREQ, PWM_RES, 6);
    ledcAttachChannel(PIN_LPWM, PWM_FREQ, PWM_RES, 7);

    ledcWrite(PIN_RPWM, 0);
    ledcWrite(PIN_LPWM, 0);

    sentido = MOTOR_PARADO;
    sentidoPendiente = MOTOR_PARADO;
    pwmActual = 0;
    pwmObjetivo = 0;

    Serial.println("Motor HW-039 / BTS7960 listo.");
  }


  void actualizar() {
    if (millis() - ultimoPasoRampa < INTERVALO_RAMPA_MS) {
      return;
    }

    ultimoPasoRampa = millis();

    // Rampa de subida
    if (pwmActual < pwmObjetivo) {
      pwmActual += PASO_SUBIDA;

      if (pwmActual > pwmObjetivo) {
        pwmActual = pwmObjetivo;
      }
    }

    // Rampa de bajada
    else if (pwmActual > pwmObjetivo) {
      pwmActual -= PASO_BAJADA;

      if (pwmActual < pwmObjetivo) {
        pwmActual = pwmObjetivo;
      }
    }

    escribirPWM();

    // Si estábamos parando para invertir, aquí se decide
    // el cambio de sentido cuando pwmActual llega a cero.
    arrancarSentidoPendienteSiProcede();

    escribirPWM();
  }


  void avance() {
    Serial.print("Motor: pedido AVANCE. Velocidad = ");
    Serial.println(velocidadMotor);

    pedirSentido(MOTOR_AVANCE);
  }


  void atras() {
    Serial.print("Motor: pedido ATRAS. Velocidad = ");
    Serial.println(velocidadMotor);

    pedirSentido(MOTOR_ATRAS);
  }


  void pararSuave() {
    Serial.println("Motor: parar suave.");

    pwmObjetivo = 0;
    sentidoPendiente = MOTOR_PARADO;
  }


  void pararInmediato() {
    Serial.println("Motor: parar inmediato.");

    pwmActual = 0;
    pwmObjetivo = 0;

    sentido = MOTOR_PARADO;
    sentidoPendiente = MOTOR_PARADO;

    ledcWrite(PIN_RPWM, 0);
    ledcWrite(PIN_LPWM, 0);
  }


  void deshabilitar() {
    pararInmediato();

    digitalWrite(PIN_ENABLE_MOTOR, LOW);

    Serial.println("Motor deshabilitado.");
  }


  void subirVelocidad() {
    velocidadMotor += PASO_VELOCIDAD;

    if (velocidadMotor > VELOCIDAD_MAXIMA) {
      velocidadMotor = VELOCIDAD_MAXIMA;
    }

    Serial.print("Nueva velocidad = ");
    Serial.println(velocidadMotor);

    // Si ya está en marcha y no está invirtiendo,
    // actualizamos la velocidad objetivo.
    if (sentido != MOTOR_PARADO && sentidoPendiente == MOTOR_PARADO) {
      pwmObjetivo = velocidadMotor;
    }
  }


  void bajarVelocidad() {
    velocidadMotor -= PASO_VELOCIDAD;

    if (velocidadMotor < VELOCIDAD_MINIMA) {
      velocidadMotor = VELOCIDAD_MINIMA;
    }

    Serial.print("Nueva velocidad = ");
    Serial.println(velocidadMotor);

    // Si ya está en marcha y no está invirtiendo,
    // actualizamos la velocidad objetivo.
    if (sentido != MOTOR_PARADO && sentidoPendiente == MOTOR_PARADO) {
      pwmObjetivo = velocidadMotor;
    }
  }


  bool estaEnMarcha() {
    return pwmActual > 0 ||
           pwmObjetivo > 0 ||
           sentido != MOTOR_PARADO ||
           sentidoPendiente != MOTOR_PARADO;
  }
}
