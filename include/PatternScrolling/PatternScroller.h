#pragma once
#include "Definitions.h"
#include "DimPattern.h"
#include "ColorPattern.h"

#define MAX_PERIOD 210

enum class DimParamChangeMode { Preferred, Grow_F, Grow_R, Worm, Freeze, Center, Mix_F, Mix_R, Count };
#define NUM_DIM_PARAM_CHANGE_TYPES uint8_t(DimParamChangeMode::Count)

class PatternScroller {
  public:
    PatternScroller();
    void Init(struct_base_show_params& params, uint32_t* curTime = NULL, PaletteManager* _pm = NULL, GammaManager* _gm = NULL, uint16_t _numLEDs = 0);
    void Clone(PatternScroller* source, struct_base_show_params& params);
    bool Update(int8_t& shiftAmount);
    void SkipTime(uint32_t amount);
    void SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs, uint16_t numLEDsToSkip=0);
    bool IsStartOfDimPattern();
    bool IsReadyForDimMove();

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
    uint8_t getNumBlanks();
        
    // Params
    DimParamChangeMode dimParamChangeMode;
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
    bool WalkDimParams(int8_t& shiftAmount);
    void BlendColorPattern();
    void BlendDimPattern();
    bool ScrollPatterns();
    void ScrollPatternsWithoutTimer(bool moveForward);
    DimParamChangeMode GetPreferredDimParamChangeMode(uint8_t patternIndex, int8_t delta);

    // Utility
    bool IsHalfwayToDimMove();
    uint8_t getTargetDimPatternIndex();
    bool IsRandomDimPattern();
    void WriteDimPattern(uint8_t patternIndex, uint8_t* outputArray);
    void WriteColorPattern(uint8_t patternIndex, CRGB* outputArray);

    
    // Blending
    bool dimBlendOn = false;
    bool colorBlendOn = false;
    bool dimParamWalkedThisCycle = false;

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
};
