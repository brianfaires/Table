#include "PatternScrolling\PatternGenerator.h"

// Macros for shorthand pattern definition
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


PatternGenerator::PatternGenerator() {
  numColors = 0;
  brightLength = 0;
  transLength = 0;
  dimPeriod = 0;
  colorPeriod = 0;
}

void PatternGenerator::Init(PaletteManager* _pm, GammaManager* gm) {
	pm = _pm;
	Gamma = gm;
}

void PatternGenerator::WriteDimPattern(uint8_t patternIndex, uint8_t* outputArray) {
  uint8_t brightPeriod = 2*transLength + brightLength + 9; // All patterns have a length (excluding spacing) of this
  if(brightPeriod > dimPeriod) { THROW_DUMP("Out of bounds dimPeriod", brightPeriod) }

  switch(patternIndex) {
    case COMET_F:   		WriteDimPattern_Comet(outputArray); break;
    case COMET_R:   		WriteDimPattern_ReverseComet(outputArray); break;
    case TWO_SIDED:   		WriteDimPattern_TwoSided(outputArray); break;
    case BARBELL:   		WriteDimPattern_Barbell(outputArray); break;
    case SLOPED_TOWERS_H:   WriteDimPattern_SlopedHighTowers(outputArray); break;
    case SLOPED_TOWERS_L:   WriteDimPattern_SlopedLowTowers(outputArray); break;
    case SLIDE_H:   		WriteDimPattern_SlideHigh(outputArray); break;
    case SLIDE_L:   		WriteDimPattern_SlideLow(outputArray); break;
    case BOWTIES_F:   		WriteDimPattern_Bowties(outputArray); break;
    case BOWTIES_R:   		WriteDimPattern_ReverseBowties(outputArray); break;
    case TOWERS:  			WriteDimPattern_Towers(outputArray); break;
    case SNAKE:  			WriteDimPattern_Snake(outputArray); break;
    case SNAKE3:  			WriteDimPattern_Snake3(outputArray); break;
    case THREE_COMETS_F:	WriteDimPattern_ThreeComets(outputArray); break;
    case THREE_COMETS_R:	WriteDimPattern_ThreeReverseComets(outputArray); break;
    default:  				WriteDimPattern_NoDim(outputArray); DUMP(patternIndex); break;
  }

  //for(uint8_t i =0; i < dimPeriod; i++) { Serial.println(String(i) + ": " + outputArray[i]);}
}

void PatternGenerator::WriteColorPattern(uint8_t patternIndex, CRGB* outputArray) {
  switch(patternIndex) {
    case 0:  WriteColorPattern_Gradient(outputArray); break;
    case 1: WriteColorPattern_Blocks(outputArray); break;
	default: DUMP(patternIndex); break;
  }
  
  //for(uint8_t i =0; i < colorPeriod; i++) { Serial.println(String(i) + ": (" + outputArray[i].r + ", " + outputArray[i].g + ", " + outputArray[i].b + ")"); }
}

// Color patterns
void PatternGenerator::WriteColorPattern_Gradient(CRGB* outputArray) {
  uint8_t segLength = colorPeriod / numColors;
  uint8_t extraPixels = 0;

  for(uint8_t i = 0; i < numColors; i++) {
	uint8_t adjLength = segLength;
	if(segLength*numColors + extraPixels < colorPeriod) {
		adjLength++;
	}
	
	CRGB first = pm->palette[i];
	CRGB second = i == numColors-1 ? pm->palette[0] : pm->palette[i+1];
	Gamma->Inverse(first);
	Gamma->Inverse(second);
	
    for(uint8_t j = 0; j < adjLength; j++) {
      uint8_t blendAmount = j * 255 / adjLength;
	  uint16_t idx = segLength*i + j + extraPixels;
      outputArray[idx] = blend(first, second, blendAmount);
	  Gamma->Correct(outputArray[idx]);
    }
	
	if(adjLength > segLength) { extraPixels++; }
  }
}

void PatternGenerator::WriteColorPattern_Blocks(CRGB* outputArray) {
  uint16_t colorLengths[numColors];
  uint16_t minLength = colorPeriod / numColors;

  for(uint8_t i = 0; i < numColors; i++) { colorLengths[i] = minLength; }
  
  // Assign extra pixels, inside first
  uint8_t extra = colorPeriod - minLength * numColors;
  if(extra % 2 == 1 && numColors % 2 == 1) {
    colorLengths[numColors/2]++;
    extra--;
  }    
  if(extra > 0) {
    uint8_t assigned = 0;
    for(uint8_t i = 0; i < numColors/2; i++) {
      colorLengths[numColors/2 - 1 - i]++;
      assigned++;
      if(assigned == extra) { break; }
      
      colorLengths[(numColors+1)/2 + i]++;
      assigned++;
      if(assigned == extra) { break; }
    }
  }

  // Write pattern
  uint16_t pixel = 0;
  for(uint8_t col = 0; col < numColors; col++) {
    for(uint16_t i = 0; i < colorLengths[col]; i++) {
      outputArray[pixel++] = pm->palette[col];
    }
  }
}


// Dim patterns
void PatternGenerator::WriteDimPattern_Comet(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADES(2*transLength+9)
  
  FADE_UP(2*transLength+9)
  DRAW_BRIGHT(brightLength)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_ReverseComet(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADE_DOWN(2*transLength+9)
  
  DRAW_BRIGHT(brightLength)
  FADE_DOWN(2*transLength+9)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_TwoSided(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  SETUP_FADES(transLength+4);
  
  FADE_UP(transLength+4)
  DRAW_BRIGHT(brightLength+1)
  FADE_DOWN(transLength+4)
  FILL_SPACING()
}

// This function reverses trans and bright to keep the 2*trans + bright period
void PatternGenerator::WriteDimPattern_Barbell(uint8_t* outputArray) {
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

void PatternGenerator::WriteDimPattern_SlopedHighTowers(uint8_t* outputArray) {
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

void PatternGenerator::WriteDimPattern_SlopedLowTowers(uint8_t* outputArray) {
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

void PatternGenerator::WriteDimPattern_SlideHigh(uint8_t* outputArray) {
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

void PatternGenerator::WriteDimPattern_SlideLow(uint8_t* outputArray) {
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

// This function wants transLength to be even
void PatternGenerator::WriteDimPattern_Bowties(uint8_t* outputArray) {
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

// This function wants transLength to be even
void PatternGenerator::WriteDimPattern_ReverseBowties(uint8_t* outputArray) {
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

void PatternGenerator::WriteDimPattern_Towers(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  
  DRAW_DIM(transLength+2)
  SINGLE_SPACE()
  DRAW_BRIGHT(brightLength+3)
  SINGLE_SPACE()
  DRAW_DIM(transLength+2)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_Snake(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  
  DRAW_BRIGHT(brightLength + 2*transLength + 9)
  FILL_SPACING()
}

// This function wants the dimPeriod to be even
void PatternGenerator::WriteDimPattern_Snake3(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  
  DRAW_BRIGHT(transLength+2)
  SINGLE_SPACE()
  
  DRAW_BRIGHT(brightLength+3)
  SINGLE_SPACE()
  
  DRAW_BRIGHT(transLength+2)
  FILL_SPACING()
}

void PatternGenerator::WriteDimPattern_ThreeComets(uint8_t* outputArray) {
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
  
  /*uint8_t adjBright = transLength + brightLength;
  SETUP_DIM_PATTERN()
  SETUP_FADES(adjBright-1)
  
  FADE_UP(adjBright-1)
  SINGLE_BRIGHT()
  SINGLE_SPACE()
  
  FADE_UP(adjBright-1)
  SINGLE_BRIGHT()
  SINGLE_SPACE()
  
  FADE_UP(adjBright-1)
  SINGLE_BRIGHT()
  FILL_SPACING()*/
}

void PatternGenerator::WriteDimPattern_ThreeReverseComets(uint8_t* outputArray) {
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
  
  /*uint8_t adjBright = transLength + brightLength;
  SETUP_DIM_PATTERN()
  SETUP_FADE_DOWN(adjBright-1)
  
  SINGLE_BRIGHT()
  FADE_DOWN(adjBright-1)
  SINGLE_SPACE()
  
  SINGLE_BRIGHT()
  FADE_DOWN(adjBright-1)
  SINGLE_SPACE()
  
  SINGLE_BRIGHT()
  FADE_DOWN(adjBright-1)
  FILL_SPACING()*/
}

void PatternGenerator::WriteDimPattern_NoDim(uint8_t* outputArray) {
  SETUP_DIM_PATTERN()
  DRAW_BRIGHT(dimPeriod)
}

// Retired
/*


void PatternGenerator::WriteDimPattern_StepBarbell(uint8_t* outputArray) {
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
