PatternScroller::PatternScroller() {
  colorPeriod = 1;
  colorPattern[0] = { 0, 0, 0 };
  lastColorMove = 0;
  lastDimMove = 0;

  brightnessPeriod = NUM_LEDS;
  myBrightness = 64;

  for(uint16_t i = 0; i < brightnessPeriod; i++) {
    brightnessPattern[i] = 255;
  }
}


void PatternScroller::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastDimMove += amount;
}

void PatternScroller::SetColorPattern(PRGB* newPattern, uint16_t newColorPeriod) {
  colorPeriod = newColorPeriod;
  
  if(colorSpeed > 0) {
    colorIndexFirst = 0;
    colorIndexLast = (NUM_LEDS-1) % newColorPeriod;
  }
  else {
    colorIndexFirst = colorPeriod - 1 - ((NUM_LEDS-1) % colorPeriod);
    colorIndexLast = colorPeriod - 1;
  }

  memcpy(colorPattern, newPattern, sizeof(PRGB)*colorPeriod);
  //Serial.println("TEST: SetColorPattern()");
}

void PatternScroller::SetDimPattern(uint8_t* newPattern, uint16_t newDimPeriod, uint32_t curTime, bool isParamChange) {
  if(dimSpeed > 0) {
    brightnessIndexFirst = 0;
    brightnessIndexLast = (NUM_LEDS-1) % brightnessPeriod;
  }
  else {
    brightnessIndexFirst = brightnessPeriod - 1 - ((NUM_LEDS-1) % brightnessPeriod);
    brightnessIndexLast = brightnessPeriod - 1;
  }

  memcpy(brightnessPattern, newPattern, brightnessPeriod);
  //for(uint8_t i = 0; i < brightnessPeriod; i++)
    //Serial.println(String(i) + ": " + brightnessPattern[i]);
}

void PatternScroller::Init(uint32_t curTime) {
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    colors[i] = { colorPattern[i % colorPeriod].a, colorPattern[i % colorPeriod].b, colorPattern[i % colorPeriod].blendAmount };
  }

  colorIndexFirst = 0;
  colorIndexLast = (NUM_LEDS-1) % colorPeriod;

  //if(BrightnessPatternIsInitialized()) {
    for(uint16_t i = 0; i < NUM_LEDS; i++) {
      brightnesses[i] = brightnessPattern[i % brightnessPeriod];
  //}

    brightnessIndexFirst = 0;
    brightnessIndexLast = (NUM_LEDS-1) % brightnessPeriod;
  }

  lastDimMove = curTime;
  lastColorMove = curTime;
}

void PatternScroller::Update(uint32_t& curTime) {
  //uint8_t retVal = 0; // Return value reflects if color and/or brightness patterns moved
  
  bool brightnessMoved = false;
  if(dimSpeed == 0) {
    lastDimMove = curTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(dimSpeed);
    if(curTime - lastDimMove >= stepSize) {
      ScrollDimPattern(dimSpeed > 0);
      lastDimMove += stepSize;
      brightnessMoved = true;
    }
  }

  // If color is moving into the brightness, sync up the movement frames to avoid flicker
  //bool mustMoveColor = brightnessMoved && (((dimSpeed > 0) && (colorSpeed > dimSpeed)) || ((dimSpeed < 0) && (colorSpeed < dimSpeed)));
  if(colorSpeed == 0) {
    lastColorMove = curTime;
  }
  else {
    uint32_t stepSize = ONE_SEC_US / abs(colorSpeed);
    if(/*mustMoveColor ||*/ ((curTime > lastColorMove) && (curTime - lastColorMove >= stepSize))) {
      ScrollColorPattern(colorSpeed > 0);
      lastColorMove += stepSize;
      //Serial.println("mustMove: " + String(mustMoveColor));
    }
  }
}

void PatternScroller::ScrollColorPattern(bool scrollForward) {
  if(scrollForward) {
    // Scroll colors forward
    if(colorParamWaitCounter < 2*PATTERN_PARAM_CHANGE_DISTANCE) { colorParamWaitCounter++; }
    
    for(uint16_t i = NUM_LEDS-1; i > 0; i--)
      colors[i] = colors[i-1];

    // Adjust indexes
    if(--colorIndexFirst == 0xFFFF) { colorIndexFirst = colorPeriod - 1; }
    if(--colorIndexLast  == 0xFFFF) { colorIndexLast  = colorPeriod - 1; }
    
    // Write the new color
    colors[0] = colorPattern[colorIndexFirst];
  }
  else {
    // Scroll colors backward
    if(colorParamWaitCounter > 0) { colorParamWaitCounter--; }
    
    for(uint16_t i = 0; i < NUM_LEDS-1; i++)
      colors[i] = colors[i+1];

    // Adjust indexes
    if(++colorIndexFirst == colorPeriod) { colorIndexFirst = 0; }
    if(++colorIndexLast  == colorPeriod) { colorIndexLast  = 0; }

    // Write the new color
    colors[NUM_LEDS-1] = colorPattern[colorIndexLast];
  }
}

void PatternScroller::ScrollDimPattern(bool scrollForward) {
  if(scrollForward) {
    // Scroll brightnesses forward
    if(brightnessParamWaitCounter < 2*BRIGHTNESS_PARAM_CHANGE_DISTANCE) { brightnessParamWaitCounter++; }
    
    for(uint16_t i = NUM_LEDS-1; i > 0; i--)
      brightnesses[i] = brightnesses[i-1];

    // Adjust indexes
    if(--brightnessIndexFirst == 0xFFFF) { brightnessIndexFirst = brightnessPeriod - 1; }
    if(--brightnessIndexLast  == 0xFFFF) { brightnessIndexLast  = brightnessPeriod - 1; }
    
    // Write the new brightness
    brightnesses[0] = brightnessPattern[brightnessIndexFirst];
  }
  else {
    // Scroll pixels backward
    if(brightnessParamWaitCounter > 0) { brightnessParamWaitCounter--; }
    
    for(uint16_t i = 0; i < NUM_LEDS-1; i++)
      brightnesses[i] = brightnesses[i+1];

    // Adjust indexes
    if(++brightnessIndexFirst == brightnessPeriod) { brightnessIndexFirst = 0; }
    if(++brightnessIndexLast  == brightnessPeriod) { brightnessIndexLast  = 0; }

    brightnesses[NUM_LEDS-1] = brightnessPattern[brightnessIndexLast];
  }
}

bool PatternScroller::IsReadyForBrightnessChange(uint32_t curTime) {
  uint32_t stepSize = ONE_SEC_US / abs(dimSpeed);
  if(curTime - lastDimMove < stepSize) { return false; }
  
  if(brightnessParamWaitCounter > 0 && brightnessParamWaitCounter < 2*BRIGHTNESS_PARAM_CHANGE_DISTANCE) { return false; }
  if(dimSpeed == 0) { return brightnessIndexFirst == 0 || brightnessIndexLast == brightnessPeriod - 1; }
  else if(dimSpeed >= 0) { return brightnessIndexFirst == 0; }
  else { return brightnessIndexLast == brightnessPeriod - 1; }
}

bool PatternScroller::IsReadyForColorPatternChange(uint32_t curTime) {
  uint32_t stepSize = ONE_SEC_US / abs(colorSpeed);
  if(curTime - lastColorMove < stepSize) { return false; }
  
  if(colorParamWaitCounter > 0 && colorParamWaitCounter < 2*PATTERN_PARAM_CHANGE_DISTANCE) { return false; }
  if(colorSpeed == 0) { return colorIndexFirst == 0 || colorIndexLast == colorPeriod - 1; }
  else if(colorSpeed > 0) { return colorIndexFirst == 0; }
  else { return colorIndexLast == colorPeriod - 1; }
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

