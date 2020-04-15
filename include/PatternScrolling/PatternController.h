#pragma once
#include "Definitions.h"
#include "PatternScroller.h"

#define NUM_PATTERN_PARAMS 5

class PatternController {
  public:
    void Init(uint16_t _numLEDs, uint32_t* curTime, struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, const uint16_t* _allowedDimPeriods, const uint16_t* allowedColorPeriods);
    void Update(struct_base_show_params& params, CRGB* target, uint8_t* target_b);
    void SkipTime(uint32_t amount);

    uint32_t getColorPauseLength();
    void setColorPauseLength(uint32_t value);
    uint32_t getColorBlendLength();
    void setColorBlendLength(uint32_t value);
    uint32_t getDimPauseLength();
    void setDimPauseLength(uint32_t value);
    uint32_t getDimBlendLength();
    void setDimBlendLength(uint32_t value);
    DimParamChangeMode getDimParamChangeMode();
    bool getChangeDimParamsWithMovement();
    bool getEnableDoubleBrightMove();
    void setEnableDoubleBrightMove(bool value);
    void setDimParamChangeMode(DimParamChangeMode value);
    void setDimParamChangeMode(bool changeDimParamsWithMovement);
    void setDimParamChangeMode(DimParamChangeMode value, bool changeDimParamsWithMovement);
    uint8_t getBrightness();
    void setBrightness(uint8_t brightness);
    
  private:
    PatternScroller ps1, ps2;
    PatternScroller *ps;
    PatternScroller *secondary;
    void ScaleParams(struct_base_show_params& params, struct_base_show_params& output, uint8_t dimPeriod = 0, uint8_t colorPeriod = 0);
    void WalkSpeeds();
    void StartSplit(struct_base_show_params& params);
    void EndSplit(bool swap);

    bool splitDisplay = false;
    bool secondaryScrollerIsLow = true;
    int16_t splitIndex = 0;
    uint16_t numLEDs;
    int8_t colorSpeed, dimSpeed;
    
    const uint16_t* allowedDimPeriods;
    const uint16_t* allowedColorPeriods;
};

