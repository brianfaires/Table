#ifndef PATTERN_SCROLLER_H
#define PATTERN_SCROLLER_H

#include "GammaManager.h"
#include "PaletteManager.h"
#include "PatternRepeater.h"
#include "PatternGenerator.h"
#include "ArduinoTrace.h"

class PatternScroller {
  enum param_change_type { IMMEDIATE, ONCE_PER_MOVE, ONCE_PER_PERIOD };
   
  public:
    PatternScroller();
    void Init(PaletteManager* pm, GammaManager* gm, uint16_t numLEDs, uint8_t dimPatternIndex, uint8_t colorPatternIndex, struct_base_show_params& params, uint32_t curTime);
    bool Update(uint32_t curTime);
    void SkipTime(uint32_t amount);
    void SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs);
    void SetDisplayMode(uint8_t dimPattern, uint8_t colorPattern, uint32_t curTime);

    param_change_type dimParamChangeType;
    param_change_type colorParamChangeType;
    
    uint16_t colorPeriod, dimPeriod;
    int8_t dimSpeed, colorSpeed;
    uint8_t numColors, colorThickness, brightLength, transLength;
    uint32_t dimBlendLength, colorBlendLength;
    uint32_t dimPauseLength, colorPauseLength;
    
  private:
    PatternRepeater pr;
    PatternGenerator pg;

    bool WalkColorParams(uint32_t curTime);
    bool WalkDimParams(uint32_t curTime);
    void BlendColorPattern(uint32_t curTime);
    void BlendDimPattern(uint32_t curTime);
    bool IsReadyForDimMove(uint32_t curTime);
    bool IsReadyForColorMove(uint32_t curTime);
    bool IsStartOfColorPattern();
    bool IsStartOfDimPattern();
    bool ScrollPatterns(uint32_t curTime);

    uint16_t numLEDs;
    
    uint8_t oldDimPattern[NUM_LEDS], curDimPattern[NUM_LEDS], targetDimPattern[NUM_LEDS];
    PRGB oldColorPattern[NUM_LEDS], curColorPattern[NUM_LEDS], targetColorPattern[NUM_LEDS];

    uint8_t targetDimPatternIndex, targetColorPatternIndex;
    uint8_t oldDimPatternIndex, oldColorPatternIndex;
    uint8_t randomDimPatternIndex, randomColorPatternIndex;

    //uint32_t lastDimParamChange, lastColorParamChange;
    uint32_t lastDimPatternChange, lastColorPatternChange;
    uint32_t lastDimMove, lastColorMove;
};

#endif

