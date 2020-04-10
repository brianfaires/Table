#include "Globals.h"

// Timing debug tools
#ifdef TIMING_ANALYSIS
  uint32_t timingValues[NUM_TIMING_POINTS] = {0};
  uint32_t curDebugTime = 0;
  uint32_t lastDebugTime = 0;
  uint8_t curTiminingAnalysisPoint = 0;
#endif

#ifdef CHECK_FOR_CLIPPING
  uint32_t lastClippedTime = 0;
#endif

// Organizational structs for timers, configuration, and runtime parameters
struct_timers timing;
struct_config layerConfig;
struct_base_show_params baseParams;
struct_top_show_params topParams;

// Color and brightness outputs
CRGBArray<NUM_LEDS> leds;
uint8_t leds_b[NUM_LEDS];
uint8_t leds_5bit_brightness[NUM_LEDS];
CRGBArray<NUM_LEDS> leds_top;
uint8_t leds_top_b[NUM_LEDS];
uint8_t globalBrightness = INIT_GLOBAL_BRIGHTNESS;

// Objects
PaletteManager pm;
GammaManager Gamma;
PatternController pc;
