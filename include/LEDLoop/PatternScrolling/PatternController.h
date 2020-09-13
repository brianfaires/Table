#pragma once
#include "Definitions.h"
#include "PatternScroller.h"

#define NUM_PATTERN_PARAMS 5

class PatternController {
  public:
    void Init(uint16_t _numLEDs, uint32_t* curTime, struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, std::vector<uint16_t> _allowedDimPeriods, std::vector<uint16_t> allowedColorPeriods, uint8_t numAllowedDimPeriods, uint8_t numAllowedColorPeriods);
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
    DimPatternChangeType getDimPatternChangeType();
    bool getChangeDimParamsWithMovement();
    bool getEnableDoubleBrightMove();
    void setEnableDoubleBrightMove(bool value);
    void setDimPatternChangeType(DimPatternChangeType value);
    void setDimPatternChangeType(bool changeDimParamsWithMovement);
    void setDimPatternChangeType(DimPatternChangeType value, bool changeDimParamsWithMovement);
    void setBaseDimParamChangeType(BaseDimParamChangeType value);
    uint8_t getBrightness();
    void setBrightness(uint8_t brightness);
    void setDimIndexOffset(uint16_t value);
    uint16_t getDimIndexOffset();
    void setColorIndexOffset(uint16_t value);
    uint16_t getColorIndexOffset();

    void BeginDimBlend();
    void BeginColorBlend();
    
    uint8_t GenerateDisplayModeValue(DimPatternName dimPatt, ColorPatternName colPatt);
    void setManualBlocks(uint8_t* _colorIndexes, uint8_t _numColorIndexes, uint16_t _dimPeriod);
    uint8_t* getManualBlocks();

    bool syncScrollingSpeeds = false;
    
    PatternScroller *ps;

  private:
    uint8_t NUM_ALLOWED_DIM_PERIODS;
    uint8_t NUM_ALLOWED_COLOR_PERIODS;
    PatternScroller ps1, ps2;
    PatternScroller *secondary;
    void ScaleParams(struct_base_show_params& params, struct_scroller_params& output, uint8_t dimPeriod = 0, uint8_t colorPeriod = 0);
    void WalkSpeeds();
    void StartSplit(struct_scroller_params& params);
    void EndSplit(bool swap);

    bool splitDisplay = false;
    bool secondaryScrollerIsLow = true;
    int16_t splitIndex = 0;
    uint16_t numLEDs;
    int8_t colorSpeed, dimSpeed;

    bool manualControl = false;

    std::vector<uint16_t> allowedDimPeriods;
    std::vector<uint16_t> allowedColorPeriods;
};

