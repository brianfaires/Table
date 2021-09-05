#include "obj/PatternController/ColorPattern.h"

void ColorPattern::Init(PaletteManager* _pm, GammaManager* _gm) {
    pm = _pm;
    Gamma = _gm;
}

void ColorPattern::Draw(ColorPatternName pattern, CRGB* outputArray) {
  switch(pattern)
  {
    case ColorPatternName::Gradient:
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
    
    case ColorPatternName::Blocks:
    {
      uint16_t colorLengths[numColors];
      uint16_t minLength = colorPeriod / numColors;

      for(uint8_t i = 0; i < numColors; i++) { colorLengths[i] = minLength; }
      
      // Assign extra pixels, inside first
      uint16_t extra = colorPeriod - minLength * numColors;
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

    case ColorPatternName::ManualBlocks:
    {
      if(numColorIndexes == 0) {
        THROW(F("ManualBlocks mode used but not initialized. Setting to monochromatic."))
        uint8_t colorIndexes[] = { 0 };
        setManualBlocks(colorIndexes, 1, dimPeriod);
      }
      
      uint16_t numDimPeriods = colorPeriod / dimPeriod;
      if(numDimPeriods * dimPeriod != colorPeriod) { THROW_DUMP(F("colorPeriod must be a multiple of dimPeriod for ManualBlocks"), colorPeriod) }
      // Write pattern
      uint16_t pixel = 0;
      for(uint16_t i = 0; i < numDimPeriods; i++) {
        uint8_t colIndex = colorIndexes[i];
        for(uint16_t j = 0; j < dimPeriod; j++) {
          outputArray[pixel++] = pm->palette[colIndex];
        }
      }

      break;
    }

    default:
      THROW_DUMP("Unrecognized colorPattern", int(pattern))
  }
}

void ColorPattern::setManualBlocks(uint8_t* _colorIndexes, uint8_t _numColorIndexes, uint16_t _dimPeriod) {
  dimPeriod = _dimPeriod;
  numColorIndexes = _numColorIndexes;
  colorPeriod = numColorIndexes * dimPeriod;

  for(int i = 0; i < numColorIndexes; i++)
  {
    colorIndexes[i] = _colorIndexes[i];
  }
}

uint8_t* ColorPattern::getManualBlocks() {
  //uint8_t retVal[numColorIndexes];
  //memcpy(retVal, colorIndexes, numColorIndexes);
  //return retVal;

  return colorIndexes;
}