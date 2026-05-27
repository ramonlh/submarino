#pragma once

// =======================================================
// CONFIGURACION GENERAL DEL SUBMARINO
// =======================================================

// ---------- PINES ----------
inline constexpr int PIN_RF = 16;

inline constexpr int PIN_SERVO_PROFUNDIDAD = 18;

inline constexpr int PIN_RPWM = 25;
inline constexpr int PIN_LPWM = 26;
inline constexpr int PIN_ENABLE_MOTOR = 27;

// ---------- SERVO TIMON DIRECCION ----------
inline constexpr int PIN_SERVO_TIMON = 18;

// Ajustar según tu montaje mecánico.
// Si gira al revés, intercambia TIMON_IZQUIERDA y TIMON_DERECHA.
inline constexpr int TIMON_IZQUIERDA = 60;
inline constexpr int TIMON_CENTRO    = 90;
inline constexpr int TIMON_DERECHA   = 120;


// ---------- MOTOR ----------
inline constexpr int PWM_FREQ = 1000;
inline constexpr int PWM_RES  = 8;

inline constexpr int VELOCIDAD_INICIAL = 100;
inline constexpr int VELOCIDAD_MINIMA  = 70;
inline constexpr int VELOCIDAD_MAXIMA  = 160;
inline constexpr int PASO_VELOCIDAD    = 15;

inline constexpr unsigned long INTERVALO_RAMPA_MS = 30;
inline constexpr int PASO_SUBIDA = 3;
inline constexpr int PASO_BAJADA = 6;


// ---------- SEGURIDAD ----------
inline constexpr bool USAR_SEGURIDAD_TIMEOUT = false;
inline constexpr unsigned long TIMEOUT_MANDO_MS = 2000;


// ---------- MANDO 433 MHz ----------
inline constexpr unsigned long TIEMPO_REPETICION_MS = 250;