#pragma once
#include "ArduinoTrace.h"
#include "GammaManager.h"
#include "PaletteManager.h"

class ColorPattern {
  public:
    static void Init(PaletteManager* _pm, GammaManager* _gm);
    static uint8_t numColors, colorPeriod;
    virtual void Draw(CRGB* outputArray);

  protected:
    static PaletteManager* pm;
    static GammaManager* Gamma;
};

#define NUM_COLOR_PATTERNS 2
enum class ColorPatternName : uint8_t { GRADIENT, BLOCKS };

class ColorPattern_Gradient     : public ColorPattern { void Draw(CRGB* outputArray); };
class ColorPattern_Blocks       : public ColorPattern { void Draw(CRGB* outputArray); };
