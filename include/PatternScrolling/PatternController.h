#pragma once
#include "Definitions.h"
#include "PatternScroller.h"

#define NUM_PATTERN_PARAMS 5

struct struct_patternScrollerParams {
  uint32_t brightness;
  uint32_t colorPauseLength;
  uint32_t dimPauseLength;
  uint32_t colorBlendLength;
  uint32_t dimBlendLength;

  bool enableDoubleBrightMove;
  bool changeDimParamsWithMovement;
  DimPatternChangeType dimPatternChangeType;
  BaseDimParamChangeType baseDimParamChangeType;
};

class PatternController {
  public:
    struct_patternScrollerParams scrollParams;

    void Init(uint16_t _numLEDs, uint32_t* curTime, struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, const uint16_t* _allowedDimPeriods, const uint16_t* allowedColorPeriods);
    void Update(struct_base_show_params& params, CRGB* target, uint8_t* target_b);
    void SkipTime(uint32_t amount);

    
  private:
    PatternScroller ps1, ps2;
    PatternScroller *ps, *secondary;
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

