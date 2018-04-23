#define GET_FADE_STEP_SIZE(x) 255.0f / (x+1)

void WriteColorPattern_Gradient(PatternHandler& ph) {
  #ifdef DEBUG_ERRORS
    if(ph.numColors > PALETTE_SIZE || ph.numColors < 1) {
      Serial.println("ERROR: WriteColorPattern_Gradient, ph.numColors = " + String(ph.numColors));
      ph.numColors = PALETTE_SIZE;
    }
    if(ph.numColors * ph.colorThickness > NUM_LEDS) {
      Serial.println("Error: WriteColorPattern_Gradient, period = " + String(ph.numColors * ph.colorThickness));
      ph.colorThickness = NUM_LEDS / ph.numColors;
    }
  #endif

  uint8_t period = ph.numColors * ph.colorThickness;
  PRGB pattern[period];
  
  for(uint8_t i = 0; i < ph.numColors; i++) {
    for(uint8_t j = 0; j < ph.colorThickness; j++) {
      pattern[ph.colorThickness*i + j] = (PRGB) { i, i == ph.numColors-1 ? 0 : i+1, j * 255 / ph.colorThickness };
    }
  }

  ph.SetColorPattern(pattern, period);
}

void WriteColorPattern_Blocks(PatternHandler& ph) {
  #ifdef DEBUG_ERRORS
    if(ph.numColors > PALETTE_SIZE || ph.numColors < 1) {
      Serial.println("ERROR: Snake, ph.numColors = " + String(ph.numColors));
      ph.numColors = PALETTE_SIZE;
    }
    if(ph.numColors * ph.colorThickness > NUM_LEDS) {
      Serial.println("Error: Snake, period = " + String(ph.numColors * ph.colorThickness));
      ph.colorThickness = NUM_LEDS / ph.numColors;
    }
  #endif

  uint8_t bandThickness = (ph.colorThickness + 1) / 2;
  uint8_t period = ph.numColors * bandThickness;
  PRGB pattern[period];
  uint8_t pixel = 0;
  for(uint8_t col = 0; col < ph.numColors; col++) {
    for(uint8_t i = 0; i < bandThickness; i++) {
      pattern[pixel++] = (PRGB){ col, 0, 0 };
    }
  }
  
  ph.SetColorPattern(pattern, period);
}


void WriteDimPattern_Comet(PatternHandler& ph) {
  float fadeStepSize = GET_FADE_STEP_SIZE(2*ph.transLength + 1);
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  uint8_t lastLimitMinusOne = limit - 1;
  
  for(limit += 2*ph.transLength + 1; i < limit; i++) {
    pattern[i] = fadeStepSize * (i - lastLimitMinusOne);
  }

  for(limit += ph.brightLength + 1; i < limit; i++) {
    pattern[i] = 255;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_BackwardComet(PatternHandler& ph) {
  float fadeStepSize = GET_FADE_STEP_SIZE(2*ph.transLength + 1);
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += ph.brightLength + 1; i < limit; i++) {
    pattern[i] = 255;
  }

  for(limit += 2*ph.transLength + 1; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_TwoSided(PatternHandler& ph) {
  float fadeStepSize = GET_FADE_STEP_SIZE(ph.transLength+1);
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }
  
  uint8_t lastLimitMinusOne = limit - 1;

  for(limit += ph.transLength + 1; i < limit; i++) {
    pattern[i] = fadeStepSize * (i - lastLimitMinusOne);
  }
  
  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  for(limit += ph.transLength + 1; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_Barbell(PatternHandler& ph) {
  uint8_t adjSpacing = ph.spacing;
  uint8_t adjBrightLength = ph.brightLength / 2;
  uint8_t adjTransLength = ph.transLength;
  if(ph.brightLength % 2 == 0) { adjTransLength++; }
  else { adjBrightLength++; adjSpacing++; }

  float fadeStepSize = GET_FADE_STEP_SIZE(adjTransLength);
  uint8_t pattern[ph.GetPeriod()];
  
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
    pattern[i] = fadeStepSize * (i - lastLimitMinusOne);
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_StepBarbell(PatternHandler& ph) {
  uint8_t adjSpacing = ph.spacing;
  uint8_t adjBrightLength = ph.brightLength / 2;
  uint8_t adjTransLength = ph.transLength;
  if(ph.brightLength % 2 == 0) { adjTransLength++; }
  else { adjBrightLength++; adjSpacing++; }

  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = adjSpacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }
  
  for(limit += 2*adjTransLength; i < limit; i++) {
    pattern[i] = 64;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_BrokenBarbell(PatternHandler& ph) {
  uint8_t adjSpacing = ph.spacing;
  uint8_t adjBrightLength = ph.brightLength / 2;
  uint8_t adjTransLength = ph.transLength;
  if(ph.brightLength % 2 == 0) { adjSpacing++; }
  else { adjBrightLength++; }

  float fadeStepSize = GET_FADE_STEP_SIZE(adjTransLength);
  uint8_t pattern[ph.GetPeriod()];

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
    pattern[i] = fadeStepSize * (i - lastLimitMinusOne);
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_BrokenStepBarbell(PatternHandler& ph) {
  uint8_t adjSpacing = ph.spacing;
  uint8_t adjBrightLength = ph.brightLength / 2;
  uint8_t adjTransLength = ph.transLength;
  if(ph.brightLength % 2 == 0) { adjSpacing++; }
  else { adjBrightLength++; }
  
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = adjSpacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  } 

  for(limit += adjTransLength; i < limit; i++) {
    pattern[i] = 64;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += adjTransLength; i < limit; i++) {
    pattern[i] = 64;
  }
  
  for(limit += adjBrightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_InverseStepBarbell(PatternHandler& ph) {
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += 1 + ph.transLength; i < limit; i++) {
    pattern[i] = 64;
  }
  
  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  for(limit += 1 + ph.transLength; i < limit; i++) {
    pattern[i] = 64;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_Towers(PatternHandler& ph) {
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }
  
  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = 64;
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = 64;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_StepsUp(PatternHandler& ph) {
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = 80;
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = 160;
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_StepsDown(PatternHandler& ph) {
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = 160;
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = 80;
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_SlopedHighTowers(PatternHandler& ph) {
  float fadeStepSize = GET_FADE_STEP_SIZE(ph.transLength);
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit; 
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  uint8_t lastLimitMinusOne = limit - 1;

  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (i - lastLimitMinusOne);
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 255;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_SlopedLowTowers(PatternHandler& ph) {
  float fadeStepSize = GET_FADE_STEP_SIZE(ph.transLength);
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (i - lastLimitMinusOne);
  }

  pattern[i] = 0;
  i++;
  limit++;
  
  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 64;
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_SlideHigh(PatternHandler& ph) {
  float fadeStepSize = GET_FADE_STEP_SIZE(ph.transLength);
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }

  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 255;
  }
  
  pattern[i] = 0;
  i++;
  limit++;

  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (i - lastLimitMinusOne);
  }

  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_SlideLow(PatternHandler& ph) {
  float fadeStepSize = GET_FADE_STEP_SIZE(ph.transLength);
  uint8_t pattern[ph.GetPeriod()];

  uint8_t i = 0;
  uint8_t limit;
  for(limit = ph.spacing; i < limit; i++) {
    pattern[i] = 0;
  }


  uint8_t lastLimitMinusOne = limit - 1;
  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (i - lastLimitMinusOne);
  }

  pattern[i] = 0;
  i++;
  limit++;

  for(limit += ph.brightLength; i < limit; i++) {
    pattern[i] = 255;
  }
  
  pattern[i] = 0;
  i++;
  limit++;

  for(limit += ph.transLength; i < limit; i++) {
    pattern[i] = fadeStepSize * (limit - i);
  }
  
  ph.SetBrightnessPattern(pattern, timing.now);
}

void WriteDimPattern_None(PatternHandler& ph) {
  const uint8_t period = ph.GetPeriod();
  uint8_t pattern[period];
  for(uint8_t i = 0; i < period; i++) {
    pattern[i] = 80;
  }
  
  ph.SetBrightnessPattern(pattern, timing.now);

}


#define BLS_NUM_DIMMING_MODES 16
#define BLS_NUM_COLOR_MODES 2
void BaseLayerScroller(bool initDisplay, PatternHandler& ph) {
  static bool alreadyUpdatedPatternParams = false;
  static bool alreadyUpdatedBrightnessParams = false;
  static uint8_t dimmingMode = 0;
  static uint8_t colorMode = 0;

  uint8_t displayMode = scaleParam(baseParams.displayMode, 0, BLS_NUM_DIMMING_MODES * BLS_NUM_COLOR_MODES - 1);
  dimmingMode = displayMode % BLS_NUM_DIMMING_MODES;
  colorMode = displayMode / BLS_NUM_DIMMING_MODES;

  uint8_t abs_brightnessSpeed = scaleParam((uint8_t)abs(baseParams.brightnessSpeed), 8, 63);
  int8_t brightnessSpeed = abs_brightnessSpeed * (baseParams.brightnessSpeed >= 0 ? 1 : -1);
  
  // Bound colorSpeed based on brightnessSpeed
  int8_t colorSpeed_lower, colorSpeed_upper;
  if(brightnessSpeed > 0) {
    colorSpeed_lower = brightnessSpeed / -4;
    colorSpeed_upper = brightnessSpeed * 3/2;
  }
  else {
    colorSpeed_upper = brightnessSpeed * 3/2;
    colorSpeed_upper = brightnessSpeed / -4;
  }
  
  int8_t colorSpeed = brightnessSpeed/2;//scaleParam(baseParams.colorSpeed, colorSpeed_lower, colorSpeed_upper);
  uint8_t numColors = scaleParam(baseParams.numColors, 1, 5);
  uint8_t colorThickness = scaleParam(baseParams.colorThickness, 3, 10);

  const uint8_t fixedPeriod = 19;
  uint8_t transLength = scaleParam(baseParams.transLength, 4, 8);
  uint8_t brightLength = scaleParam(baseParams.brightLength, 0, fixedPeriod - 2*transLength - 1);
  uint8_t spacing = fixedPeriod - transLength - brightLength;
  
  if(initDisplay) {
    ph.numColors = numColors;
    ph.colorThickness = colorThickness;
    ph.brightLength = brightLength;
    ph.spacing = spacing;
    ph.transLength = transLength;
    ph.colorSpeed = colorSpeed;
    ph.brightnessSpeed = brightnessSpeed;
  }

  
  BaseLayerScroller_SlowWalkSpeeds(brightnessSpeed, colorSpeed, ph);

  if(ph.IsReadyForColorPatternChange() || initDisplay) {
    if(!alreadyUpdatedPatternParams) {
      // Gradually update params
      if(ph.colorThickness < colorThickness) {
        ph.colorThickness++;
        alreadyUpdatedPatternParams = true;
        ph.ColorPatternParametersChanged();
      }
      else if(ph.colorThickness > colorThickness) {
        ph.colorThickness--;
        alreadyUpdatedPatternParams = true;
        ph.ColorPatternParametersChanged();
      }
  
      if(ph.numColors < numColors) {
        ph.numColors++;
        alreadyUpdatedPatternParams = true;
        ph.ColorPatternParametersChanged();
      }
      else if(ph.numColors > numColors) {
        ph.numColors--;
        alreadyUpdatedPatternParams = true;
        ph.ColorPatternParametersChanged();
      }

      // Write color pattern
      if(colorMode == 0) {
        WriteColorPattern_Gradient(ph);
        alreadyUpdatedPatternParams = true;
        ph.ColorPatternParametersChanged();
      }
      else if(colorMode == 1) {
        WriteColorPattern_Blocks(ph);
        alreadyUpdatedPatternParams = true;
        ph.ColorPatternParametersChanged();
      }
    }
  }
  else {
    alreadyUpdatedPatternParams = false;
  }
  
  if(ph.IsReadyForBrightnessChange() || initDisplay) {
    if(!alreadyUpdatedBrightnessParams) {
      // Gradually update params
      if(ph.brightLength < brightLength) {
        ph.brightLength++;
        alreadyUpdatedBrightnessParams = true;
        ph.BrightnessParametersChanged();
      }
      else if(ph.brightLength > brightLength) {
        ph.brightLength--;
        alreadyUpdatedBrightnessParams = true;
        ph.BrightnessParametersChanged();
      }
  
      if(ph.spacing < spacing) {
        ph.spacing++;
        alreadyUpdatedBrightnessParams = true;
        ph.BrightnessParametersChanged();
      }
      else if(ph.spacing > spacing) {
        ph.spacing--;
        alreadyUpdatedBrightnessParams = true;
        ph.BrightnessParametersChanged();
      }
  
      if(ph.transLength < transLength) {
        ph.transLength++;
        alreadyUpdatedBrightnessParams = true;
        ph.BrightnessParametersChanged();
      }
      else if(ph.transLength > transLength) {
        ph.transLength--;
        alreadyUpdatedBrightnessParams = true;
        ph.BrightnessParametersChanged();
      }

      BaseLayerScroller_WriteDimmingPattern(dimmingMode, ph);
    }
  }
  else {
    alreadyUpdatedBrightnessParams = false;
  }

  if(initDisplay) { ph.Init(timing.now); }
  ph.Update(timing.now);
  ph.SetCRGBs(leds, NUM_LEDS, pm);
}

void BaseLayerScroller_WriteDimmingPattern(uint8_t dimmingMode, PatternHandler& ph) {
  switch(dimmingMode) {
    case 0:  WriteDimPattern_Comet(ph); break;
    case 1:  WriteDimPattern_BackwardComet(ph); break;
    case 2:  WriteDimPattern_TwoSided(ph); break;
    case 3:  WriteDimPattern_Barbell(ph); break;
    case 4:  WriteDimPattern_StepBarbell(ph); break;
    case 5:  WriteDimPattern_BrokenBarbell(ph); break;
    case 6:  WriteDimPattern_InverseStepBarbell(ph); break;
    case 7:  WriteDimPattern_BrokenStepBarbell(ph); break;
    case 8:  WriteDimPattern_Towers(ph); break;
    case 9:  WriteDimPattern_StepsUp(ph); break;
    case 10: WriteDimPattern_StepsDown(ph); break;
    case 11: WriteDimPattern_SlopedHighTowers(ph); break;
    case 12: WriteDimPattern_SlopedLowTowers(ph); break;
    case 13: WriteDimPattern_SlideHigh(ph); break;
    case 14: WriteDimPattern_SlideLow(ph); break;
    default: WriteDimPattern_None(ph); break;
  }
}

void BaseLayerScroller_SlowWalkSpeeds(int8_t brightnessSpeed, int8_t colorSpeed, PatternHandler& ph) {
  // Gradually update speeds even if not ready for a pattern change; slow down at lower levels
  if(ph.brightnessSpeed != brightnessSpeed) {
    uint8_t absSpeed = abs(ph.brightnessSpeed);
    if(absSpeed < 5) {
      // From 10% chance to 4%
      if(random8(50) < absSpeed+2) {
        if(ph.brightnessSpeed < brightnessSpeed) { ph.brightnessSpeed++; }
        else { ph.brightnessSpeed--; }
      }
    }
    else {
      // From 33% chance to 10%
      if(random16(530) < absSpeed+48) {
        if(ph.brightnessSpeed < brightnessSpeed) { ph.brightnessSpeed++; }
        else { ph.brightnessSpeed--; }
      }
    }
  }

  if(ph.colorSpeed != colorSpeed) {
    uint8_t absSpeed = abs(ph.colorSpeed);
    if(absSpeed < 5) {
      // From 10% chance to 4%
      if(random8(50) <= absSpeed+1) {
        if(ph.colorSpeed < colorSpeed) { ph.colorSpeed++; }
        else { ph.colorSpeed--; }
      }
    }
    else {
      // From 33% chance to 10%
      if(random16(530) < absSpeed+48) {
        if(ph.colorSpeed < colorSpeed) { ph.colorSpeed++; }
        else { ph.colorSpeed--; }
      }
    }
  }
}

