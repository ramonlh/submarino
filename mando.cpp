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
  { 1,  "Avance",             6047491UL, ACC_AVANCE },
  { 2,  "Atras",              6047500UL, ACC_ATRAS },

  { 3,  "Luces",              6047536UL, ACC_LUCES },
  { 4,  "Libre / Auxiliar",   6047692UL, ACC_AUXILIAR },

  { 5,  "Izquierda",          6047548UL, ACC_IZQUIERDA },
  { 6,  "Derecha",            6047695UL, ACC_DERECHA },
  { 7,  "Timon centro",       6047551UL, ACC_TIMON_CENTRO },
  { 8,  "Timon centro",       6047728UL, ACC_TIMON_CENTRO },

  { 9,  "Velocidad +",        6047683UL, ACC_VELOCIDAD_MAS },
  { 10, "Velocidad -",        6047503UL, ACC_VELOCIDAD_MENOS },
  { 11, "Parar motor",        6047740UL, ACC_PARAR_MOTOR },
  { 12, "Parada total",       6047731UL, ACC_PARADA_TOTAL },

  { 13, "Timon centro",       6047743UL, ACC_TIMON_CENTRO },
  { 14, "Libre",              6047680UL, ACC_LIBRE },
  { 15, "Parar motor",        6047539UL, ACC_PARAR_MOTOR },
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