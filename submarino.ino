#include "config.h"
#include "mando.h"
#include "motor.h"
#include "timon.h"

// Guarda cuándo recibimos el último botón del mando
unsigned long ultimoComandoMs = 0;

// =======================================================
// SETUP
// =======================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("======================================");
  Serial.println(" SUBMARINO - VERSION MODULAR");
  Serial.println(" Mando + Motor + Timon");
  Serial.println("======================================");

  Mando::begin();
  Motor::begin();
  Timon::begin();

  paradaTotal();

  Serial.println("Sistema listo.");
}

// =======================================================
// LOOP
// =======================================================

void loop() {
  Accion accion;

  if (Mando::leer(accion)) {
    ultimoComandoMs = millis();
    ejecutarAccion(accion);
  }

  Motor::actualizar();

  comprobarSeguridad();
}


// =======================================================
// EJECUTAR ACCIONES DEL MANDO
// =======================================================

void ejecutarAccion(Accion accion) {
  switch (accion) {

    case ACC_VELOCIDAD_MAS:
      Motor::subirVelocidad();
      break;

    case ACC_VELOCIDAD_MENOS:
      Motor::bajarVelocidad();
      break;

    case ACC_AUXILIAR:
      Serial.println("Auxiliar: sin uso por ahora.");
      break;

    case ACC_SUBIR:
      Serial.println("Subir: reservado para futuro timon de profundidad.");
      break;

    case ACC_BAJAR:
      Serial.println("Bajar: reservado para futuro timon de profundidad.");
      break;

    case ACC_IZQUIERDA:
      Timon::izquierda();
      break;

    case ACC_DERECHA:
      Timon::derecha();
      break;

    case ACC_AVANCE:
      Motor::avance();
      break;

    case ACC_ATRAS:
      Motor::atras();
      break;

    case ACC_PARAR_MOTOR:
      Motor::pararSuave();
      break;

    case ACC_PARADA_TOTAL:
      paradaTotal();
      break;

    case ACC_TIMON_CENTRO:
      Timon::centro();
      break;

    case ACC_PROFUNDIDAD_CENTRO:
      Serial.println("Profundidad centro: reservado para futuro servo de profundidad.");
      break;

    case ACC_EMERGER_EMERGENCIA:
      emergerEmergencia();
      break;

    case ACC_LIBRE:
      Serial.println("Boton libre.");
      break;

    default:
      Serial.println("Accion sin uso.");
      break;
  }
}


// =======================================================
// FUNCIONES DE SEGURIDAD
// =======================================================

void emergerEmergencia() {
  Serial.println("EMERGENCIA: parar motor y centrar timon.");

  Motor::deshabilitar();
  Timon::centro();
}

void paradaTotal() {
  Serial.println("PARADA TOTAL.");

  Motor::deshabilitar();
  Timon::centro();
}

void comprobarSeguridad() {
  if (!USAR_SEGURIDAD_TIMEOUT) {
    return;
  }

  if (millis() - ultimoComandoMs > TIMEOUT_MANDO_MS) {
    Serial.println("SEGURIDAD: sin mando. Parar motor y centrar timon.");

    Motor::deshabilitar();
    Timon::centro();

    // Para no repetir el mensaje constantemente
    ultimoComandoMs = millis();
  }
}