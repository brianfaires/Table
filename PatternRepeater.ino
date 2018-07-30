  PatternRepeater::PatternRepeater() {
  colorPatternLength = 1;
  colorPattern[0] = { 0, 0, 0 };
  colorIndexFirst = 0;
  lastColorMove = 0;
  colorSpeed = 0;
  
  brightnessPatternLength = 1;
  brightnessPattern[0] = 255;
  brightnessIndexFirst = 0;
  lastBrightnessMove = 0;
  brightnessSpeed = 0;
  
  myBrightness = 255;
}

void PatternRepeater::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastBrightnessMove += amount;
}

void PatternRepeater::SetColorPattern(PRGB* newPattern, uint16_t newColorPatternLength) {
  if(colorPatternLength != newColorPatternLength) { colorIndexFirst = 0; }
  colorPatternLength = newColorPatternLength;
  memcpy(colorPattern, newPattern, sizeof(PRGB)*colorPatternLength);
}

void PatternRepeater::SetBrightnessPattern(uint8_t* newPattern, uint16_t newBrightnessPatternLength) {
  if(brightnessPatternLength != newBrightnessPatternLength) { brightnessIndexFirst = 0; }
  brightnessPatternLength = newBrightnessPatternLength;
  memcpy(brightnessPattern, newPattern, brightnessPatternLength);
}

void PatternRepeater::Init(uint32_t curTime) {
  colorIndexFirst = 0;
  brightnessIndexFirst = 0;
  
  lastBrightnessMove = curTime;
  lastColorMove = curTime;
}

void PatternRepeater::Update(uint32_t& curTime) {
  // Move brightness pattern
  if(brightnessSpeed == 0) {
    lastBrightnessMove = curTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(brightnessSpeed);
    if(curTime - lastBrightnessMove >= stepSize) {
      ScrollBrightnessPattern(brightnessSpeed > 0);
      lastBrightnessMove += ONE_SECOND / abs(brightnessSpeed);
    }
  }

  // Move color pattern
  if(colorSpeed == 0) {
    lastColorMove = curTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(colorSpeed);
    if(curTime - lastColorMove >= stepSize) {
      ScrollColorPattern(colorSpeed > 0);
      lastColorMove += ONE_SECOND / abs(colorSpeed);
    }
  }
}

void PatternRepeater::ScrollColorPattern(bool scrollForward) {
  if(scrollForward) {
    if(--colorIndexFirst == 0xFFFF) { colorIndexFirst = colorPatternLength - 1; }
  }
  else {
    if(++colorIndexFirst == colorPatternLength) { colorIndexFirst = 0; }
  }
}

void PatternRepeater::ScrollBrightnessPattern(bool scrollForward) {
  if(scrollForward) {
    if(--brightnessIndexFirst == 0xFFFF) { brightnessIndexFirst = brightnessPatternLength - 1; }
  }
  else {
    if(++brightnessIndexFirst == brightnessPatternLength) { brightnessIndexFirst = 0; }
  }
}

bool PatternRepeater::IsReadyForBrightnessMove(uint32_t curTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(brightnessSpeed == 0) { return true; }
  
  uint32_t stepSize = ONE_SECOND / abs(brightnessSpeed);
  return curTime - lastBrightnessMove >= stepSize;
}

bool PatternRepeater::IsReadyForColorMove(uint32_t currentTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(colorSpeed == 0) { return true; }
  
  uint32_t stepSize = ONE_SECOND / abs(colorSpeed);
  return currentTime - lastColorMove >= stepSize;
}

void PatternRepeater::SetCRGBs(CRGB* target, uint16_t numLEDs, PaletteManager& pm) {
  uint16_t curColorIndex = colorIndexFirst;
  uint16_t curBrightnessIndex = brightnessIndexFirst;


  CRGB temp;
  for(uint16_t i = 0; i < numLEDs; i++) {
    // Blend using CRGB; this adds saturation and brightness but avoids jumping directions around the color wheel
    temp = pm.palette[colorPattern[curColorIndex].a];
    temp = ReverseGammaCorrect(temp);
    target[i] = blend(temp, ReverseGammaCorrect(pm.palette[colorPattern[curColorIndex].b]), colorPattern[curColorIndex].blendAmount);


    /*
    target[i].maximizeBrightness();
    CHSV temp2 = rgb2hsv_approximate(target[i]);
    temp2.v = brightnessPattern[curBrightnessIndex] * myBrightness / 255;
    target[i] = temp2;
    */
    target[i] %= brightnessPattern[curBrightnessIndex] * myBrightness / 255;
    target[i] = GammaCorrect(target[i]);
    
    #ifdef DEBUG_ERRORS
      if(curBrightnessIndex >= brightnessPatternLength) { Serial.println("ERROR: SetCRGBs(): curBrightnessIndex out of bounds: " + 
      String(curBrightnessIndex) + " / " + String(brightnessPatternLength)); }
      if(curColorIndex >= colorPatternLength) { Serial.println("ERROR: SetCRGBs(): curColorIndex out of bounds: " + 
      String(curColorIndex) + " / " + String(colorPatternLength)); }
    #endif

    //Serial.println(String(i) + ": " + String(colorPattern[curColorIndex].a) + ", "  + String(colorPattern[curColorIndex].b) + ", "  + String(colorPattern[curColorIndex].blendAmount));
    
    if(++curColorIndex == colorPatternLength) { curColorIndex = 0; }
    if(++curBrightnessIndex == brightnessPatternLength) { curBrightnessIndex = 0; }
  }
/*
  Serial.println("SetCRGBs(): ");
  for(uint8_t ii = 0; ii < numLEDs; ii++) {
    Serial.print(String(ii) + " = " + String(brightnessPattern[ii % brightnessPatternLength]) + "\t");
    Serial.println(String(target[ii].b));
  }*/
}

