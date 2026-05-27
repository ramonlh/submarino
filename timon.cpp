#include "timon.h"
#include "config.h"
#include <Arduino.h>
#include <ESP32Servo.h>

namespace {
  Servo servoTimon;
}

namespace Timon {

  void begin() {
    servoTimon.setPeriodHertz(50);
    servoTimon.attach(PIN_SERVO_TIMON, 500, 2400);

    centro();

    Serial.println("Servo de timon de direccion listo.");
  }

  void izquierda() {
    Serial.println("Timon: izquierda.");
    servoTimon.write(TIMON_IZQUIERDA);
  }

  void derecha() {
    Serial.println("Timon: derecha.");
    servoTimon.write(TIMON_DERECHA);
  }

  void centro() {
    Serial.println("Timon: centro.");
    servoTimon.write(TIMON_CENTRO);
  }
}