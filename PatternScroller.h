#pragma once
#include "Arduino.h"
#include "FastLED.h"
#include "GammaManager.h"
#include "PaletteManager.h"
#include "PatternGenerator.h"

#include "ArduinoTrace.h"

#define MAX_PERIOD 210

enum param_change_type { GROW_F, GROW_R, WORM, FREEZE, CENTER, MIX_F }; // debug: use MIX_F and MIX_R; worm on bright and freeze on trans, and vice versa

class PatternScroller {
  struct changeParams {
    uint32_t dimBlendLength, colorBlendLength, dimPauseLength, colorPauseLength;
    uint8_t brightness;
    param_change_type dimParamChangeMode;
  };
  
  public:
    PatternScroller();
    void Init(struct_base_show_params& params, uint32_t curTime, PaletteManager* _pm = NULL, GammaManager* gm = NULL, uint16_t _numLEDs = 0);
    void Clone(PatternScroller* source, struct_base_show_params& params, uint32_t curTime);
    bool Update(uint32_t curTime);
    void SkipTime(uint32_t amount);
    void SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs);
    bool IsStartOfDimPattern();

    // Getters/Setters
    void SetDisplayMode(uint8_t displayMode, uint32_t curTime);
    uint8_t GetColorPeriod();
    uint8_t GetDimPeriod();
    uint32_t GetDimBlendLength();
    void SetDimBlendLength(uint32_t value);
    uint32_t GetColorBlendLength();
    void SetColorBlendLength(uint32_t value);
    uint32_t GetDimPauseLength();
    void SetDimPauseLength(uint32_t value, uint32_t curTime);
    uint32_t GetColorPauseLength();
    void SetColorPauseLength(uint32_t value, uint32_t curTime);
    int8_t GetDimSpeed();
    void SetDimSpeed(int8_t value, uint32_t curTime);
    int8_t GetColorSpeed();
    void SetColorSpeed(int8_t value, uint32_t curTime);
    
    // Params
    param_change_type dimParamChangeType;
    bool changeDimParamASAP;
    uint8_t brightness;
    uint8_t numColors, brightLength, transLength;
        
  private:
    // Objects
    PaletteManager* pm;
    GammaManager* Gamma;
    PatternGenerator pg;

    // Smoothing
    bool WalkColorParams(uint32_t curTime);
    bool WalkDimParams(uint32_t curTime);
    void BlendColorPattern(uint32_t curTime);
    void BlendDimPattern(uint32_t curTime);
    bool ScrollPatterns(uint32_t curTime);
    bool ScrollPatternsWithoutTimer(bool moveForward);

    // Utility
    bool IsReadyForDimMove(uint32_t curTime);
    bool IsHalfwayToDimMove(uint32_t curTime);
    uint8_t GetTargetDimPatternIndex();
    bool IsRandomDimPattern();
    
    // Blending
    bool dimBlendOn = false;
    bool colorBlendOn = false;
    bool dimParamWalkedThisCycle = false;

    // Params
    uint8_t colorPeriod, dimPeriod;
    int8_t dimSpeed, colorSpeed;
    uint16_t numLEDs;
    uint32_t dimPauseLength, colorPauseLength;
    uint32_t dimBlendLength, colorBlendLength, dimBlendLength_q, colorBlendLength_q;
    bool dimBlendLength_queued, colorBlendLength_queued; // debug: these could be replaced by determining current blendAmount and adjusting last___PatternChange accordingly

    // Display mode
    uint8_t oldDimPatternIndex;
    uint8_t targetDimPatternIndex, targetColorPatternIndex;
    uint8_t randomDimPatternIndex, randomColorPatternIndex;

    // Scrolling patterns
    uint8_t colorIndexFirst;
    uint8_t dimIndexFirst;
    uint8_t oldDimPattern[MAX_PERIOD], curDimPattern[MAX_PERIOD], targetDimPattern[MAX_PERIOD];
    CRGB oldColorPattern[MAX_PERIOD], curColorPattern[MAX_PERIOD], targetColorPattern[MAX_PERIOD];

    // Timers
    uint32_t lastDimMove, lastColorMove;
    uint32_t lastDimPatternChange, lastColorPatternChange;
};

