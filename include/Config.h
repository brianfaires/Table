#pragma once
#include "FastLED.h"

#define INIT_GLOBAL_BRIGHTNESS 128

////////////////////////////////////////
//////////// Runtime config ////////////
#define MANUAL_PARAMS
//#define TEST_PALETTES
//#define TEST_COLOR_CORRECTION // Overrides everything else
//#define EXPLICIT_PARAMETERS
////////////////////////////////////////


////////////////////////////////////////
//////////// Debugging Level ///////////
#define DEBUG_BTN(msg)        DEBUG(msg)
#define DEBUG_ANIMATION(msg)  //DEBUG(msg)
#define DEBUG_TIMING(msg)     //DEBUG(msg)
#define DEBUG_CLIPPING(msg)   //THROW(msg)
////////////////////////////////////////


/////////////////////////////////////////////
////////// Init Animation Settings //////////
#define INIT_BASE_ANIMATION BaseAnimation::Scroller
#define INIT_BASE_DIM_SPEED 16
#define INIT_BASE_COLOR_SPEED 64
#define INIT_BASE_BRIGHT_LENGTH 64
#define INIT_BASE_TRANS_LENGTH 240
#define INIT_BASE_NUM_COLORS 200
#define INIT_BASE_DISPLAY_MODE 0
#define INIT_BASE_DIM_PERIOD 16
#define INIT_BASE_COLOR_PERIOD 255

#define INIT_TOP_ANIMATION TopAnimation::None
#define INIT_TOP_PORTION 128
#define INIT_TOP_SPEED 64
/////////////////////////////////////////////


/////////////////////////////////////////////
//////// Init PaletteManager Settings ///////
#define INIT_PALETTE static_cast<PaletteIndex>(8)
#define INIT_PM_WALK_LENGTH   3 * ONE_SEC
#define INIT_PM_PAUSE_LENGTH  0 * ONE_MIN
#define INIT_FADE_IN_FROM_BLACK false
/////////////////////////////////////////////


//////////////////////////////////////////////////
////////////// Init Layer Settings ///////////////
#define INIT_BASE_PAUSE_LENGTH      30 * ONE_MIN
#define INIT_BASE_TRANS_OUT_LENGTH  3 * ONE_SEC
#define INIT_BASE_TRANS_IN_LENGTH   3 * ONE_SEC
#define INIT_BASE_PARAM_WALK        30 * ONE_MIN
#define INIT_TOP_PAUSE_LENGTH       30 * ONE_MIN
#define INIT_TOP_TRANS_OUT_LENGTH   3 * ONE_SEC
#define INIT_TOP_TRANS_IN_LENGTH    3 * ONE_SEC
#define INIT_TOP_PARAM_WALK         30 * ONE_MIN
//////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//////////////////// Init PatternController Settings ///////////////////
#define INIT_PATTERN_CONTROLLER_DIM_PAUSE_LENGTH    5 * ONE_SEC
#define INIT_PATTERN_CONTROLLER_DIM_BLEND_LENGTH    1.5 * ONE_SEC
#define INIT_PATTERN_CONTROLLER_COLOR_PAUSE_LENGTH  2 * ONE_SEC
#define INIT_PATTERN_CONTROLLER_COLOR_BLEND_LENGTH  1 * ONE_SEC
#define INIT_PATTERN_SCROLLER_BRIGHTNESS 255
#define INIT_DIM_PARAM_CHANGE_TYPE WORM
#define INIT_CHANGE_DIM_PARAMS_WITH_MOVEMENT true
#define INIT_ENABLE_DOUBLE_BRIGHT_MOVE false
////////////////////////////////////////////////////////////////////////


#define NUM_LEDS 520
#if NUM_LEDS == 420
  // 2*2*3*5*6*7
  #define NUM_ALLOWED_DIM_PERIODS 10
  #define NUM_ALLOWED_COLOR_PERIODS 11
  const uint16_t PROGMEM allowedDimPeriods[] = { 21, 28, 30, 35, 42, 60, 70, 84, 105, 210 };
  const uint16_t PROGMEM allowedColorPeriods[] = { 20, 21, 28, 30, 35, 42, 60, 70, 84, 105, 210 };
#elif NUM_LEDS == 408
  // 2*2*2*3*17
  #define NUM_ALLOWED_DIM_PERIODS 7
  #define NUM_ALLOWED_COLOR_PERIODS 7
  const uint16_t PROGMEM allowedDimPeriods[] = { 17, 24, 34, 51, 68, 102, 204 };
  const uint16_t PROGMEM allowedColorPeriods[] = { 17, 24, 34, 51, 68, 102, 204 };
#elif NUM_LEDS == 520
  // 2*2*2*5*13
  #define NUM_ALLOWED_DIM_PERIODS 7
  #define NUM_ALLOWED_COLOR_PERIODS 7
  const uint16_t PROGMEM allowedDimPeriods[] = { 20, 26, 40, 52, 65, 104, 130 };
  const uint16_t PROGMEM allowedColorPeriods[] = { 20, 26, 40, 52, 65, 104, 130 };
#endif
////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////
//////////// Hardware config ////////////
#define REFRESH_RATE 100 // Todo: investigate - This once had flickering at 100FPS, no idea why - may be APA102 manufacturer (See FastLED FAQ)
#define STARTUP_DELAY_MS 1000
#define LED_DATA_RATE_MHZ 24
#define SERIAL_DATA_RATE_HZ 115200
#define CLOCK_PIN 13
#define LED_PIN 12
#define BTN1_PIN 14
#define BTN2_PIN 15
#define NUM_BTNS 2
/////////////////////////////////////////

