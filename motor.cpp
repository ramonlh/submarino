#include "motor.h"
#include "config.h"
#include <Arduino.h>

namespace {
  enum SentidoMotor {
    MOTOR_PARADO,
    MOTOR_AVANCE,
    MOTOR_ATRAS
  };

  SentidoMotor sentido = MOTOR_PARADO;

  int velocidadMotor = VELOCIDAD_INICIAL;

  int pwmActual = 0;
  int pwmObjetivo = 0;

  unsigned long ultimoPasoRampa = 0;

  void escribirPWM() {
    if (pwmActual == 0) {
      ledcWrite(PIN_RPWM, 0);
      ledcWrite(PIN_LPWM, 0);

      if (pwmObjetivo == 0) {
        sentido = MOTOR_PARADO;
      }

      return;
    }

    if (sentido == MOTOR_AVANCE) {
      ledcWrite(PIN_RPWM, pwmActual);
      ledcWrite(PIN_LPWM, 0);
    }
    else if (sentido == MOTOR_ATRAS) {
      ledcWrite(PIN_RPWM, 0);
      ledcWrite(PIN_LPWM, pwmActual);
    }
    else {
      ledcWrite(PIN_RPWM, 0);
      ledcWrite(PIN_LPWM, 0);
    }
  }
}

namespace Motor {

  void begin() {
    pinMode(PIN_ENABLE_MOTOR, OUTPUT);

    // Al arrancar, motor deshabilitado
    digitalWrite(PIN_ENABLE_MOTOR, LOW);

    ledcAttach(PIN_RPWM, PWM_FREQ, PWM_RES);
    ledcAttach(PIN_LPWM, PWM_FREQ, PWM_RES);

    ledcWrite(PIN_RPWM, 0);
    ledcWrite(PIN_LPWM, 0);

    Serial.println("Motor HW-039 / BTS7960 listo.");
  }

  void actualizar() {
    if (millis() - ultimoPasoRampa < INTERVALO_RAMPA_MS) {
      return;
    }

    ultimoPasoRampa = millis();

    if (pwmActual < pwmObjetivo) {
      pwmActual += PASO_SUBIDA;

      if (pwmActual > pwmObjetivo) {
        pwmActual = pwmObjetivo;
      }
    }
    else if (pwmActual > pwmObjetivo) {
      pwmActual -= PASO_BAJADA;

      if (pwmActual < pwmObjetivo) {
        pwmActual = pwmObjetivo;
      }
    }

    escribirPWM();
  }

  void avance() {
    Serial.print("Motor avance. Velocidad = ");
    Serial.println(velocidadMotor);

    digitalWrite(PIN_ENABLE_MOTOR, HIGH);

    sentido = MOTOR_AVANCE;
    pwmObjetivo = velocidadMotor;
  }

  void atras() {
    Serial.print("Motor atras. Velocidad = ");
    Serial.println(velocidadMotor);

    digitalWrite(PIN_ENABLE_MOTOR, HIGH);

    sentido = MOTOR_ATRAS;
    pwmObjetivo = velocidadMotor;
  }

  void pararSuave() {
    Serial.println("Motor parar suave.");
    pwmObjetivo = 0;
  }

  void pararInmediato() {
    Serial.println("Motor parar inmediato.");

    pwmActual = 0;
    pwmObjetivo = 0;
    sentido = MOTOR_PARADO;

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

    if (sentido != MOTOR_PARADO) {
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

    if (sentido != MOTOR_PARADO) {
      pwmObjetivo = velocidadMotor;
    }
  }

  bool estaEnMarcha() {
    return sentido != MOTOR_PARADO;
  }
}