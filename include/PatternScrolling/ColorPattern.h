#pragma once
#include "ArduinoTrace.h"
#include "GammaManager.h"
#include "PaletteManager.h"

#define NUM_COLOR_PATTERNS 2
enum class ColorPatternName : uint8_t { GRADIENT, BLOCKS };

class ColorPattern {
  public:
    void Init(PaletteManager* _pm, GammaManager* _gm);
    uint8_t numColors, colorPeriod;
    void Draw(ColorPatternName pattern, CRGB* outputArray);

  private:
    PaletteManager* pm;
    GammaManager* Gamma;
};
