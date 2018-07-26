PatternRepeater::PatternRepeater() {
  colorPatternLength = 1;
  colorPattern[0] = { 0, 0, 0 };
  lastColorMove = 0;
  lastBrightnessMove = 0;
  spacing = 0;
  transLength = 0;
  brightLength = 0;
  numColors = 1;
  myBrightness = 255;
  brightnessPatternBlendTime = ONE_SECOND;
  brightnessPatternLength = NUM_LEDS;

  for(uint16_t i = 0; i < brightnessPatternLength; i++) {
    brightnessPattern[i] = 255;
  }
}

bool PatternRepeater::BrightnessPatternIsInitialized() {
  return spacing > 0 || transLength > 0 || brightLength > 0;
}

void PatternRepeater::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastBrightnessMove += amount;
}

void PatternRepeater::SetColorPattern(PRGB* newPattern, uint16_t newColorPatternLength) {
  if(colorPatternLength != newColorPatternLength) {
    colorIndexFirst = 0;
  }
  colorPatternLength = newColorPatternLength;
  memcpy(colorPattern, newPattern, sizeof(PRGB)*colorPatternLength);
}

void PatternRepeater::SetBrightnessPattern(uint8_t* newPattern, uint16_t newBrightnessPatternLength, uint32_t curTime, bool isParamChange) {
  if(brightnessPatternLength != newBrightnessPatternLength) { brightnessIndexFirst = 0; }
  brightnessPatternLength = newBrightnessPatternLength;

  memcpy(lastBrightnessPattern, brightnessPattern, brightnessPatternLength);
  memcpy(nextBrightnessPattern, newPattern, brightnessPatternLength);
  
  if(!isParamChange) { lastBrightnessPatternChange = curTime; }
  
  //for(uint8_t i = 0; i < brightnessPatternLength; i++)
    //Serial.println(String(i) + ": " + brightnessPattern[i]);
}

void PatternRepeater::Init(uint32_t curTime) {
  colorIndexFirst = 0;

  if(BrightnessPatternIsInitialized()) {
    brightnessIndexFirst = 0;
  }
  
  lastBrightnessMove = curTime;
  lastColorMove = curTime;
}

void PatternRepeater::Update(uint32_t& currentTime) {
  //uint8_t retVal = 0; // Return value reflects if color and/or pattern moved on this update
  
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

  // Move brightness pattern
  //bool brightnessMoved = false;
  if(brightnessSpeed == 0) {
    lastBrightnessMove = currentTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(brightnessSpeed);
    if(currentTime - lastBrightnessMove >= stepSize) {
      ScrollBrightnessPattern(brightnessSpeed > 0);
      lastBrightnessMove += stepSize;
      //brightnessMoved = true;
      //retVal |= BRIGHTNESS_PATTERN_MOVED;
    }
  }

  // Move color pattern
  if(colorSpeed == 0) {
    lastColorMove = currentTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(colorSpeed);
    if(currentTime - lastColorMove >= stepSize) {
      ScrollColorPattern(colorSpeed > 0);
      lastColorMove += stepSize;
      //retVal |= COLOR_PATTERN_MOVED;
    }
  }

  //Serial.println("TEST: lastColorMove: " + String(lastColorMove));
  //return retVal;
}

void PatternRepeater::ScrollColorPattern(bool scrollForward) {
  if(scrollForward) {
    // Scroll colors forward
    if(colorParamWaitCounter < 2*PATTERN_PARAM_CHANGE_DISTANCE) { colorParamWaitCounter++; }

    // Adjust indexes
    if(--colorIndexFirst == 65535) { colorIndexFirst = colorPatternLength - 1; }
  }
  else {
    // Scroll colors backward
    if(colorParamWaitCounter > 0) { colorParamWaitCounter--; }
    
    // Adjust indexes
    if(++colorIndexFirst == colorPatternLength) { colorIndexFirst = 0; }
  }
}

void PatternRepeater::ScrollBrightnessPattern(bool scrollForward) {
  if(scrollForward) {
    // Scroll brightnesses forward
    if(brightnessParamWaitCounter < 2*BRIGHTNESS_PARAM_CHANGE_DISTANCE) { brightnessParamWaitCounter++; }

    // Adjust indexes
    if(--brightnessIndexFirst == 65535) { brightnessIndexFirst = brightnessPatternLength - 1; }
  }
  else {
    // Scroll pixels backward
    if(brightnessParamWaitCounter > 0) { brightnessParamWaitCounter--; }
    
    // Adjust indexes
    if(++brightnessIndexFirst == brightnessPatternLength) { brightnessIndexFirst = 0; }
  }
}

bool PatternRepeater::IsReadyForBrightnessChange(uint32_t currentTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(brightnessSpeed == 0) { return false; }
  
  uint32_t stepSize = ONE_SECOND / abs(brightnessSpeed);
  return currentTime - lastBrightnessMove >= stepSize;
}

bool PatternRepeater::IsReadyForColorPatternChange(uint32_t currentTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(colorSpeed == 0) { return false; }
  
  uint32_t stepSize = ONE_SECOND / abs(colorSpeed);

  //Serial.println(String(currentTime) + " : " + String(lastColorMove));
  return currentTime - lastColorMove >= stepSize;
}

void PatternRepeater::SetCRGBs(CRGB* target, uint16_t numLEDs, PaletteManager& pm) {
  uint16_t curColorIndex = colorIndexFirst;
  uint16_t curBrightnessIndex = brightnessIndexFirst;

  CRGB temp;
  for(uint16_t i = 0; i < numLEDs; i++) {
    // Blend using CRGB; this adds saturation and brightness but avoids jumping directions around the color wheel
    temp = pm.palette[colorPattern[curColorIndex].a];
    target[i] = blend(temp, pm.palette[colorPattern[curColorIndex].b], colorPattern[curColorIndex].blendAmount);
    target[i] %= brightnessPattern[curBrightnessIndex] * myBrightness / 255;
    #ifdef DEBUG_ERRORS
      if(curBrightnessIndex >= brightnessPatternLength) { Serial.println("ERROR: SetCRGBs(): curBrightnessIndex out of bounds."); }
      if(curColorIndex >= colorPatternLength) { Serial.println("ERROR: SetCRGBs(): curColorIndex out of bounds."); }
    #endif

    //Serial.println(String(i) + ": " + String(colorPattern[curColorIndex].a));

    if(++curColorIndex == colorPatternLength) { curColorIndex = 0; }
    if(++curBrightnessIndex == brightnessPatternLength) { curBrightnessIndex = 0; }
  }
}

void PatternRepeater::BrightnessParametersChanged() {
  brightnessParamWaitCounter = BRIGHTNESS_PARAM_CHANGE_DISTANCE;
}

void PatternRepeater::ColorPatternParametersChanged() {
  colorParamWaitCounter = PATTERN_PARAM_CHANGE_DISTANCE;
}

