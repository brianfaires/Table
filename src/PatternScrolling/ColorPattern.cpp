#include "PatternScrolling\ColorPattern.h"

void ColorPattern::Init(PaletteManager* _pm, GammaManager* _gm) {
    pm = _pm;
    Gamma = _gm;
}

void ColorPattern::Draw(ColorPatternName pattern, CRGB* outputArray) {
  switch(pattern)
  {
    case ColorPatternName::GRADIENT:
    {
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
      break;
    }
    
    case ColorPatternName::BLOCKS:
    {
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
      break;
    }
    default:
      THROW_DUMP("Unrecognized colorPattern", int(pattern))
  }
}
