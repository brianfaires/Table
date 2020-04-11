#pragma once
#include "ArduinoTrace.h"
#include "GammaManager.h"
#include "PaletteManager.h"

enum class PatternType : uint8_t { SYMMETRIC, FRONT, REVERSE, FRONT3, REVERSE3 };
enum class DimPatternName : uint8_t { NONE, COMET_F, COMET_R, TWO_SIDED, BARBELL, SLOPED_TOWERS_H, SLOPED_TOWERS_L, SLIDE_H, SLIDE_L, BOWTIES_F, BOWTIES_R, TOWERS, SNAKE, SNAKE3, THREE_COMETS_F, THREE_COMETS_R };
#define NUM_DIM_PATTERNS 16

class DimPattern {
  public:
    DimPattern();
    void Draw(DimPatternName pattern, uint8_t* outputArray);
    PatternType getPatternType(DimPatternName pattern);

    uint8_t brightLength, transLength, dimPeriod;

  private:
    PatternType allDimPatternTypes[NUM_DIM_PATTERNS];
};
