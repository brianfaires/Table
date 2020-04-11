#include "PatternScrolling\DimPattern.h"

#define GET_FADE_STEP_SIZE(x) 255.0f / (x+1)
#define SETUP_FADE_DOWN(x)  float fadeStepSize = GET_FADE_STEP_SIZE(x);
#define SETUP_FADES(x) float fadeStepSize = GET_FADE_STEP_SIZE(x); uint8_t lastLimitMinusOne;
#define REBUILD_FADE(x) fadeStepSize = GET_FADE_STEP_SIZE(x);
#define SETUP_FADE2(x) float fadeStepSize2 = GET_FADE_STEP_SIZE(x);

#define DRAW_SINGLE(x) outputArray[i] = x; i++; limit++;
#define SINGLE_SPACE() DRAW_SINGLE(0)
#define SINGLE_BRIGHT() DRAW_SINGLE(255);
#define FILL_SPACING() while(i < dimPeriod) { outputArray[i] = 0; i++; }
#define DRAW_SPACING(x) for(limit += x; i < limit; i++) { outputArray[i] = 0; }

#define DRAW_BRIGHT(x) for(limit += x; i < limit; i++) { outputArray[i] = 255; }
#define DRAW_DIM(x) for(limit += x; i < limit; i++) { outputArray[i] = 32; }
#define FADE_DOWN(x)  for(limit += x; i < limit; i++) { outputArray[i] = fadeStepSize * (limit - i); }
#define FADE_DOWN2(x) for(limit += x; i < limit; i++) { outputArray[i] = fadeStepSize2 * (limit - i); }
#define FADE_UP(x)  lastLimitMinusOne = limit - 1; for(limit += x; i < limit; i++) { outputArray[i] = fadeStepSize  * (uint8_t)(i - lastLimitMinusOne); }
#define FADE_UP2(x) lastLimitMinusOne = limit - 1; for(limit += x; i < limit; i++) { outputArray[i] = fadeStepSize2 * (uint8_t)(i - lastLimitMinusOne); }

PatternType DimPattern::getPatternType(DimPatternName pattern) { return allDimPatternTypes[int(pattern)]; }

DimPattern::DimPattern() {
  allDimPatternTypes[int(DimPatternName::NONE)] =            PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::BARBELL)] =         PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::BOWTIES_F)] =       PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::BOWTIES_R)] =       PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::COMET_F)] =         PatternType::FRONT;
  allDimPatternTypes[int(DimPatternName::COMET_R)] =         PatternType::REVERSE;
  allDimPatternTypes[int(DimPatternName::SLIDE_H)] =         PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::SLIDE_L)] =         PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::SLOPED_TOWERS_H)] = PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::SLOPED_TOWERS_L)] = PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::SNAKE)] =           PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::SNAKE3)] =          PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::TOWERS)] =          PatternType::SYMMETRIC;
  allDimPatternTypes[int(DimPatternName::THREE_COMETS_F)] =  PatternType::FRONT3;
  allDimPatternTypes[int(DimPatternName::THREE_COMETS_R)] =  PatternType::REVERSE3;
  allDimPatternTypes[int(DimPatternName::TWO_SIDED)] =       PatternType::SYMMETRIC;
}

void DimPattern::Draw(DimPatternName pattern, uint8_t* outputArray) {
  uint8_t i = 0;
  uint8_t limit = 0; 

  switch(pattern)
  {
    case DimPatternName::COMET_F:
    {
      SETUP_FADES(2*transLength+9)
      FADE_UP(2*transLength+9)
      DRAW_BRIGHT(brightLength)
      break;
    }
    case DimPatternName::COMET_R:
    {

      SETUP_FADE_DOWN(2*transLength+9)
      DRAW_BRIGHT(brightLength)
      FADE_DOWN(2*transLength+9)
      break;
    }
    case DimPatternName::TWO_SIDED:
    {
      SETUP_FADES(transLength+4);
      FADE_UP(transLength+4)
      DRAW_BRIGHT(brightLength+1)
      FADE_DOWN(transLength+4)
      break;
    }
    case DimPatternName::BARBELL:// Reverses trans and bright to keep the 2*trans + bright period
    {
      uint8_t halfFade = (brightLength+9) / 2;
      SETUP_FADES(halfFade);
      DRAW_BRIGHT(transLength)
      FADE_DOWN(halfFade)
      if(2*halfFade < brightLength+9) { DRAW_SINGLE(fadeStepSize) }
      FADE_UP(halfFade)
      DRAW_BRIGHT(transLength)
    }
    case DimPatternName::SLOPED_TOWERS_H:
    {
      SETUP_FADES(transLength+2)
      SINGLE_BRIGHT()
      FADE_DOWN(transLength+2)
      SINGLE_SPACE()
      DRAW_BRIGHT(brightLength+1)
      SINGLE_SPACE()
      FADE_UP(transLength+2)
      SINGLE_BRIGHT()
      break;
    }
    case DimPatternName::SLOPED_TOWERS_L:
    {
      SETUP_FADES(transLength+2)
      SINGLE_BRIGHT()
      FADE_DOWN(transLength+2)
      SINGLE_SPACE()
      DRAW_DIM(brightLength+1)
      SINGLE_SPACE()
      FADE_UP(transLength+2)
      SINGLE_BRIGHT()
      FILL_SPACING()
      break;
    }
    case DimPatternName::SLIDE_H:
    {
      SETUP_FADES(transLength+2)
      FADE_UP(transLength+2)
      SINGLE_BRIGHT()
      SINGLE_SPACE()
      DRAW_BRIGHT(brightLength+1)
      SINGLE_SPACE()
      SINGLE_BRIGHT()
      FADE_DOWN(transLength+2)
      break;
    }
    case DimPatternName::SLIDE_L:
    {
      SETUP_FADES(transLength+2)
      FADE_UP(transLength+2)
      SINGLE_BRIGHT()
      SINGLE_SPACE()
      DRAW_DIM(brightLength+1)
      SINGLE_SPACE()
      SINGLE_BRIGHT()
      FADE_DOWN(transLength+2)
      break;
    }
    case DimPatternName::BOWTIES_F:
    {
      uint8_t transOuter = (transLength+3) / 2;
      uint8_t transInner = transLength + 3 - transOuter;

      SETUP_FADES(transOuter)
      SETUP_FADE2(transInner)

      SINGLE_BRIGHT()
      FADE_DOWN(transOuter)
      FADE_UP2(transInner)
      DRAW_BRIGHT(brightLength+1)
      FADE_DOWN2(transInner)
      FADE_UP(transOuter)
      SINGLE_BRIGHT()
      break;
    }
    case DimPatternName::BOWTIES_R:
    {
      uint8_t transOuter = (transLength+2) / 2;
      uint8_t transInner = transLength + 2 - transOuter;

      SETUP_FADES(transOuter)
      SETUP_FADE2(transInner)

      FADE_UP(transOuter)
      SINGLE_BRIGHT()
      FADE_DOWN2(transInner)
      SINGLE_SPACE()
      DRAW_BRIGHT(brightLength+1)
      SINGLE_SPACE()
      FADE_UP2(transInner)
      SINGLE_BRIGHT()
      FADE_DOWN(transOuter)
      break;
    }
    case DimPatternName::TOWERS:
    {
      DRAW_DIM(transLength+2)
      SINGLE_SPACE()
      DRAW_BRIGHT(brightLength+3)
      SINGLE_SPACE()
      DRAW_DIM(transLength+2)
      break;
    }
    case DimPatternName::SNAKE:
    {
      DRAW_BRIGHT(brightLength + 2*transLength + 9)
      break;
    }
    case DimPatternName::SNAKE3:
    {
      DRAW_BRIGHT(transLength+2)
      SINGLE_SPACE()
      
      DRAW_BRIGHT(brightLength+3)
      SINGLE_SPACE()
      
      DRAW_BRIGHT(transLength+2)
      break;
    }
    case DimPatternName::THREE_COMETS_F:
    {
      SETUP_FADES(transLength+1)
      SETUP_FADE2(brightLength+2)
      
      FADE_UP(transLength+1)
      SINGLE_BRIGHT()
      SINGLE_SPACE()
      
      FADE_UP2(brightLength+2)
      SINGLE_BRIGHT()
      SINGLE_SPACE()
      
      FADE_UP(transLength+1)
      SINGLE_BRIGHT()
      break;
    }
    case DimPatternName::THREE_COMETS_R:
    {
      SETUP_FADE_DOWN(transLength+1)
      SETUP_FADE2(brightLength+2)
      
      SINGLE_BRIGHT()
      FADE_DOWN(transLength+1)
      SINGLE_SPACE()
      
      SINGLE_BRIGHT()
      FADE_DOWN2(brightLength+2)
      SINGLE_SPACE()
      
      SINGLE_BRIGHT()
      FADE_DOWN(transLength+1)
      break;
    }
    default:
      THROW_DUMP("Unrecognized DimPatternName", uint8_t(pattern))
  }
    FILL_SPACING()
}