#ifndef GLOBALS_H
#define GLOBALS_H

#include "A_Config.h"
#include "Z_Types.h"
#include "PaletteManager.h"
#include "GammaManager.h"
#include "PatternScroller.h"
#include "PatternRepeater.h"
#include "PatternController.h"

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
CRGBArray<NUM_LEDS> leds_top;

PaletteManager pm;
PatternController pc;
GammaManager Gamma;

struct_timers timing;
struct_config config;
struct_base_show_params baseParams;
struct_top_show_params topParams;


#endif
