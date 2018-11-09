#pragma once
#include "GammaManager.h"
#include "PaletteManager.h"
#include "PatternRepeater.h"
#include "PatternGenerator.h"
#include "ArduinoTrace.h"
#include "PatternCommon.h"

class PatternScroller {
  enum param_change_type { IMMEDIATE, ONCE_PER_MOVE, ONCE_PER_PERIOD };
   
  public:
    PatternScroller();
    void Init(struct_base_show_params& params, uint32_t curTime, PaletteManager* _pm = NULL, GammaManager* gm = NULL, uint16_t _numLEDs = 0);
    bool Update(uint32_t curTime);
    void SkipTime(uint32_t amount);
    void SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs);
    void SetDisplayMode(struct_base_show_params& params, uint32_t curTime);
    void Clone(PatternScroller* source, struct_base_show_params& params, uint32_t curTime);
    bool IsStartOfColorPattern();
    bool IsStartOfDimPattern();

    param_change_type dimParamChangeType;
    param_change_type colorParamChangeType;
    
    uint16_t colorPeriod, dimPeriod;
    int8_t dimSpeed, colorSpeed;
    uint8_t numColors, brightLength, transLength;
    uint32_t dimBlendLength, colorBlendLength;
    uint32_t dimPauseLength, colorPauseLength;
        
  private:
    PaletteManager* pm;
    GammaManager* Gamma;
    PatternRepeater pr;
    PatternGenerator pg;

    bool WalkColorParams(uint32_t curTime);
    bool WalkDimParams(uint32_t curTime);
    void BlendColorPattern(uint32_t curTime);
    void BlendDimPattern(uint32_t curTime);
    bool IsReadyForDimMove(uint32_t curTime);
    bool IsReadyForColorMove(uint32_t curTime);
    bool ScrollPatterns(uint32_t curTime);

    uint16_t numLEDs;
    
    uint8_t oldDimPattern[MAX_PERIOD], curDimPattern[MAX_PERIOD], targetDimPattern[MAX_PERIOD];
    CRGB oldColorPattern[MAX_PERIOD], curColorPattern[MAX_PERIOD], targetColorPattern[MAX_PERIOD];

    uint8_t targetDimPatternIndex, targetColorPatternIndex;
    uint8_t oldDimPatternIndex, oldColorPatternIndex;
    uint8_t randomDimPatternIndex, randomColorPatternIndex;

    //uint32_t lastDimParamChange, lastColorParamChange;
    uint32_t lastDimMove, lastColorMove;
    uint32_t lastDimPatternChange, lastColorPatternChange;
};

