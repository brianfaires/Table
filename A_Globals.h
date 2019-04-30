#pragma once

#include "A_Config.h"
#include "ArduinoTrace.h"
#include "Z_Types.h"
#include "PaletteManager.h"
#include "GammaManager.h"
#include "PatternController.h"

#define FPS_TO_TIME(x) (ONE_SEC / x)

#define NUM_PM_PARAMS 2

// Timing debug tools
#ifdef DEBUG_TIMING //todo: see if this is defined when DEBUG_TIMING() is defined but with no implementation
  uint32_t timingValues[10];
  uint32_t curDebugTime;
  uint32_t lastDebugTime;
#endif

// Config and params
struct_timers timing;
struct_config layerConfig;
struct_base_show_params baseParams;
struct_top_show_params topParams;

// Objects
CRGBArray<NUM_LEDS> leds;
uint8_t leds_b[NUM_LEDS];
CRGBArray<NUM_LEDS> leds_top;
uint8_t leds_top_b[NUM_LEDS];
uint8_t globalBrightness = INIT_GLOBAL_BRIGHTNESS;

PaletteManager pm;
GammaManager Gamma;
PatternController pc;

// Layer counters
uint8_t baseTransitionProgress;
uint8_t topTransitionProgress;
