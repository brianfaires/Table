#pragma once
#include "ArduinoTrace.h"
#include "lib/GammaManager.h"
#include "lib/PaletteManager.h"

enum class PatternType : uint8_t { Symmetric, AllCenter, Front, Reverse, Front3, Reverse3, MixR, COUNT };
enum class DimPatternName : uint8_t { Random, Comet_F, Comet_R, Two_Sided, Barbell, Towers_H, Towers_L, Slide_H, Slide_L, Bowties, Diamonds, Towers, Snake, Snake3, Comets3_F, Comets3_R, COUNT };
#define NUM_PATTERN_TYPES uint8_t(PatternType::COUNT)
#define NUM_DIM_PATTERNS uint8_t(DimPatternName::COUNT)


#define MIN_SCROLLER_LIT 9
#define MIN_SCROLLER_LIT_PLUS_ONE 10

class DimPattern {
  public:
    DimPattern();
    void Draw(DimPatternName pattern, uint8_t* outputArray);
    PatternType getPatternType(DimPatternName pattern);

    uint8_t brightLength, transLength, dimPeriod;

  private:
    PatternType allDimPatternTypes[NUM_DIM_PATTERNS];
};
