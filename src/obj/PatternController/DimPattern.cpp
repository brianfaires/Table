#include "obj/PatternController/DimPattern.h"

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
#define DRAW_DIM(x) for(limit += x; i < limit; i++) { outputArray[i] = 64; }
#define FADE_DOWN(x)  for(limit += x; i < limit; i++) { outputArray[i] = fadeStepSize * (limit - i); }
#define FADE_DOWN2(x) for(limit += x; i < limit; i++) { outputArray[i] = fadeStepSize2 * (limit - i); }
#define FADE_UP(x)  lastLimitMinusOne = limit - 1; for(limit += x; i < limit; i++) { outputArray[i] = fadeStepSize  * (uint8_t)(i - lastLimitMinusOne); }
#define FADE_UP2(x) lastLimitMinusOne = limit - 1; for(limit += x; i < limit; i++) { outputArray[i] = fadeStepSize2 * (uint8_t)(i - lastLimitMinusOne); }

PatternType DimPattern::getPatternType(DimPatternName pattern) { return allDimPatternTypes[int(pattern)]; }

DimPattern::DimPattern() {
  allDimPatternTypes[int(DimPatternName::Random)] =    PatternType::Symmetric;
  allDimPatternTypes[int(DimPatternName::Barbell)] =   PatternType::Symmetric;
  allDimPatternTypes[int(DimPatternName::Bowties)] =   PatternType::AllCenter;
  allDimPatternTypes[int(DimPatternName::Diamonds)] =  PatternType::AllCenter;
  allDimPatternTypes[int(DimPatternName::Comet_F)] =   PatternType::Front;
  allDimPatternTypes[int(DimPatternName::Comet_R)] =   PatternType::Reverse;
  allDimPatternTypes[int(DimPatternName::Slide_H)] =   PatternType::Symmetric;
  allDimPatternTypes[int(DimPatternName::Slide_L)] =   PatternType::Symmetric;
  allDimPatternTypes[int(DimPatternName::Towers_H)] =  PatternType::Symmetric;
  allDimPatternTypes[int(DimPatternName::Towers_L)] =  PatternType::Symmetric;
  allDimPatternTypes[int(DimPatternName::Snake)] =     PatternType::Symmetric;
  allDimPatternTypes[int(DimPatternName::Snake3)] =    PatternType::MixR;
  allDimPatternTypes[int(DimPatternName::Towers)] =    PatternType::Symmetric;
  allDimPatternTypes[int(DimPatternName::Comets3_F)] = PatternType::Front3;
  allDimPatternTypes[int(DimPatternName::Comets3_R)] = PatternType::Reverse3;
  allDimPatternTypes[int(DimPatternName::Two_Sided)] = PatternType::AllCenter;
}

void DimPattern::Draw(DimPatternName pattern, uint8_t* outputArray) {
  uint8_t i = 0;
  uint8_t limit = 0;

  //if(random(40)==1) { DUMP(transLength) DUMP(brightLength) }

  // Yay for magic numbers! Not sure of the best way to enforce this logic as configurable
  if(dimPeriod < 2*transLength + brightLength + MIN_SCROLLER_LIT_PLUS_ONE) { THROW_DUMP("Invalid dimPeriod!", dimPeriod) DUMP(transLength) DUMP(brightLength) }

  switch(pattern)
  {
    case DimPatternName::Comet_F:
    {
      SETUP_FADES(2*transLength+MIN_SCROLLER_LIT)
      FADE_UP(2*transLength+MIN_SCROLLER_LIT)
      DRAW_BRIGHT(brightLength)
      break;
    }
    case DimPatternName::Comet_R:
    {

      SETUP_FADE_DOWN(2*transLength+MIN_SCROLLER_LIT)
      DRAW_BRIGHT(brightLength)
      FADE_DOWN(2*transLength+MIN_SCROLLER_LIT)
      break;
    }
    case DimPatternName::Two_Sided:
    {
      SETUP_FADES(transLength+4);
      FADE_UP(transLength+4)
      DRAW_BRIGHT(brightLength+1)
      FADE_DOWN(transLength+4)
      break;
    }
    case DimPatternName::Barbell:// Reverses trans and bright to keep the 2*trans + bright period
    {
      uint8_t halfFade = (brightLength+MIN_SCROLLER_LIT) / 2;
      SETUP_FADES(halfFade);
      DRAW_BRIGHT(transLength)
      FADE_DOWN(halfFade)
      if(2*halfFade < brightLength+MIN_SCROLLER_LIT) { DRAW_SINGLE(fadeStepSize) }
      FADE_UP(halfFade)
      DRAW_BRIGHT(transLength)
      break;
    }
    case DimPatternName::Towers_H:
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
    case DimPatternName::Towers_L:
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
    case DimPatternName::Slide_H:
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
    case DimPatternName::Slide_L:
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
    case DimPatternName::Bowties:
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
    case DimPatternName::Diamonds:
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
    case DimPatternName::Towers:
    {
      DRAW_DIM(transLength+2)
      SINGLE_SPACE()
      DRAW_BRIGHT(brightLength+3)
      SINGLE_SPACE()
      DRAW_DIM(transLength+2)
      break;
    }
    case DimPatternName::Snake:
    {
      DRAW_BRIGHT(brightLength + 2*transLength + MIN_SCROLLER_LIT)
      break;
    }
    case DimPatternName::Snake3:
    {
      DRAW_BRIGHT(transLength+2)
      SINGLE_SPACE()
      
      DRAW_BRIGHT(brightLength+3)
      SINGLE_SPACE()
      
      DRAW_BRIGHT(transLength+2)
      break;
    }
    case DimPatternName::Comets3_F:
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
    case DimPatternName::Comets3_R:
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
    case DimPatternName::Random:
      DRAW_BRIGHT(dimPeriod) // Used to debug color patterns
      break;
    default:
      THROW_DUMP("Unrecognized DimPatternName", uint8_t(pattern))
  }
    FILL_SPACING()
}