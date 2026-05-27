#pragma once

namespace Motor {
  void begin();
  void actualizar();

  void avance();
  void atras();

  void pararSuave();
  void pararInmediato();
  void deshabilitar();

  void subirVelocidad();
  void bajarVelocidad();

  bool estaEnMarcha();
}