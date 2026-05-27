#include "mando.h"
#include "config.h"
#include <RCSwitch.h>

namespace {
  RCSwitch rf;

  struct BotonMando {
    uint8_t numero;
    const char* nombre;
    unsigned long codigo;
    Accion accion;
  };

  // Si se cambia de mando, solo hay que cambiar esta tabla
  BotonMando tablaMando[] = {
    { 1,  "Velocidad +",        6047491UL, ACC_VELOCIDAD_MAS },
    { 2,  "Velocidad -",        6047500UL, ACC_VELOCIDAD_MENOS },
    { 3,  "Luces",              6047536UL, ACC_LUCES },
    { 4,  "Libre / Auxiliar",   6047692UL, ACC_AUXILIAR },

    { 5,  "Subir",              6047695UL, ACC_SUBIR },
    { 6,  "Bajar",              6047548UL, ACC_BAJAR },
    { 7,  "Izquierda",          6047551UL, ACC_IZQUIERDA },
    { 8,  "Derecha",            6047728UL, ACC_DERECHA },

    { 9,  "Avance",             6047683UL, ACC_AVANCE },
    { 10, "Atras",              6047503UL, ACC_ATRAS },
    { 11, "Parar motor",        6047740UL, ACC_PARAR_MOTOR },
    { 12, "Parada total",       6047731UL, ACC_PARADA_TOTAL },

    { 13, "Timon centro",       6047743UL, ACC_TIMON_CENTRO },
    { 14, "Profundidad centro", 6047680UL, ACC_PROFUNDIDAD_CENTRO },
    { 15, "Emerger emergencia", 6047539UL, ACC_EMERGER_EMERGENCIA },
    { 16, "Libre",              6047488UL, ACC_LIBRE }
  };

  const int NUM_BOTONES = sizeof(tablaMando) / sizeof(tablaMando[0]);

  unsigned long ultimoCodigo = 0;
  unsigned long ultimoCodigoMs = 0;

  int buscarCodigo(unsigned long codigo) {
    for (int i = 0; i < NUM_BOTONES; i++) {
      if (tablaMando[i].codigo == codigo) {
        return i;
      }
    }

    return -1;
  }
}

namespace Mando {

  void begin() {
    rf.enableReceive(digitalPinToInterrupt(PIN_RF));

    Serial.print("Mando 433 MHz listo en GPIO");
    Serial.println(PIN_RF);
  }

  bool leer(Accion &accion) {
    accion = ACC_NADA;

    if (!rf.available()) {
      return false;
    }

    unsigned long codigo = rf.getReceivedValue();
    rf.resetAvailable();

    if (codigo == 0) {
      Serial.println("Codigo 433 no decodificado.");
      return false;
    }

    // Evita repetir demasiado si se mantiene pulsado el mismo botón
    if (codigo == ultimoCodigo &&
        millis() - ultimoCodigoMs < TIEMPO_REPETICION_MS) {
      return false;
    }

    ultimoCodigo = codigo;
    ultimoCodigoMs = millis();

    int indice = buscarCodigo(codigo);

    Serial.println("--------------------------------------");
    Serial.print("Codigo recibido: ");
    Serial.println(codigo);

    if (indice < 0) {
      Serial.println("Codigo no encontrado en la tabla.");
      return false;
    }

    Serial.print("Boton ");
    Serial.print(tablaMando[indice].numero);
    Serial.print(" -> ");
    Serial.println(tablaMando[indice].nombre);

    accion = tablaMando[indice].accion;
    return true;
  }
}