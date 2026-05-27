#pragma once
#include <Arduino.h>

// Acciones que puede pedir el mando
enum Accion {
  ACC_NADA,

  ACC_VELOCIDAD_MAS,
  ACC_VELOCIDAD_MENOS,
  ACC_LUCES,
  ACC_AUXILIAR,

  ACC_SUBIR,
  ACC_BAJAR,
  ACC_IZQUIERDA,
  ACC_DERECHA,

  ACC_AVANCE,
  ACC_ATRAS,
  ACC_PARAR_MOTOR,
  ACC_PARADA_TOTAL,

  ACC_TIMON_CENTRO,
  ACC_PROFUNDIDAD_CENTRO,
  ACC_EMERGER_EMERGENCIA,
  ACC_LIBRE
};

namespace Mando {
  void begin();

  // Devuelve true si se ha recibido un botón válido
  bool leer(Accion &accion);
}