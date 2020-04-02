#pragma once
#include "Config.h"

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Animation Indexes //////////////////////////////////
#define NONE 0

#define SCROLLER 1
#define FIRE 2
#define DISCO_FIRE 3
#define STACKS 4
#define COLOR_EXPLOSION 5
#define ORBS 6
#define GLIMMER_BANDS 7
#define CENTER_SPAWN 8
#define NUM_BASE_ANIMATIONS 1

#define GLITTER 1
#define RAIN 2
#define TWINKLE 3
#define BOUNCE 4
#define COMETS 5
#define COLLISION 6
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
#define NUM_TOP_ANIMATIONS 2 // Todo: Find the right place for this

#define NUM_BASE_PARAMS 9
struct struct_base_show_params {
  uint8_t animation = INIT_BASE_ANIMATION;
  int8_t dimSpeed = INIT_BASE_DIM_SPEED;
  int8_t colorSpeed = INIT_BASE_COLOR_SPEED;
  uint8_t brightLength = INIT_BASE_BRIGHT_LENGTH;
  uint8_t transLength = INIT_BASE_TRANS_LENGTH;
  uint8_t numColors = INIT_BASE_NUM_COLORS;
  uint8_t displayMode = INIT_BASE_DISPLAY_MODE;
  uint8_t dimPeriod = INIT_BASE_DIM_PERIOD;
  uint8_t colorPeriod = INIT_BASE_COLOR_PERIOD;
};

#define NUM_PM_PARAMS 2 // Todo: Define this in PaletteManager

#define NUM_TOP_PARAMS 3
struct struct_top_show_params {
  uint8_t animation = INIT_TOP_ANIMATION;
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


struct struct_stacker {
  uint16_t startPixel;
  uint16_t endPixel;
  bool moveForward;
  uint8_t stackSize;
  uint8_t spacing;
  uint8_t numColors;
  uint16_t moveSpeed;
  
  uint8_t numStacks;
  uint16_t moveIndex;
  
  uint32_t lastMove = 0;
};


struct struct_comet{
  uint16_t startPos;
  bool moveForward;
};
