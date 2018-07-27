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
  
}

void PatternController::Init(struct_base_show_params* params, uint32_t curTime) {
  pr.Init(curTime);

  ScaleParams(params);
  pr.colorSpeed = colorSpeed;
  pr.brightnessSpeed = brightnessSpeed;
    
  pg.numColors = numColors;
  pg.colorThickness = colorThickness;
  pg.WriteColorPattern(colorPatternIndex, targetColorPattern);
  pr.SetColorPattern(targetColorPattern, colorPeriod);

  pg.brightLength = brightLength;
  pg.transLength = transLength;
  pg.spacing = spacing;
  pg.WriteBrightnessPattern(brightnessPatternIndex, targetBrightnessPattern);
  pr.SetBrightnessPattern(targetBrightnessPattern, brightnessPeriod);
}

void PatternController::SkipTime(uint32_t amount) {
  pr.SkipTime(amount);
}

void PatternController::ScaleParams(struct_base_show_params* params) {
  uint8_t displayMode = scaleParam(params->displayMode, 0, NUM_BRIGHTNESS_PATTERNS * NUM_COLOR_PATTERNS - 1);
  brightnessPatternIndex = displayMode % NUM_BRIGHTNESS_PATTERNS;
  colorPatternIndex = displayMode / NUM_BRIGHTNESS_PATTERNS;

  uint8_t abs_brightnessSpeed = scaleParam((uint8_t)abs(params->brightnessSpeed), 0, 63);
  brightnessSpeed = abs_brightnessSpeed * (params->brightnessSpeed >= 0 ? 1 : -1);
  
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
  
  colorSpeed = brightnessSpeed/2;//scaleParam(params->colorSpeed, colorSpeed_lower, colorSpeed_upper);
  numColors = scaleParam(params->numColors, 2, PALETTE_SIZE-1);
  #ifdef DEBUG_ERRORS
    if(NUM_LEDS > 255*2) { Serial.println("ERROR: NUM_LEDS/numColors results in colorThickness > 255"); }
  #endif
  uint8_t colorThickness = scaleParam(params->colorThickness, 8, NUM_LEDS/(PALETTE_SIZE-1));

  transLength = scaleParam(params->transLength, 4, 8);
  brightLength = scaleParam(params->brightLength, 0, brightnessPeriod - 2*transLength - 2);
  spacing = brightnessPeriod - 2*transLength - brightLength - 2;
}

void PatternController::Update(struct_base_show_params* params, CRGB* target, uint16_t numLEDs, PaletteManager& pm, uint32_t curTime) {
  ScaleParams(params);
  WalkSpeeds();
  
  if(WalkColorParams()) {
    pg.WriteColorPattern(colorPatternIndex, targetColorPattern);
    pr.SetColorPattern(targetColorPattern, colorPeriod);
  }
  
  if(WalkBrightnessParams()) {
    pg.WriteBrightnessPattern(brightnessPatternIndex, targetBrightnessPattern);
    pr.SetBrightnessPattern(targetBrightnessPattern, brightnessPeriod);
  }
  
  pr.Update(curTime);
  pr.SetCRGBs(target, numLEDs, pm);
}

void PatternController::WalkSpeeds() {
  // Gradually update speeds even if not ready for a pattern change; slow down at lower levels
  if(pr.brightnessSpeed != brightnessSpeed) {
    uint8_t absSpeed = abs(pr.brightnessSpeed);
    if(absSpeed < 5) {
      // From 10% chance to 4%
      if(random8(50) < absSpeed+2) {
        if(pr.brightnessSpeed < brightnessSpeed) { pr.brightnessSpeed++; }
        else { pr.brightnessSpeed--; }
      }
    }
    else {
      // From 33% chance to 10%
      if(random16(530) < absSpeed+48) {
        if(pr.brightnessSpeed < brightnessSpeed) { pr.brightnessSpeed++; }
        else { pr.brightnessSpeed--; }
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
}

bool PatternController::WalkColorParams() {
  bool updateMade = false;
  
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
  
  if(pr.IsReadyForBrightnessMove(timing.now)) {
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

