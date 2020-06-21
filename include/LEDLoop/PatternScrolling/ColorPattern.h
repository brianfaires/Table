#pragma once
#include "ArduinoTrace.h"
#include "LEDLoop/GammaManager.h"
#include "LEDLoop/PaletteManager.h"

enum class ColorPatternName : uint8_t { Gradient, Blocks, Count };
#define NUM_COLOR_PATTERNS uint8_t(ColorPatternName::Count)

class ColorPattern {
  public:
    void Init(PaletteManager* _pm, GammaManager* _gm);
    uint8_t numColors, colorPeriod;
    void Draw(ColorPatternName pattern, CRGB* outputArray);

  private:
    PaletteManager* pm;
    GammaManager* Gamma;
};
