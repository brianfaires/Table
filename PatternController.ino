/*
  // Blend brightness pattern
  uint8_t blendAmount;
  uint32_t blendTime = currentTime - lastBrightnessPatternChange;
  if(blendTime >= brightnessPatternBlendTime) {
    blendAmount = 255;
  }
  else {
    blendAmount = 255 * blendTime / brightnessPatternBlendTime;
  }
  
  for(uint8_t i = 0; i < brightnessPatternLength; i++) {
    brightnessPattern[i] = (blendAmount * nextBrightnessPattern[i] + (255 - blendAmount) * lastBrightnessPattern[i]) / 255;
  }



  */
  
PatternController::PatternController() {
  brightnessPeriod = 21;
  colorPeriod = 420;
  colorPatternIndex = 0;
  brightnessPatternIndex = 0;
}

void PatternController::Init(struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, uint32_t curTime) {
  pr.Init(pm, gm, curTime);

  ScaleParams(params);
  pr.colorSpeed = colorSpeed;
  pr.dimSpeed = dimSpeed;
    
  pg.numColors = numColors;
  pg.colorThickness = colorThickness;
  pg.WriteColorPattern(colorPatternIndex, targetColorPattern);
  pr.SetColorPattern(targetColorPattern, pg.GetColorPeriod(colorPatternIndex));
  pg.brightLength = brightLength;
  pg.transLength = transLength;
  pg.spacing = spacing;
  
  pg.WriteBrightnessPattern(brightnessPatternIndex, targetBrightnessPattern);
  pr.SetDimPattern(targetBrightnessPattern, pg.GetBrightnessPeriod());
}

void PatternController::SkipTime(uint32_t amount) {
  pr.SkipTime(amount);
}

void PatternController::SetDisplayMode(uint8_t colorPattern, uint8_t brightnessPattern) {
    uint8_t lastColorIndex = colorPatternIndex;
    colorPatternIndex = colorPattern;
    if(lastColorIndex != colorPatternIndex) {
      pg.WriteColorPattern(colorPatternIndex, targetColorPattern);
      pr.SetColorPattern(targetColorPattern, pg.GetColorPeriod(colorPatternIndex));
    }
    
    uint8_t lastBrightnessIndex = brightnessPatternIndex;
    brightnessPatternIndex = brightnessPattern;
    if(lastBrightnessIndex != brightnessPatternIndex) {
      pg.WriteBrightnessPattern(brightnessPatternIndex, targetBrightnessPattern);
      pr.SetDimPattern(targetBrightnessPattern, pg.GetBrightnessPeriod());
    }
}

void PatternController::ScaleParams(struct_base_show_params& params) {
  #ifdef EXPLICIT_PARAMETERS
    SetDisplayMode(params.displayMode / NUM_BRIGHTNESS_PATTERNS, params.displayMode % NUM_BRIGHTNESS_PATTERNS);
    dimSpeed = params.dimSpeed;
    colorSpeed = params.colorSpeed;
    numColors = params.numColors;
    colorThickness = params.colorThickness;
    transLength = params.transLength;
    brightLength = params.brightLength;
    spacing = params.spacing;
  #else  
    uint8_t displayMode = scaleParam(params.displayMode, 0, NUM_BRIGHTNESS_PATTERNS * NUM_COLOR_PATTERNS - 1);
    SetDisplayMode(displayMode / NUM_BRIGHTNESS_PATTERNS, displayMode % NUM_BRIGHTNESS_PATTERNS);

    uint8_t abs_dimSpeed = scaleParam((uint8_t)abs(params.dimSpeed), 0, 63);
    dimSpeed = abs_dimSpeed * (params.dimSpeed >= 0 ? 1 : -1);
    
    // Bound colorSpeed based on dimSpeed
    int8_t colorSpeed_lower, colorSpeed_upper;
    if(dimSpeed > 0) {
      colorSpeed_lower = dimSpeed / -4;
      colorSpeed_upper = dimSpeed * 3/2;
    }
    else {
      colorSpeed_upper = dimSpeed * 3/2;
      colorSpeed_upper = dimSpeed / -4;
    }
    
    colorSpeed = dimSpeed/2;//scaleParam(params.colorSpeed, colorSpeed_lower, colorSpeed_upper);
    numColors = scaleParam(params.numColors, 2, PALETTE_SIZE-1);
    #ifdef DEBUG_ERRORS
      if(NUM_LEDS > 255*2) { Serial.println("ERROR: NUM_LEDS/numColors results in colorThickness > 255"); }
    #endif
    colorThickness = scaleParam(params.colorThickness, 8, NUM_LEDS/numColors);
  
    transLength = scaleParam(params.transLength, 4, 8);
    brightLength = scaleParam(params.brightLength, 0, brightnessPeriod - 2*transLength - 2);
    spacing = brightnessPeriod - 2*transLength - brightLength - 2;
  #endif
}

void PatternController::Update(struct_base_show_params& params, CRGB* target, uint16_t numLEDs, uint32_t curTime) {
  ScaleParams(params);
  WalkSpeeds();

  if(WalkColorParams()) {
    pg.WriteColorPattern(colorPatternIndex, targetColorPattern);
    pr.SetColorPattern(targetColorPattern, pg.GetColorPeriod(colorPatternIndex));
  }
  
  if(WalkBrightnessParams()) {
    pg.WriteBrightnessPattern(brightnessPatternIndex, targetBrightnessPattern);
    pr.SetDimPattern(targetBrightnessPattern, pg.GetBrightnessPeriod());
  }

  pr.Update(curTime);
  pr.SetCRGBs(target, numLEDs);
}

void PatternController::WalkSpeeds() {
  #ifdef EXPLICIT_PARAMETERS
    pr.dimSpeed = dimSpeed;
    pr.colorSpeed = colorSpeed;
  #else
    // Gradually update speeds even if not ready for a pattern change; slow down at lower levels
    if(pr.dimSpeed != dimSpeed) {
      uint8_t absSpeed = abs(pr.dimSpeed);
      if(absSpeed < 5) {
        // From 10% chance to 4%
        if(random8(50) < absSpeed+2) {
          if(pr.dimSpeed < dimSpeed) { pr.dimSpeed++; }
          else { pr.dimSpeed--; }
        }
      }
      else {
        // From 33% chance to 10%
        if(random16(530) < absSpeed+48) {
          if(pr.dimSpeed < dimSpeed) { pr.dimSpeed++; }
          else { pr.dimSpeed--; }
        }
      }
    }
  
    if(pr.colorSpeed != colorSpeed) {
      uint8_t absSpeed = abs(pr.colorSpeed);
      if(absSpeed < 5) {
        // From 10% chance to 4%
        if(random8(50) <= absSpeed+1) {
          if(pr.colorSpeed < colorSpeed) { pr.colorSpeed++; }
          else { pr.colorSpeed--; }
        }
      }
      else {
        // From 33% chance to 10%
        if(random16(530) < absSpeed+48) {
          if(pr.colorSpeed < colorSpeed) { pr.colorSpeed++; }
          else { pr.colorSpeed--; }
        }
      }
    }
  #endif
}

bool PatternController::WalkColorParams() {
  bool updateMade = false;

  // Change values immediately
  pg.colorThickness = colorThickness;
  pg.numColors = numColors;
  return true;

  if(pr.IsReadyForColorMove(timing.now)) {
    // Gradually update params in sync with movement
    if(pg.colorThickness < colorThickness) {
      pg.colorThickness++;
      updateMade = true;
    }
    else if(pg.colorThickness > colorThickness) {
      pg.colorThickness--;
      updateMade = true;
    }
  
    if(pg.numColors < numColors) {
      pg.numColors++;
      updateMade = true;
    }
    else if(pg.numColors > numColors) {
      pg.numColors--;
      updateMade = true;
    }
  }

  return updateMade;
}

bool PatternController::WalkBrightnessParams() {
  bool updateMade = false;
  
  if(pr.IsReadyForDimMove(timing.now)) {
    // Gradually update params in sync with movement
    
    if(pg.brightLength < brightLength) {
      pg.brightLength++;
      updateMade = true;
    }
    else if(pg.brightLength > brightLength) {
      pg.brightLength--;
      updateMade = true;
    }
  
    if(pg.spacing < spacing) {
      pg.spacing++;
      updateMade = true;
    }
    else if(pg.spacing > spacing) {
      pg.spacing--;
      updateMade = true;
    }
  
    if(pg.transLength < transLength) {
      pg.transLength++;
      updateMade = true;
    }
    else if(pg.transLength > transLength) {
      pg.transLength--;
      updateMade = true;
    }
  }

  return updateMade;
}

