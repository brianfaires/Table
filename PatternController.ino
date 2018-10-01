PatternController::PatternController() {
  brightnessPeriod = 21;
  colorPeriod = 420;
  targetColorPatternIndex = 0;
  targetDimPatternIndex = 0;
  dimPatternBlendLength = INIT_PATTERN_CONTROLLER_DIM_BLEND_LENGTH;
  colorPatternBlendLength = INIT_PATTERN_CONTROLLER_COLOR_BLEND_LENGTH;
  dimPatternPauseLength = INIT_PATTERN_CONTROLLER_DIM_PAUSE_LENGTH;
  colorPatternPauseLength = INIT_PATTERN_CONTROLLER_COLOR_PAUSE_LENGTH;
}

uint32_t PatternController::GetColorPauseLength() {
  return colorPatternPauseLength;
}
void PatternController::SetColorPauseLength(uint32_t value) {
  colorPatternPauseLength = value;
}

uint32_t PatternController::GetDimPauseLength() {
  return dimPatternPauseLength;
}
void PatternController::SetDimPauseLength(uint32_t value) {
  dimPatternPauseLength = value;
}

uint32_t PatternController::GetColorBlendLength() {
  return colorPatternBlendLength;
}
void PatternController::SetColorBlendLength(uint32_t value) {
  colorPatternBlendLength = value;
}

uint32_t PatternController::GetDimBlendLength() {
  return dimPatternBlendLength;
}
void PatternController::SetDimBlendLength(uint32_t value) {
  dimPatternBlendLength = value;
}

void PatternController::Init(struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, uint32_t curTime) {
  pr.Init(pm, gm, curTime);

  lastDimParamChange = curTime;
  lastColorParamChange = curTime;
  lastDimPatternChange = curTime;
  lastColorPatternChange = curTime;

  ScaleParams(params, curTime);
  pr.colorSpeed = colorSpeed;
  pr.dimSpeed = dimSpeed;
    
  pg.numColors = numColors;
  pg.colorThickness = colorThickness;
  pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);
  pr.SetColorPattern(targetColorPattern, pg.GetColorPeriod(targetColorPatternIndex));
  pg.brightLength = brightLength;
  pg.transLength = transLength;
  pg.spacing = spacing;
  
  pg.WriteDimPattern(targetDimPatternIndex, targetDimPattern);
  pr.SetDimPattern(targetDimPattern, pg.GetDimPeriod());
}

void PatternController::SkipTime(uint32_t amount) {
  pr.SkipTime(amount);
}

void PatternController::SetDisplayMode(uint8_t colorPattern, uint8_t dimPattern, uint32_t curTime) {
    if(targetColorPatternIndex != colorPattern) {
      // New dim pattern target; this was not the cause of a blend completing.  Mark oldIndex as 0xFF to signal this.
      memcpy(oldColorPattern, curColorPattern, pg.GetColorPeriod(oldColorPatternIndex));
      oldColorPatternIndex = 0xFF;// targetColorPatternIndex;
      
      targetColorPatternIndex = colorPattern;
      pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);

      lastColorPatternChange = curTime;
    }

    if(targetDimPatternIndex != dimPattern) {
      // New dim pattern target; this was not the cause of a blend completing.  Mark oldIndex as 0xFF to signal this.
      uint8_t lastDimPattern = oldDimPatternIndex;
      memcpy(oldDimPattern, curDimPattern, pg.GetDimPeriod());
      oldDimPatternIndex = 0xFF;// targetDimPatternIndex;

      targetDimPatternIndex = dimPattern;

      if(dimPattern == NUM_DIM_PATTERNS-1) {
        do { randomDimPatternIndex = random8(NUM_DIM_PATTERNS-1); } while(randomDimPatternIndex == lastDimPattern);
        pg.WriteDimPattern(randomDimPatternIndex, targetDimPattern);
      }
      else {
        pg.WriteDimPattern(targetDimPatternIndex, targetDimPattern);
      }
      
      lastDimPatternChange = curTime;
    }
}

void PatternController::ScaleParams(struct_base_show_params& params, uint32_t curTime) {
  #ifdef EXPLICIT_PARAMETERS
    SetDisplayMode(params.displayMode / NUM_DIM_PATTERNS, params.displayMode % NUM_DIM_PATTERNS, curTime);
    dimSpeed = params.dimSpeed;
    colorSpeed = params.colorSpeed;
    numColors = params.numColors;
    colorThickness = params.colorThickness;
    transLength = params.transLength;
    brightLength = params.brightLength;
    spacing = params.spacing;
  #else  
    uint8_t displayMode = scaleParam(params.displayMode, 0, NUM_DIM_PATTERNS * NUM_COLOR_PATTERNS - 1);
    SetDisplayMode(displayMode / NUM_DIM_PATTERNS, displayMode % NUM_DIM_PATTERNS, curTime);

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

void PatternController::Update(struct_base_show_params& params, CRGB* target, uint8_t* target_b, uint16_t numLEDs, uint32_t curTime) {
  ScaleParams(params, curTime);
  WalkSpeeds();

  if(WalkColorParams()) {
    if(oldColorPatternIndex != 0xFF) { pg.WriteColorPattern(oldColorPatternIndex, curColorPattern); }
    pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);
  }  
  if(WalkDimParams()) {
    if(oldDimPatternIndex != 0xFF) { pg.WriteDimPattern(oldDimPatternIndex, curDimPattern); }
    pg.WriteDimPattern(targetDimPatternIndex, targetDimPattern);
  }

  BlendColorPattern(curTime);
  BlendDimPattern(curTime);
  
  pr.SetColorPattern(curColorPattern, pg.GetColorPeriod(targetColorPatternIndex));
  pr.SetDimPattern(curDimPattern, pg.GetDimPeriod());
  
  pr.Update(curTime);
  pr.SetCRGBs(target, target_b, numLEDs);
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

bool PatternController::WalkDimParams() {
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

void PatternController::BlendColorPattern(uint32_t curTime) {
  memcpy(curColorPattern, targetColorPattern, pg.GetColorPeriod(targetColorPatternIndex));
}

void PatternController::BlendDimPattern(uint32_t curTime) {
  if(curTime - lastDimPatternChange >= dimPatternPauseLength) {
    uint32_t transitionTime = curTime - lastDimPatternChange - dimPatternPauseLength;
    if(transitionTime < dimPatternBlendLength) {
      uint8_t blendAmount = 255 * transitionTime / dimPatternBlendLength;//debug: changed from 256, why?
  
      for(uint8_t i = 0; i < pg.GetDimPeriod(); i++) {
        curDimPattern[i] = (255 - blendAmount) * oldDimPattern[i] / 255 + blendAmount * targetDimPattern[i] / 255;
      }
  
      pr.SetDimPattern(targetDimPattern, pg.GetDimPeriod());
    }
    else {
      // Blending just finished
      oldDimPatternIndex = targetDimPatternIndex;
      memcpy(oldDimPattern, targetDimPattern, pg.GetDimPeriod());
      memcpy(curDimPattern, targetDimPattern, pg.GetDimPeriod());
      
      if(targetDimPatternIndex == NUM_DIM_PATTERNS-1) {
        uint8_t lastRandomIndex = randomDimPatternIndex;
        do { randomDimPatternIndex = random8(NUM_DIM_PATTERNS-1); } while (randomDimPatternIndex == lastRandomIndex);
        pg.WriteDimPattern(randomDimPatternIndex, targetDimPattern);
      }
      else {
        pg.WriteDimPattern(targetDimPatternIndex, targetDimPattern);
      }

      lastDimPatternChange = curTime;
    }
  }
}

