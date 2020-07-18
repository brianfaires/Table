#pragma once
#include "FastLED.h"
#include "config/Config.h"

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Animation Indexes //////////////////////////////////
#define NUM_BASE_ANIMATIONS 1
enum class BaseAnimation : uint8_t { None, Scroller, Fire, DiscoFire, Stacks, ColorExplosion, Orbs, GlimmerBands, CenterSpawn, Count };

#define NUM_TOP_ANIMATIONS 2
enum class TopAnimation : uint8_t { None, Glitter, Rain, Twinkle, Bounce, Comets, Collision, Count };
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//////// Internal timers: Use microseconds (1), or use milliseconds (0) //////////
#if 0 
  #define SYSTEM_TIME millis()
  #define ONE_SEC 1000
  #define IO_TIME_FACTOR 1
#else
  #define SYSTEM_TIME micros()
  #define ONE_SEC 1000000L
  #define IO_TIME_FACTOR 1000
#endif
#define ONE_MIN 60 * ONE_SEC
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
///////////////////// Structs and their sizes ////////////////////////////////////
#define NUM_BASE_PARAMS 9
struct struct_base_show_params {
  BaseAnimation animation = INIT_BASE_ANIMATION;
  int8_t dimSpeed = INIT_BASE_DIM_SPEED;
  int8_t colorSpeed = INIT_BASE_COLOR_SPEED;
  fract16 brightLength = INIT_BASE_BRIGHT_LENGTH; // Todo: these as floats or fracts?
  fract16 transLength = INIT_BASE_TRANS_LENGTH;
  uint8_t numColors = INIT_BASE_NUM_COLORS;
  uint8_t displayMode = INIT_BASE_DISPLAY_MODE;
  uint8_t dimPeriod = INIT_BASE_DIM_PERIOD;
  uint8_t colorPeriod = INIT_BASE_COLOR_PERIOD;
};

struct struct_scroller_params {
  BaseAnimation animation;
  int8_t dimSpeed;
  int8_t colorSpeed;
  int8_t brightLength;
  int8_t transLength;
  uint8_t numColors;
  uint8_t displayMode;
  uint8_t dimPeriod;
  uint8_t colorPeriod;
};

struct struct_stacker_params {
  uint16_t dimPeriod;
  uint8_t numColors;
};

#define NUM_TOP_PARAMS 3
struct struct_top_show_params {
  TopAnimation animation = INIT_TOP_ANIMATION;
  uint8_t portion = INIT_TOP_PORTION;
  int8_t speed = INIT_TOP_SPEED;
};

#define NUM_LAYER_PARAMS 8
struct struct_config {
  uint32_t basePauseLength = INIT_BASE_PAUSE_LENGTH;
  uint32_t baseTransOutLength = INIT_BASE_TRANS_OUT_LENGTH;
  uint32_t baseTransInLength = INIT_BASE_TRANS_IN_LENGTH;
  uint32_t baseParamWalkTime = INIT_BASE_PARAM_WALK;
  
  uint32_t topPauseLength = INIT_TOP_PAUSE_LENGTH;
  uint32_t topTransOutLength = INIT_TOP_TRANS_OUT_LENGTH;
  uint32_t topTransInLength = INIT_TOP_TRANS_IN_LENGTH;
  uint32_t topParamWalkTime = INIT_TOP_PARAM_WALK;
};

struct struct_timers {
  uint32_t now = 0;
  uint32_t lastDraw = 0;
  uint32_t lastBaseParamWalk = 0, lastTopParamWalk = 0;
  uint32_t lastBaseTransition = 0, lastTopTransition = 0;
};

struct base_animation {
  String name;
  void Draw(struct_base_show_params* baseParams, CRGB* leds, uint8_t* leds_b);
};

struct struct_stack {
    uint8_t length;
    uint16_t pixel;
    uint8_t color;
};

struct struct_comet{
  uint16_t startPos;
  bool moveForward;
};
