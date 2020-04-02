#pragma once
#include "Definitions.h"
#include "PatternScroller.h"

#define NUM_PATTERN_PARAMS 5

class PatternController {
  public:
    PatternController();
    void Init(uint16_t _numLEDs, uint32_t* curTime, struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, const uint16_t* _allowedDimPeriods, const uint16_t* allowedColorPeriods);
    void Update(struct_base_show_params& params, CRGB* target, uint8_t* target_b);
    void SkipTime(uint32_t amount);

    uint32_t GetColorPauseLength();
    void SetColorPauseLength(uint32_t value);
    uint32_t GetColorBlendLength();
    void SetColorBlendLength(uint32_t value);
    uint32_t GetDimPauseLength();
    void SetDimPauseLength(uint32_t value);
    uint32_t GetDimBlendLength();
    void SetDimBlendLength(uint32_t value);
    param_change_type GetDimParamChangeType();
    bool GetChangeDimParamsWithMovement();
    bool GetEnableDoubleBrightMove();
    void SetEnableDoubleBrightMove(bool value);
    void SetDimParamChangeType(param_change_type value);
    void SetDimParamChangeType(bool changeDimParamsWithMovement);
    void SetDimParamChangeType(param_change_type value, bool changeDimParamsWithMovement);
    void SetBrightness(uint8_t brightness);
    
  private:
    PatternScroller ps1, ps2;
    PatternScroller *ps;
    PatternScroller *secondary;
    void ScaleParams(struct_base_show_params& params, struct_base_show_params& output, uint8_t dimPeriod = 0, uint8_t colorPeriod = 0);
    void WalkSpeeds();
    void StartSplit(struct_base_show_params& params);
    void EndSplit();

    bool splitDisplay = false;
    bool secondaryScrollerIsLow = true;
    uint16_t splitIndex = 0;
    uint16_t numLEDs;
    int8_t colorSpeed, dimSpeed;
    
    const uint16_t* allowedDimPeriods;
    const uint16_t* allowedColorPeriods;
};

