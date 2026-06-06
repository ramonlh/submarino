#include "timon.h"
#include "config.h"
#include <Arduino.h>
#include <ESP32Servo.h>

namespace {
  Servo servoTimon;

  int anguloActual = TIMON_CENTRO;
  int anguloObjetivo = TIMON_CENTRO;

  unsigned long ultimoMovimientoMs = 0;

  const unsigned long INTERVALO_MOVIMIENTO_MS = 25;
  const int PASO_TIMON = 1;

  void pedirAngulo(int nuevoAngulo) {
    anguloObjetivo = constrain(nuevoAngulo, 0, 180);

    Serial.print("Timon objetivo = ");
    Serial.println(anguloObjetivo);
  }
}

namespace Timon {

  void begin() {
    servoTimon.setPeriodHertz(50);

    // Estos valores funcionaron en tu prueba simple
    servoTimon.attach(PIN_SERVO_TIMON, 1000, 2000);

    anguloActual = TIMON_CENTRO;
    anguloObjetivo = TIMON_CENTRO;

    servoTimon.write(anguloActual);

    Serial.print("Servo de timon listo en GPIO");
    Serial.println(PIN_SERVO_TIMON);
  }

  void actualizar() {
    if (millis() - ultimoMovimientoMs < INTERVALO_MOVIMIENTO_MS) {
      return;
    }

    ultimoMovimientoMs = millis();

    if (anguloActual < anguloObjetivo) {
      anguloActual += PASO_TIMON;

      if (anguloActual > anguloObjetivo) {
        anguloActual = anguloObjetivo;
      }

      servoTimon.write(anguloActual);
    }
    else if (anguloActual > anguloObjetivo) {
      anguloActual -= PASO_TIMON;

      if (anguloActual < anguloObjetivo) {
        anguloActual = anguloObjetivo;
      }

      servoTimon.write(anguloActual);
    }
  }

  void izquierda() {
    Serial.println("Timon: izquierda suave.");
    pedirAngulo(TIMON_IZQUIERDA);
  }

  void derecha() {
    Serial.println("Timon: derecha suave.");
    pedirAngulo(TIMON_DERECHA);
  }

  void centro() {
    Serial.println("Timon: centro suave.");
    pedirAngulo(TIMON_CENTRO);
  }
}