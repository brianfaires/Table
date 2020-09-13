#pragma once
#include "ArduinoTrace.h"
#include "LEDLoop/GammaManager.h"
#include "LEDLoop/PaletteManager.h"

enum class ColorPatternName : uint8_t { Gradient, Blocks, ManualBlocks, Count };
#define NUM_COLOR_PATTERNS uint8_t(ColorPatternName::Count)

const uint8_t MAX_NUM_DIM_PERIODS = 84;

class ColorPattern {
  public:
    void Init(PaletteManager* _pm, GammaManager* _gm);
    uint8_t numColors;
    uint16_t colorPeriod;
    void Draw(ColorPatternName pattern, CRGB* outputArray);
    
    void setManualBlocks(uint8_t* _colorIndexes, uint8_t _numColorIndexes, uint16_t _dimPeriod);
    uint8_t* getManualBlocks();

  private:
    PaletteManager* pm;
    GammaManager* Gamma;

    uint16_t dimPeriod;
    uint8_t numColorIndexes = 0;
    uint8_t colorIndexes[MAX_NUM_DIM_PERIODS];
};
