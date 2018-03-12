PatternRepeater::PatternRepeater() {
  colorPatternLength = 1;
  colorPattern[0] = { 0, 0, 0 };
  lastColorMove = 0;
  lastBrightnessMove = 0;
  spacing = 0;
  transLength = 0;
  brightLength = 0;
  numColors = 1;

  for(uint8_t i = 0; i < GetPeriod(); i++) {
    brightnessPattern[i] = 255;
  }
}

uint8_t PatternRepeater::GetPeriod() {
  return spacing + 2*transLength + brightLength + 2;
}

bool PatternRepeater::BrightnessPatternIsInitialized() {
  return spacing > 0 || transLength > 0 || brightLength > 0;
}

void PatternRepeater::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastBrightnessMove += amount;
}

void PatternRepeater::SetColorPattern(PRGB* newPattern, uint8_t newColorPatternLength) {
  colorPatternLength = newColorPatternLength;
  
  if(colorSpeed > 0) {
    colorIndexFirst = 0;
    colorIndexLast = (NUM_LEDS-1) % newColorPatternLength;
  }
  else {
    colorIndexFirst = colorPatternLength - 1 - ((NUM_LEDS-1) % colorPatternLength);
    colorIndexLast = colorPatternLength - 1;
  }

  memcpy(colorPattern, newPattern, sizeof(PRGB)*colorPatternLength);
  //Serial.println("TEST: SetColorPattern()");
}

void PatternRepeater::SetBrightnessPattern(uint8_t* newPattern) {
  if(brightnessSpeed > 0) {
    brightnessIndexFirst = 0;
    brightnessIndexLast = (NUM_LEDS-1) % GetPeriod();
  }
  else {
    brightnessIndexFirst = GetPeriod() - 1 - ((NUM_LEDS-1) % GetPeriod());
    brightnessIndexLast = GetPeriod() - 1;
  }

  memcpy(brightnessPattern, newPattern, GetPeriod());
  //for(uint8_t i = 0; i < GetPeriod(); i++)
    //Serial.println(String(i) + ": " + brightnessPattern[i]);
}

void PatternRepeater::Init(uint32_t curTime) {
  colorIndexFirst = 0;
  colorIndexLast = (NUM_LEDS-1) % colorPatternLength;

  if(BrightnessPatternIsInitialized()) {
    brightnessIndexFirst = 0;
    brightnessIndexLast = (NUM_LEDS-1) % GetPeriod();
  }

  lastBrightnessMove = curTime;
  lastColorMove = curTime;
}

void PatternRepeater::Update(uint32_t& currentTime) {
  bool brightnessMoved = false;
  if(brightnessSpeed == 0) {
    lastBrightnessMove = currentTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(brightnessSpeed);
    if(currentTime - lastBrightnessMove >= stepSize) {
      ScrollBrightness(brightnessSpeed > 0);
      lastBrightnessMove += stepSize;
      brightnessMoved = true;
    }
  }

  // If color is moving into the brightness, sync up the movement frames to avoid flicker
  //bool mustMoveColor = brightnessMoved && (((brightnessSpeed > 0) && (colorSpeed > brightnessSpeed)) || ((brightnessSpeed < 0) && (colorSpeed < brightnessSpeed)));
  if(colorSpeed == 0) {
    lastColorMove = currentTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(colorSpeed);
    if(/*mustMoveColor ||*/ ((currentTime > lastColorMove) && (currentTime - lastColorMove >= stepSize))) {
      ScrollPattern(colorSpeed > 0);
      lastColorMove += stepSize;
      //Serial.println("mustMove: " + String(mustMoveColor));
    }
  }

  //Serial.println("TEST: lastColorMove: " + String(lastColorMove));
}

void PatternRepeater::ScrollPattern(bool scrollForward) {
  if(scrollForward) {
    // Scroll colors forward
    if(colorParamWaitCounter < 2*PATTERN_PARAM_CHANGE_DISTANCE) { colorParamWaitCounter++; }

    // Adjust indexes
    if(--colorIndexFirst == 255) { colorIndexFirst = colorPatternLength - 1; }
    if(--colorIndexLast  == 255) { colorIndexLast  = colorPatternLength - 1; }
  }
  else {
    // Scroll colors backward
    if(colorParamWaitCounter > 0) { colorParamWaitCounter--; }
    
    // Adjust indexes
    if(++colorIndexFirst == colorPatternLength) { colorIndexFirst = 0; }
    if(++colorIndexLast  == colorPatternLength) { colorIndexLast  = 0; }
  }
}

void PatternRepeater::ScrollBrightness(bool scrollForward) {
  if(scrollForward) {
    // Scroll brightnesses forward
    if(brightnessParamWaitCounter < 2*BRIGHTNESS_PARAM_CHANGE_DISTANCE) { brightnessParamWaitCounter++; }
    
    // Adjust indexes
    if(--brightnessIndexFirst == 255) { brightnessIndexFirst = GetPeriod() - 1; }
    if(--brightnessIndexLast  == 255) { brightnessIndexLast  = GetPeriod() - 1; }
  }
  else {
    // Scroll pixels backward
    if(brightnessParamWaitCounter > 0) { brightnessParamWaitCounter--; }
    
    // Adjust indexes
    if(++brightnessIndexFirst == GetPeriod()) { brightnessIndexFirst = 0; }
    if(++brightnessIndexLast  == GetPeriod()) { brightnessIndexLast  = 0; }
  }
}

bool PatternRepeater::IsReadyForBrightnessChange() {
  if(brightnessParamWaitCounter > 0 && brightnessParamWaitCounter < 2*BRIGHTNESS_PARAM_CHANGE_DISTANCE) { return false; }
  if(brightnessSpeed == 0) { return brightnessIndexFirst == 0 || brightnessIndexLast == GetPeriod() - 1; }
  else if(brightnessSpeed >= 0) { return brightnessIndexFirst == 0; }
  else { return brightnessIndexLast == GetPeriod() - 1; }
}

bool PatternRepeater::IsReadyForPatternChange() {
  if(colorParamWaitCounter > 0 && colorParamWaitCounter < 2*PATTERN_PARAM_CHANGE_DISTANCE) { return false; }
  if(colorSpeed == 0) { return colorIndexFirst == 0 || colorIndexLast == colorPatternLength - 1; }
  else if(colorSpeed > 0) { return colorIndexFirst == 0; }
  else { return colorIndexLast == colorPatternLength - 1; }
}

void PatternRepeater::SetCRGBs(CRGB* target, uint8_t numLEDs, PaletteManager& pm) {
  uint16_t curColorIndex = colorIndexFirst;
  uint16_t curBrightnessIndex = brightnessIndexFirst;

  for(uint8_t i = 0; i < numLEDs; i++) {
    CHSV temp = blend(pm.palette[colorPattern[curColorIndex].a], pm.palette[colorPattern[curColorIndex].b], colorPattern[curColorIndex].blendAmount, SHORTEST_HUES);
    temp.v = brightnessPattern[curBrightnessIndex];
    target[i] = temp;

    if(++curColorIndex == colorPatternLength) { curColorIndex = 0; }
    if(++curBrightnessIndex == GetPeriod()) { curBrightnessIndex = 0; }
  }
}

void PatternRepeater::BrightnessParametersChanged() {
  brightnessParamWaitCounter = BRIGHTNESS_PARAM_CHANGE_DISTANCE;
}

void PatternRepeater::PatternParametersChanged() {
  colorParamWaitCounter = PATTERN_PARAM_CHANGE_DISTANCE;
}

