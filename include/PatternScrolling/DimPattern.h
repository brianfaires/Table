#pragma once
#include "ArduinoTrace.h"
#include "GammaManager.h"
#include "PaletteManager.h"

class DimPattern {
  public:
    static uint8_t brightLength, transLength, dimPeriod;
    virtual void Draw(uint8_t* outputArray);
};

#define NUM_DIM_PATTERNS 16
enum class DimPatternName : uint8_t { NONE, COMET_F, COMET_R, TWO_SIDED, BARBELL, SLOPED_TOWERS_H, SLOPED_TOWERS_L, SLIDE_H, SLIDE_L, BOWTIES_F, BOWTIES_R, TOWERS, SNAKE, SNAKE3, THREE_COMETS_F, THREE_COMETS_R };

class DimPattern_Comet            : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_ReverseComet     : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_TwoSided         : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_Barbell          : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_Snake            : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_Snake3           : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_3Comets          : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_3ReverseComets   : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_Towers           : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_Bowties          : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_ReverseBowties   : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_SlopedHighTowers : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_SlopedLowTowers  : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_SlideHigh        : public DimPattern { void Draw(uint8_t* outputArray); };
class DimPattern_SlideLow         : public DimPattern { void Draw(uint8_t* outputArray); };
