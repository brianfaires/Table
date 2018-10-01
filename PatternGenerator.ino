inline uint8_t PatternGenerator::GetDimPeriod() {
  return 2*transLength + brightLength + spacing + 2;
}

inline uint16_t PatternGenerator::GetColorPeriod(uint8_t targetColorPatternIndex) {
  switch(targetColorPatternIndex) {
    case 0:  return numColors * colorThickness;
    default: return colorThickness;
  }

  #ifdef DEBUG_ERRORS
    Serial.println("GetColorPeriod() jumped over switch statement.");
  #endif
  return 0;
}

void PatternGenerator::WriteDimPattern(uint8_t targetDimPatternIndex, uint8_t* outputArray) {
  #ifdef DEBUG_ERRORS
    if(targetDimPatternIndex >= NUM_DIM_PATTERNS) { Serial.println("ERROR: targetDimPatternIndex out of bounds: " + String(targetDimPatternIndex)); }
  #endif
  
  switch(targetDimPatternIndex) {
    case 0:  WriteDimPattern_Comet(outputArray); break;
    case 1:  WriteDimPattern_BackwardComet(outputArray); break;
    case 2:  WriteDimPattern_TwoSided(outputArray); break;
    case 3:  WriteDimPattern_Barbell(outputArray); break;
    case 4:  WriteDimPattern_StepBarbell(outputArray); break;
    case 5:  WriteDimPattern_BrokenBarbell(outputArray); break;
    case 6:  WriteDimPattern_InverseStepBarbell(outputArray); break;
    case 7:  WriteDimPattern_BrokenStepBarbell(outputArray); break;
    case 8:  WriteDimPattern_Towers(outputArray); break;
    case 9:  WriteDimPattern_StepsUp(outputArray); break;
    case 10: WriteDimPattern_StepsDown(outputArray); break;
    case 11: WriteDimPattern_SlopedHighTowers(outputArray); break;
    case 12: WriteDimPattern_SlopedLowTowers(outputArray); break;
    case 13: WriteDimPattern_SlideHigh(outputArray); break;
    case 14: WriteDimPattern_SlideLow(outputArray); break;
    default: WriteDimPattern_NoDim(outputArray); break;
  }
}

void PatternGenerator::WriteColorPattern(uint8_t targetColorPatternIndex, PRGB* outputArray) {
  #ifdef DEBUG_ERRORS
    if(targetColorPatternIndex >= NUM_COLOR_PATTERNS) { Serial.println("ERROR: targetColorPatternIndex out of bounds: " + String(targetColorPatternIndex)); }
  #endif
  
  switch(targetColorPatternIndex) {
    case 0:  WriteColorPattern_Gradient(outputArray); break;
    default: WriteColorPattern_Blocks(outputArray); break;
  }
}


void PatternGenerator::WriteColorPattern_Gradient(PRGB* outputArray) {
  #ifdef DEBUG_ERRORS
    if(numColors > PALETTE_SIZE || numColors < 1) {
      Serial.println("ERROR: WriteColorPattern_Gradient, numColors = " + String(numColors));
      numColors = PALETTE_SIZE;
    }
    if(numColors * colorThickness > NUM_LEDS) {
      Serial.println("Error: WriteColorPattern_Gradient, period = " + String(numColors * colorThickness));
      colorThickness = NUM_LEDS / numColors;
    }
  #endif

  PRGB pattern[GetColorPeriod(0)];//debug: magic number index
  
  for(uint8_t i = 0; i < numColors; i++) {
    for(uint8_t j = 0; j < colorThickness; j++) {
      uint8_t b = i == numColors-1 ? 0 : i+1; // Necessary because using "{ }" will convert to int and throw a warning
      uint8_t blendAmount = j * 255 / colorThickness; // Necessary because using "{ }" will convert to int and throw a warning
      pattern[colorThickness*i + j] = (PRGB) { i, b, blendAmount};
      //Serial.println(String(i) + "/" + String(j) + ": " + String(i == numColors-1 ? 0 : i+1) + ", " + String(j * 255 / colorThickness));
    }
  }

  memcpy(outputArray, pattern, sizeof(PRGB)*GetColorPeriod(0));
}

void PatternGenerator::WriteColorPattern_Blocks(PRGB* outputArray) {
  #ifdef DEBUG_ERRORS
    if(numColors > PALETTE_SIZE || numColors < 1) {
      Serial.println("ERROR: Snake, numColors = " + String(numColors));
      numColors = PALETTE_SIZE;
    }
    if(colorThickness > NUM_LEDS) {
      Serial.println("Error: Snake, period = " + String(colorThickness));
      colorThickness = NUM_LEDS / numColors;
    }
  #endif

  uint16_t period = GetColorPeriod(1); //debug:magic number index
  PRGB pattern[period];
  uint16_t colorLengths[numColors];
  uint16_t minLength = period / numColors;

  for(uint8_t i = 0; i < numColors; i++) { colorLengths[i] = minLength; }
  
  // Assign extra pixels, inside first
  uint8_t extra = period - minLength * numColors;
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
      pattern[pixel++] = (PRGB){ col, 0, 0 };
    }
  }
  
  memcpy(outputArray, pattern, sizeof(PRGB)*GetColorPeriod(1));
}


void PatternGenerator::WriteDimPattern_Comet(uint8_t* outputPattern) {
  float fadeStepSize = GET_FADE_STEP_SIZE(2*transLength + 1);
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  uint8_t lastLimitMinusOne = limit - 1;
  
  for(limit += 2*transLength + 2; i < limit; i++) {
    pattern[i] = MIN_BRIGHTNESS + fadeStepSize * (uint8_t)(i - lastLimitMinusOne);
  }

  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

//for(uint8_t i =0; i < GetDimPeriod(); i++) { Serial.println(String(i) + ": " + pattern[i]);}
  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_BackwardComet(uint8_t* outputPattern) {
  float fadeStepSize = GET_FADE_STEP_SIZE(2*transLength + 1);
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  for(limit += 2*transLength + 2; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_TwoSided(uint8_t* outputPattern) {
  float fadeStepSize = GET_FADE_STEP_SIZE(transLength+1);
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }
  
  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += transLength + 1; i < limit; i++) {
    pattern[i] = fadeStepSize * (uint8_t)(i - lastLimitMinusOne);
  }
  
  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  for(limit += transLength + 1; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_Barbell(uint8_t* outputPattern) {
  uint8_t adjSpacing = spacing;
  uint8_t adjBrightLength = brightLength / 2;
  uint8_t adjTransLength = transLength;
  if(brightLength % 2 == 0) { adjTransLength++; }
  else { adjBrightLength++; adjSpacing++; }

  float fadeStepSize = GET_FADE_STEP_SIZE(adjTransLength);
  uint8_t pattern[GetDimPeriod()];
  
  uint8_t i = 0;
  uint8_t limit;
  for(limit = adjSpacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  for(limit += adjTransLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += adjTransLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (uint8_t)(i - lastLimitMinusOne);
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_StepBarbell(uint8_t* outputPattern) {
  uint8_t adjSpacing = spacing;
  uint8_t adjBrightLength = brightLength / 2;
  uint8_t adjTransLength = transLength;
  if(brightLength % 2 == 0) { adjTransLength++; }
  else { adjBrightLength++; adjSpacing++; }

  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = adjSpacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }
  
  for(limit += 2*adjTransLength; i < limit; i++) {
    pattern[i] = 32;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_BrokenBarbell(uint8_t* outputPattern) {
  uint8_t adjSpacing = spacing;
  uint8_t adjBrightLength = brightLength / 2;
  uint8_t adjTransLength = transLength;
  if(brightLength % 2 == 0) { adjSpacing++; }
  else { adjBrightLength++; }

  float fadeStepSize = GET_FADE_STEP_SIZE(adjTransLength);
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = adjSpacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  for(limit += adjTransLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  pattern[i] = 0;
  i++;
  limit++;

  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += adjTransLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (uint8_t)(i - lastLimitMinusOne);
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_BrokenStepBarbell(uint8_t* outputPattern) {
  uint8_t adjSpacing = spacing;
  uint8_t adjBrightLength = brightLength / 2;
  uint8_t adjTransLength = transLength;
  if(brightLength % 2 == 0) { adjSpacing++; }
  else { adjBrightLength++; }
  
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = adjSpacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  } 

  for(limit += adjTransLength; i < limit; i++) {
    pattern[i] = 32;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += adjTransLength; i < limit; i++) {
    pattern[i] = 32;
  }
  
  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_InverseStepBarbell(uint8_t* outputPattern) {
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += 1 + transLength; i < limit; i++) {
    pattern[i] = 32;
  }
  
  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  for(limit += 1 + transLength; i < limit; i++) {
    pattern[i] = 32;
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_Towers(uint8_t* outputPattern) {
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }
  
  for(limit += transLength; i < limit; i++) {
    pattern[i] = 32;
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += transLength; i < limit; i++) {
    pattern[i] = 32;
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_StepsUp(uint8_t* outputPattern) {
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += transLength; i < limit; i++) {
    pattern[i] = 70;
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += transLength; i < limit; i++) {
    pattern[i] = 150;
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_StepsDown(uint8_t* outputPattern) {
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += transLength; i < limit; i++) {
    pattern[i] = 150;
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += transLength; i < limit; i++) {
    pattern[i] = 70;
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_SlopedHighTowers(uint8_t* outputPattern) {
  float fadeStepSize = GET_FADE_STEP_SIZE(transLength);
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit; 
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  uint8_t lastLimitMinusOne = limit - 1;

  for(limit += transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (uint8_t)(i - lastLimitMinusOne);
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_SlopedLowTowers(uint8_t* outputPattern) {
  float fadeStepSize = GET_FADE_STEP_SIZE(transLength);
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (uint8_t)(i - lastLimitMinusOne);
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 32;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_SlideHigh(uint8_t* outputPattern) {
  float fadeStepSize = GET_FADE_STEP_SIZE(transLength);
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 255;
  }
  
  pattern[i] = 0;
  i++;
  limit++;

  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (uint8_t)(i - lastLimitMinusOne);
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_SlideLow(uint8_t* outputPattern) {
  float fadeStepSize = GET_FADE_STEP_SIZE(transLength);
  uint8_t pattern[GetDimPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += brightLength; i < limit; i++) {
    pattern[i] = 32;
  }
  
  pattern[i] = 0;
  i++;
  limit++;

  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (uint8_t)(i - lastLimitMinusOne);
  }

  memcpy(outputPattern, pattern, GetDimPeriod());
}

void PatternGenerator::WriteDimPattern_NoDim(uint8_t* outputPattern) {
  uint8_t period = GetDimPeriod();
  uint8_t pattern[period];
  for(uint8_t i = 0; i < period; i++) {
    pattern[i] = 255;
  }
  
  memcpy(outputPattern, pattern, period);

}
