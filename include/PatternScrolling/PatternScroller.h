#pragma once
#include "Definitions.h"
#include "DimPattern.h"
#include "ColorPattern.h"

#define MAX_PERIOD 210

#define NUM_DIM_PARAM_CHANGE_TYPES 6
enum param_change_type { PREFERRED, GROW_F, GROW_R, WORM, FREEZE, CENTER, MIX_F }; //todo: use MIX_F and MIX_R; worm on bright and freeze on trans, and vice versa

class PatternScroller {
  public:
    PatternScroller();
    void Init(struct_base_show_params& params, uint32_t* curTime = NULL, PaletteManager* _pm = NULL, GammaManager* _gm = NULL, uint16_t _numLEDs = 0);
    void Clone(PatternScroller* source, struct_base_show_params& params);
    bool Update();
    void SkipTime(uint32_t amount);
    void SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs);
    bool IsStartOfDimPattern();

    // Getters/Setters
    void setDisplayMode(uint8_t displayMode);
    uint8_t getColorPeriod();
    uint8_t getDimPeriod();
    uint32_t getDimBlendLength();
    void setDimBlendLength(uint32_t value);
    uint32_t getColorBlendLength();
    void setColorBlendLength(uint32_t value);
    uint32_t getDimPauseLength();
    void setDimPauseLength(uint32_t value);
    uint32_t getColorPauseLength();
    void setColorPauseLength(uint32_t value);
    int8_t getDimSpeed();
    void setDimSpeed(int8_t value);
    int8_t getColorSpeed();
    void setColorSpeed(int8_t value);
    
    // Params
    param_change_type dimParamChangeType;
    bool changeDimParamsWithMovement;
    bool enableDoubleBrightMove;
    uint8_t brightness;
    uint8_t numColors, brightLength, transLength;
        
  private:
	  uint32_t* curTime;

    // Objects
    PaletteManager* pm;
    GammaManager* Gamma;
    DimPattern dimPattern;
    ColorPattern colorPattern;

    // Smoothing
    bool WalkColorParams();
    bool WalkDimParams();
    void BlendColorPattern();
    void BlendDimPattern();
    bool ScrollPatterns();
    void ScrollPatternsWithoutTimer(bool moveForward);
    param_change_type GetPreferredDimParamChangeType(uint8_t patternIndex, int8_t delta);

    // Utility
    bool IsReadyForDimMove();
    bool IsHalfwayToDimMove();
    uint8_t getTargetDimPatternIndex();
    bool IsRandomDimPattern();
    
    // Blending
    bool dimBlendOn = false;
    bool colorBlendOn = false;

    // Params
    uint8_t colorPeriod, dimPeriod;
    int8_t dimSpeed, colorSpeed;
    uint16_t numLEDs;
    uint32_t dimPauseLength, colorPauseLength;
    uint32_t dimBlendLength, colorBlendLength;

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

    void WriteDimPattern(uint8_t patternIndex, uint8_t* outputArray);
    void WriteColorPattern(uint8_t patternIndex, CRGB* outputArray);
};
