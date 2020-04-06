#include "PatternScrolling\DimPattern.h"

#define GET_FADE_STEP_SIZE(x) 255.0f / (x+1)
#define SETUP_DIM_PATTERN()  uint8_t i = 0; uint8_t limit = 0; 
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


// Instantiate static members of base class
uint8_t DimPattern::brightLength;
uint8_t DimPattern::transLength;
uint8_t DimPattern::dimPeriod;


void DimPattern::Draw(uint8_t* outputArray) {
  THROW("Base class function should never be caled!")
  SETUP_DIM_PATTERN()
  DRAW_BRIGHT(dimPeriod)
}

void DimPattern_Comet::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADES(2*transLength+9)
  
  FADE_UP(2*transLength+9)
  DRAW_BRIGHT(brightLength)
  FILL_SPACING()
}

void DimPattern_ReverseComet::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADE_DOWN(2*transLength+9)
  
  DRAW_BRIGHT(brightLength)
  FADE_DOWN(2*transLength+9)
  FILL_SPACING()
}

void DimPattern_TwoSided::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADES(transLength+4);
  
  FADE_UP(transLength+4)
  DRAW_BRIGHT(brightLength+1)
  FADE_DOWN(transLength+4)
  FILL_SPACING()
}

// Reverses trans and bright to keep the 2*trans + bright period
void DimPattern_Barbell::Draw(uint8_t* outputArray) {
  uint8_t halfFade = (brightLength+9) / 2;
  SETUP_DIM_PATTERN()
  SETUP_FADES(halfFade);
  
  DRAW_BRIGHT(transLength)
  FADE_DOWN(halfFade)
  if(2*halfFade < brightLength+9) { DRAW_SINGLE(fadeStepSize) }
  FADE_UP(halfFade)
  DRAW_BRIGHT(transLength)
  FILL_SPACING()
}

void DimPattern_SlopedHighTowers::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADES(transLength+2)
  
  SINGLE_BRIGHT()
  FADE_DOWN(transLength+2)
  SINGLE_SPACE()
  DRAW_BRIGHT(brightLength+1)
  SINGLE_SPACE()
  FADE_UP(transLength+2)
  SINGLE_BRIGHT()
  FILL_SPACING()
}

void DimPattern_SlopedLowTowers::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADES(transLength+2)
  
  SINGLE_BRIGHT()
  FADE_DOWN(transLength+2)
  SINGLE_SPACE()
  DRAW_DIM(brightLength+1)
  SINGLE_SPACE()
  FADE_UP(transLength+2)
  SINGLE_BRIGHT()
  FILL_SPACING()
}

void DimPattern_SlideHigh::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADES(transLength+2)
  
  FADE_UP(transLength+2)
  SINGLE_BRIGHT()
  SINGLE_SPACE()
  DRAW_BRIGHT(brightLength+1)
  SINGLE_SPACE()
  SINGLE_BRIGHT()
  FADE_DOWN(transLength+2)
  FILL_SPACING()
}

void DimPattern_SlideLow::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADES(transLength+2)
  
  FADE_UP(transLength+2)
  SINGLE_BRIGHT()
  SINGLE_SPACE()
  DRAW_DIM(brightLength+1)
  SINGLE_SPACE()
  SINGLE_BRIGHT()
  FADE_DOWN(transLength+2)
  FILL_SPACING()
}

// Wants transLength even
void DimPattern_Bowties::Draw(uint8_t* outputArray) {
  uint8_t transOuter = (transLength+3) / 2;
  uint8_t transInner = transLength + 3 - transOuter;
  
  SETUP_DIM_PATTERN()
  SETUP_FADES(transOuter)
  SETUP_FADE2(transInner)

  SINGLE_BRIGHT()
  FADE_DOWN(transOuter)
  FADE_UP2(transInner)
  DRAW_BRIGHT(brightLength+1)
  FADE_DOWN2(transInner)
  FADE_UP(transOuter)
  SINGLE_BRIGHT()
  FILL_SPACING()
}

// Wants transLength even
void DimPattern_ReverseBowties::Draw(uint8_t* outputArray)  {
  uint8_t transOuter = (transLength+2) / 2;
  uint8_t transInner = transLength + 2 - transOuter;

  SETUP_DIM_PATTERN()
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
  FILL_SPACING()
}

void DimPattern_Towers::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  
  DRAW_DIM(transLength+2)
  SINGLE_SPACE()
  DRAW_BRIGHT(brightLength+3)
  SINGLE_SPACE()
  DRAW_DIM(transLength+2)
  FILL_SPACING()
}

void DimPattern_Snake::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  
  DRAW_BRIGHT(brightLength + 2*transLength + 9)
  FILL_SPACING()
}

// Wants dimPeriod even
void DimPattern_Snake3::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  
  DRAW_BRIGHT(transLength+2)
  SINGLE_SPACE()
  
  DRAW_BRIGHT(brightLength+3)
  SINGLE_SPACE()
  
  DRAW_BRIGHT(transLength+2)
  FILL_SPACING()
}

void DimPattern_3Comets::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
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
  FILL_SPACING()
  
  // uint8_t adjBright = transLength + brightLength;
  // SETUP_DIM_PATTERN()
  // SETUP_FADES(adjBright-1)
  
  // FADE_UP(adjBright-1)
  // SINGLE_BRIGHT()
  // SINGLE_SPACE()
  
  // FADE_UP(adjBright-1)
  // SINGLE_BRIGHT()
  // SINGLE_SPACE()
  
  // FADE_UP(adjBright-1)
  // SINGLE_BRIGHT()
  // FILL_SPACING()
}

void DimPattern_3ReverseComets::Draw(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
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
  FILL_SPACING()
  
  // uint8_t adjBright = transLength + brightLength;
  // SETUP_DIM_PATTERN()
  // SETUP_FADE_DOWN(adjBright-1)
  
  // SINGLE_BRIGHT()
  // FADE_DOWN(adjBright-1)
  // SINGLE_SPACE()
  
  // SINGLE_BRIGHT()
  // FADE_DOWN(adjBright-1)
  // SINGLE_SPACE()
  
  // SINGLE_BRIGHT()
  // FADE_DOWN(adjBright-1)
  // FILL_SPACING()
}

// Retired
/*
void DimPattern_StepBarbell::Draw(uint8_t* outputArray) {
  uint8_t spacing = dimPeriod - 2*transLength - brightLength - 2;
  uint8_t adjSpacing = spacing;
  uint8_t adjBrightLength = brightLength / 2;
  uint8_t adjTransLength = transLength;
  if(brightLength % 2 == 0) { adjTransLength++; }
  else { adjBrightLength++; adjSpacing++; }

  uint8_t i = 0;
  uint8_t limit;
  for(limit = adjSpacing; i < limit; i++) {
    outputArray[i] = 0;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    outputArray[i] = 255;
  }
  
  for(limit += 2*adjTransLength; i < limit; i++) {
    outputArray[i] = 32;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    outputArray[i] = 255;
  }
}
*/
