#pragma once
#include "ArduinoTrace.h"
#include "GammaManager.h"
#include "PaletteManager.h"

enum class PatternType : uint8_t { SYMMETRIC, FRONT, REVERSE, FRONT3, REVERSE3 };

class DimPattern {
  public:
    static uint8_t brightLength, transLength, dimPeriod;
    virtual void Draw(uint8_t* outputArray);
    PatternType patternType;
};

#define NUM_DIM_PATTERNS 16
enum class DimPatternName : uint8_t { NONE, COMET_F, COMET_R, TWO_SIDED, BARBELL, SLOPED_TOWERS_H, SLOPED_TOWERS_L, SLIDE_H, SLIDE_L, BOWTIES_F, BOWTIES_R, TOWERS, SNAKE, SNAKE3, THREE_COMETS_F, THREE_COMETS_R };

class DimPattern_Comet            : public DimPattern {
  PatternType patternType = PatternType::FRONT; void Draw(uint8_t* outputArray); };
class DimPattern_ReverseComet     : public DimPattern {
  PatternType patternType = PatternType::REVERSE; void Draw(uint8_t* outputArray); };

class DimPattern_3Comets          : public DimPattern {
  PatternType patternType = PatternType::FRONT3; void Draw(uint8_t* outputArray); };
class DimPattern_3ReverseComets   : public DimPattern {
  PatternType patternType = PatternType::REVERSE3; void Draw(uint8_t* outputArray); };

class DimPattern_Snake            : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC;  void Draw(uint8_t* outputArray); };
class DimPattern_TwoSided         : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_Barbell          : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_Snake3           : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_Towers           : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_Bowties          : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_ReverseBowties   : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_SlopedHighTowers : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_SlopedLowTowers  : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_SlideHigh        : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
class DimPattern_SlideLow         : public DimPattern {
  PatternType patternType = PatternType::SYMMETRIC; void Draw(uint8_t* outputArray); };
