#pragma once
#include "PaletteManager.h"

#define NUM_BASE_PARAMS 9
struct struct_base_show_params {
  uint8_t animation = INIT_BASE_ANIMATION;
  int8_t dimSpeed = INIT_BASE_BRIGHTNESS_SPEED;
  int8_t colorSpeed = INIT_BASE_COLOR_SPEED;
  uint8_t brightLength = INIT_BASE_BRIGHT_LENGTH;
  uint8_t transLength = INIT_BASE_TRANS_THICKNESS;
  uint8_t numColors = INIT_BASE_NUM_COLORS;
  uint8_t displayMode = INIT_BASE_DISPLAY_MODE;
  uint8_t dimPeriod = INIT_BASE_DIM_PERIOD;
  uint8_t colorPeriod = INIT_BASE_COLOR_PERIOD;
};


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

