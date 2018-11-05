#pragma once

#include "A_Config.h"
#include "Z_Types.h"
#include "PaletteManager.h"
#include "GammaManager.h"
#include "PatternController.h"
#include "ArduinoTrace.h"

#define FPS_TO_US(x) (ONE_SEC_US / x)
#define FPS_TO_MS(x) (ONE_SEC_MS / x)


#define NUM_PM_PARAMS 2

// Timing debug tools
#ifdef DEBUG_TIMING
  uint32_t timingValues[10];
  uint32_t curDebugTime;
  uint32_t lastDebugTime;
#endif

// Objects
CRGBArray<NUM_LEDS> leds;
uint8_t leds_b[NUM_LEDS];
CRGBArray<NUM_LEDS> leds_top;
uint8_t leds_top_b[NUM_LEDS];
uint8_t globalBrightness = INIT_GLOBAL_BRIGHTNESS;

PaletteManager pm;
GammaManager Gamma;
PatternController pc;

struct_timers timing;
struct_config layerConfig;
struct_base_show_params baseParams;
struct_top_show_params topParams;

