#ifndef GLOBALS_H
#define GLOBALS_H

#include "A_Config.h"
#include "Z_Types.h"
#include "PaletteManager.h"
#include "PatternScroller.h"
#include "PatternRepeater.h"

#define FPS_TO_US(x) (1000000 / x)
#define FPS_TO_MS(x) (1000 / x)

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
PatternScroller ps;
PatternRepeater pr;

struct_timers timing;
struct_config config;
struct_base_show_params baseParams;
struct_top_show_params topParams;


#endif
