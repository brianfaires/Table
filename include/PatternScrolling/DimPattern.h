#pragma once
#include "ArduinoTrace.h"
#include "GammaManager.h"
#include "PaletteManager.h"

enum class PatternType : uint8_t { Symmetric, Front, Reverse, Front3, Reverse3, Count };
enum class DimPatternName : uint8_t { Random, Comet_F, Comet_R, Two_Sided, Barbell, Towers_H, Towers_L, Slide_H, Slide_L, Bowties_F, Bowties_R, Towers, Snake, Snake3, Comets3_F, Comets3_R, Count };
#define NUM_PATTERN_TYPES uint8_t(PatternType::Count)
#define NUM_DIM_PATTERNS uint8_t(DimPatternName::Count)

class DimPattern {
  public:
    DimPattern();
    void Draw(DimPatternName pattern, uint8_t* outputArray);
    PatternType getPatternType(DimPatternName pattern);

    uint8_t brightLength, transLength, dimPeriod;

  private:
    PatternType allDimPatternTypes[NUM_DIM_PATTERNS];
};
