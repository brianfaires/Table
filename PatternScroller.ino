PatternScroller::PatternScroller() {
  colorPatternLength = 1;
  colorPattern[0] = { 0, 0, 0 };
  lastColorMove = 0;
  lastBrightnessMove = 0;
  spacing = 0;
  transLength = 0;
  brightLength = 0;
  numColors = 1;
  brightnessPatternLength = NUM_LEDS;
  myBrightness = 64;

  for(uint16_t i = 0; i < brightnessPatternLength; i++) {
    brightnessPattern[i] = 255;
  }
}

bool PatternScroller::BrightnessPatternIsInitialized() {
  return spacing > 0 || transLength > 0 || brightLength > 0;
}

void PatternScroller::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastBrightnessMove += amount;
}

void PatternScroller::SetColorPattern(PRGB* newPattern, uint16_t newColorPatternLength) {
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

void PatternScroller::SetBrightnessPattern(uint8_t* newPattern, uint16_t newBrightnessPatternLength, uint32_t curTime, bool isParamChange) {
  if(brightnessSpeed > 0) {
    brightnessIndexFirst = 0;
    brightnessIndexLast = (NUM_LEDS-1) % brightnessPatternLength;
  }
  else {
    brightnessIndexFirst = brightnessPatternLength - 1 - ((NUM_LEDS-1) % brightnessPatternLength);
    brightnessIndexLast = brightnessPatternLength - 1;
  }

  memcpy(brightnessPattern, newPattern, brightnessPatternLength);
  //for(uint8_t i = 0; i < brightnessPatternLength; i++)
    //Serial.println(String(i) + ": " + brightnessPattern[i]);
}

void PatternScroller::Init(uint32_t curTime) {
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    colors[i] = { colorPattern[i % colorPatternLength].a, colorPattern[i % colorPatternLength].b, colorPattern[i % colorPatternLength].blendAmount };
  }

  colorIndexFirst = 0;
  colorIndexLast = (NUM_LEDS-1) % colorPatternLength;

  if(BrightnessPatternIsInitialized()) {
    for(uint16_t i = 0; i < NUM_LEDS; i++) {
      brightnesses[i] = brightnessPattern[i % brightnessPatternLength];
    }

    brightnessIndexFirst = 0;
    brightnessIndexLast = (NUM_LEDS-1) % brightnessPatternLength;
  }

  lastBrightnessMove = curTime;
  lastColorMove = curTime;
}

void PatternScroller::Update(uint32_t& currentTime) {
  //uint8_t retVal = 0; // Return value reflects if color and/or brightness patterns moved
  
  bool brightnessMoved = false;
  if(brightnessSpeed == 0) {
    lastBrightnessMove = currentTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(brightnessSpeed);
    if(currentTime - lastBrightnessMove >= stepSize) {
      ScrollBrightnessPattern(brightnessSpeed > 0);
      lastBrightnessMove += stepSize;
      brightnessMoved = true;
      //retVal |= BRIGHTNESS_PATTERN_MOVED;
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
      ScrollColorPattern(colorSpeed > 0);
      lastColorMove += stepSize;
      //Serial.println("mustMove: " + String(mustMoveColor));
      //retVal |= COLOR_PATTERN_MOVED;
    }
  }

  //Serial.println("TEST: lastColorMove: " + String(lastColorMove));
  //return retVal;
}

void PatternScroller::ScrollColorPattern(bool scrollForward) {
  if(scrollForward) {
    // Scroll colors forward
    if(colorParamWaitCounter < 2*PATTERN_PARAM_CHANGE_DISTANCE) { colorParamWaitCounter++; }
    
    for(uint16_t i = NUM_LEDS-1; i > 0; i--)
      colors[i] = colors[i-1];

    // Adjust indexes
    if(--colorIndexFirst == 0xFFFF) { colorIndexFirst = colorPatternLength - 1; }
    if(--colorIndexLast  == 0xFFFF) { colorIndexLast  = colorPatternLength - 1; }
    
    // Write the new color
    colors[0] = colorPattern[colorIndexFirst];
  }
  else {
    // Scroll colors backward
    if(colorParamWaitCounter > 0) { colorParamWaitCounter--; }
    
    for(uint16_t i = 0; i < NUM_LEDS-1; i++)
      colors[i] = colors[i+1];

    // Adjust indexes
    if(++colorIndexFirst == colorPatternLength) { colorIndexFirst = 0; }
    if(++colorIndexLast  == colorPatternLength) { colorIndexLast  = 0; }

    // Write the new color
    colors[NUM_LEDS-1] = colorPattern[colorIndexLast];
  }
}

void PatternScroller::ScrollBrightnessPattern(bool scrollForward) {
  if(scrollForward) {
    // Scroll brightnesses forward
    if(brightnessParamWaitCounter < 2*BRIGHTNESS_PARAM_CHANGE_DISTANCE) { brightnessParamWaitCounter++; }
    
    for(uint16_t i = NUM_LEDS-1; i > 0; i--)
      brightnesses[i] = brightnesses[i-1];

    // Adjust indexes
    if(--brightnessIndexFirst == 0xFFFF) { brightnessIndexFirst = brightnessPatternLength - 1; }
    if(--brightnessIndexLast  == 0xFFFF) { brightnessIndexLast  = brightnessPatternLength - 1; }
    
    // Write the new brightness
    brightnesses[0] = brightnessPattern[brightnessIndexFirst];
  }
  else {
    // Scroll pixels backward
    if(brightnessParamWaitCounter > 0) { brightnessParamWaitCounter--; }
    
    for(uint16_t i = 0; i < NUM_LEDS-1; i++)
      brightnesses[i] = brightnesses[i+1];

    // Adjust indexes
    if(++brightnessIndexFirst == brightnessPatternLength) { brightnessIndexFirst = 0; }
    if(++brightnessIndexLast  == brightnessPatternLength) { brightnessIndexLast  = 0; }

    brightnesses[NUM_LEDS-1] = brightnessPattern[brightnessIndexLast];
  }
}

bool PatternScroller::IsReadyForBrightnessChange(uint32_t currentTime) {
  uint32_t stepSize = ONE_SECOND / abs(brightnessSpeed);
  if(currentTime - lastBrightnessMove < stepSize) { return false; }
  
  if(brightnessParamWaitCounter > 0 && brightnessParamWaitCounter < 2*BRIGHTNESS_PARAM_CHANGE_DISTANCE) { return false; }
  if(brightnessSpeed == 0) { return brightnessIndexFirst == 0 || brightnessIndexLast == brightnessPatternLength - 1; }
  else if(brightnessSpeed >= 0) { return brightnessIndexFirst == 0; }
  else { return brightnessIndexLast == brightnessPatternLength - 1; }
}

bool PatternScroller::IsReadyForColorPatternChange(uint32_t currentTime) {
  uint32_t stepSize = ONE_SECOND / abs(colorSpeed);
  if(currentTime - lastColorMove < stepSize) { return false; }
  
  if(colorParamWaitCounter > 0 && colorParamWaitCounter < 2*PATTERN_PARAM_CHANGE_DISTANCE) { return false; }
  if(colorSpeed == 0) { return colorIndexFirst == 0 || colorIndexLast == colorPatternLength - 1; }
  else if(colorSpeed > 0) { return colorIndexFirst == 0; }
  else { return colorIndexLast == colorPatternLength - 1; }
}

void PatternScroller::SetCRGBs(CRGB* target, uint16_t numLEDs, PaletteManager& pm) {
  CHSV temp;
  for(uint16_t i = 0; i < numLEDs; i++) {
    // Blend using CRGB; this adds saturation and brightness but avoids jumping directions around the color wheel
    temp = pm.palette[colors[i].a];
    target[i] = blend(temp, pm.palette[colors[i].b], colors[i].blendAmount);
    target[i] %= brightnesses[i] * myBrightness / 255;
  }
}

void PatternScroller::BrightnessParametersChanged() {
  brightnessParamWaitCounter = BRIGHTNESS_PARAM_CHANGE_DISTANCE;
}

void PatternScroller::ColorPatternParametersChanged() {
  colorParamWaitCounter = PATTERN_PARAM_CHANGE_DISTANCE;
}

