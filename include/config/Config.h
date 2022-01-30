/* Config file for Table.h.  None of these values should appear in libraries, even if they're currently included in the project. Includes:
      1. Debugging/testing toggles
      2. Initial states settings
      3. Hardware config settings
*/

#pragma once
#include "FastLED.h"

#define INIT_GLOBAL_BRIGHTNESS 255

////////////////////////////////////////
////////// Debugging config ////////////
//#define SHOWCASE_MODE
//#define USING_DEBUG_BOARD
//#define DISABLE_INTERIOR_LOOP
#define DISABLE_UPPER_LOOP
#define MANUAL_PARAMS
#define ALLOW_ZERO_SPEED // For PatternController
//#define USE_DEV_PALETTES
//#define EXPLICIT_PARAMETERS
//#define CHECK_FOR_CLIPPING
//#define JUMP_DIM_PARAMS // For PatternScroller
//#define PULSE_BASE_PARAMS
//#define TEST_PALETTES
//#define TIMING_ANALYSIS
//#define DEBUG_COLOR_PATTERNS
//#define RUN_GAMMA_TESTS // Overrides everything else
//#define USE_MICROS_TIMER
#define DEBUG_BTN(msg)            //DEBUG(msg)
#define DEBUG_ANIMATION(msg)      //DEBUG(msg)
#define DEBUG_PATTERN_SPLITS(msg) DEBUG(msg)
#define DEBUG_TRANSITIONS(msg)    DEBUG(msg)
////////////////////////////////////////


/////////////////////////////////////////////
////////// Init Animation Settings //////////
#define INIT_BASE_ANIMATION BaseAnimation::Scroller
#define INIT_BASE_DIM_SPEED 34
#define INIT_BASE_COLOR_SPEED 0
#define INIT_BASE_BRIGHT_LENGTH 0xFFFF
#define INIT_BASE_TRANS_LENGTH 0xFFFF
#define INIT_BASE_NUM_COLORS 255
#define INIT_BASE_DISPLAY_MODE 176 // 0 + 16*4
#define INIT_BASE_DIM_PERIOD 120
#define INIT_BASE_COLOR_PERIOD 255

#define INIT_TOP_ANIMATION TopAnimation::None
#define INIT_TOP_PORTION 128
#define INIT_TOP_SPEED 64
/////////////////////////////////////////////


/////////////////////////////////////////////
//////// Init PaletteManager Settings ///////
#define INIT_PALETTE PaletteIndex(0)
#define INIT_PM_WALK_LENGTH   5 * ONE_SEC
#define INIT_PM_PAUSE_LENGTH  60L * ONE_MIN //2 * ONE_SEC
#define INIT_FADE_IN_FROM_BLACK false
/////////////////////////////////////////////


//////////////////////////////////////////////////
////////////// Init Layer Settings ///////////////
#define INIT_BASE_PAUSE_LENGTH      60L * ONE_MIN
#define INIT_BASE_TRANS_OUT_LENGTH  3 * ONE_SEC
#define INIT_BASE_TRANS_IN_LENGTH   3 * ONE_SEC
#define INIT_BASE_PARAM_WALK        60L * ONE_MIN
#define INIT_TOP_PAUSE_LENGTH       60L * ONE_MIN
#define INIT_TOP_TRANS_OUT_LENGTH   3 * ONE_SEC
#define INIT_TOP_TRANS_IN_LENGTH    3 * ONE_SEC
#define INIT_TOP_PARAM_WALK         60L * ONE_MIN
//////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//////////////////// Init PatternController Settings ///////////////////
#define INIT_PATTERN_CONTROLLER_DIM_PAUSE_LENGTH    1 * ONE_SEC
#define INIT_PATTERN_CONTROLLER_DIM_BLEND_LENGTH    1 * ONE_SEC
#define INIT_PATTERN_CONTROLLER_COLOR_PAUSE_LENGTH  30 * ONE_SEC
#define INIT_PATTERN_CONTROLLER_COLOR_BLEND_LENGTH  2 * ONE_SEC
#define INIT_PATTERN_SCROLLER_BRIGHTNESS 255
#define INIT_DIM_PARAM_CHANGE_TYPE DimPatternChangeType::Preferred
#define INIT_BASE_DIM_PARAM_CHANGE_TYPE BaseDimParamChangeType::Manual
#define INIT_CHANGE_DIM_PARAMS_WITH_MOVEMENT true
#define INIT_ENABLE_DOUBLE_BRIGHT_MOVE false
////////////////////////////////////////////////////////////////////////

#define MAX_LEDS 520
////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////
//////////// Hardware config ////////////
#define REFRESH_RATE 1000
#define STARTUP_DELAY_MS 1000
#define LED_DATA_RATE_MHZ 18
#define SERIAL_DATA_RATE_HZ 115200
#define BTN1_PIN 14
#define BTN2_PIN 15
#define NUM_BTNS 2
#ifdef USING_DEBUG_BOARD
  #define CLOCK_PIN 5
  #define LED_PIN 18
#else
  #define CLOCK_PIN_INTERIOR 13
  #define LED_PIN_INTERIOR 12
  #define NUM_LEDS_INTERIOR 408

  #define CLOCK_PIN_UPPER 14
  #define LED_PIN_UPPER 15
  #define NUM_LEDS_UPPER 420
#endif
/////////////////////////////////////////

