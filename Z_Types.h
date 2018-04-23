#ifndef H_TYPES
#define H_TYPES

#include "PaletteManager.h"

#define NUM_BASE_PARAMS 9
typedef struct {
  uint8_t animation = INIT_BASE_ANIMATION;
  int8_t brightnessSpeed = INIT_BASE_BRIGHTNESS_SPEED;
  int8_t colorSpeed = INIT_BASE_COLOR_SPEED;
  uint8_t spacing = INIT_BASE_SPACING;
  uint8_t colorThickness = INIT_BASE_COLOR_THICKNESS_1;
  uint8_t brightLength = INIT_BASE_BRIGHT_LENGTH;
  uint8_t transLength = INIT_BASE_TRANS_THICKNESS;
  uint8_t numColors = INIT_BASE_NUM_COLORS;
  uint8_t displayMode = INIT_BASE_DISPLAY_MODE;
} struct_base_show_params;


#define NUM_TOP_PARAMS 3
typedef struct {
  uint8_t animation = INIT_TOP_ANIMATION;
  uint8_t portion = INIT_TOP_PORTION;
  int8_t speed = INIT_TOP_SPEED;
} struct_top_show_params;


#define NUM_LAYER_PARAMS 8
typedef struct {
  uint32_t basePauseLength = INIT_BASE_PAUSE_LENGTH;
  uint32_t baseTransOutLength = INIT_BASE_TRANS_OUT_LENGTH;
  uint32_t baseTransInLength = INIT_BASE_TRANS_IN_LENGTH;
  uint32_t baseParamWalkTime = INIT_BASE_PARAM_WALK;
  
  uint32_t topPauseLength = INIT_TOP_PAUSE_LENGTH;
  uint32_t topTransOutLength = INIT_TOP_TRANS_OUT_LENGTH;
  uint32_t topTransInLength = INIT_TOP_TRANS_IN_LENGTH;
  uint32_t topParamWalkTime = INIT_TOP_PARAM_WALK;
} struct_config;


typedef struct {
  uint32_t now = 0;
  uint32_t lastDraw = 0;
  uint32_t lastBaseParamWalk = 0, lastTopParamWalk = 0;
  uint32_t lastBaseTransition = 0, lastTopTransition = 0;
} struct_timers;


typedef struct {
  uint8_t a;
  uint8_t b;
  uint8_t blendAmount;
} PRGB;

class PatternHandler {
  public:
    virtual void SetCRGBs(CRGB* target, uint8_t numLEDs, PaletteManager& pm) = 0;
    virtual void SetColorPattern(PRGB* newPattern, uint8_t newColorPatternLength) = 0;
    virtual void SetBrightnessPattern(uint8_t* newPattern, uint32_t curTime) = 0;
    virtual void Init(uint32_t curTime) = 0;
    virtual void Update(uint32_t& newTime) = 0;
    virtual bool IsReadyForColorPatternChange() = 0;
    virtual bool IsReadyForBrightnessChange() = 0;
    virtual void SkipTime(uint32_t amount) = 0;
    virtual void BrightnessParametersChanged() = 0;
    virtual void ColorPatternParametersChanged() = 0;
    virtual uint8_t GetPeriod() = 0;
    
    virtual ~PatternHandler() {}

    uint8_t numColors, colorThickness, brightLength, spacing, transLength;
    int8_t colorSpeed, brightnessSpeed;
};

#endif
