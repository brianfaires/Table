// Base animations
const uint8_t FIRE_COOLING = 7;
const uint8_t FIRE_SPARKING = 120;
const uint8_t FIRE_MIN_SPARK_SIZE = 160;

void Fire() {
  static bool gReverseDirection = false;
  
  static CRGBPalette16 gPal = HeatColors_p;
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
  CreateFirePalette(&gPal);

  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, FIRE_COOLING));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < FIRE_SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(FIRE_MIN_SPARK_SIZE, 255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void CreateFirePalette(CRGBPalette16 *firePalette) {
  CRGB steps[3];
  hsv2rgb_rainbow(pm.palette[0], steps[0]);
  hsv2rgb_rainbow(pm.palette[2], steps[1]);
  hsv2rgb_rainbow(pm.palette[4], steps[2]);

  steps[0].maximizeBrightness();
  steps[1].maximizeBrightness();

  steps[0] %= 64; // Set to 1/4 of max brightness
  steps[1] %= 64;// Set to 1/4 of max brightness

  while(steps[0].r > 25 && steps[0].g > 25 && steps[0].b > 25) { steps[0]--; } // Manually increase saturation
  //while(steps[2].r < 255 && steps[2].g < 255 && steps[3].b < 255) { steps[2]++; }
  steps[2] |= CRGB(40, 40, 40); // Raise each channel to a minimum of 40
  steps[2].maximizeBrightness();
  steps[2] %= 64;// Set to 1/4 of max brightness

  CRGB start, target;

  // Step 1: Black to dim, saturated color0
  target = steps[0];
  for(uint8_t i = 0; i < 90; i++) {
    start = CRGB::Black;
    nblend(start, target, 255 * i / 90);
    firePalette->entries[i] = start;
  }

  // Step 2: color0 to color1, up value and drop saturation
  target = steps[1];
  for(uint8_t i = 90; i < 160; i++) {
    start = steps[0];
    nblend(start, target, 255 * (i-90) / 70);
    firePalette->entries[i] = start;
  }

  // Step 3: color1 to color2, up value and drop saturation
  target = steps[2];
  for(uint8_t i = 160; i < 240; i++) {
    start = steps[1];
    nblend(start, target, 255 * (i-160) / 80);
    firePalette->entries[i] = start;
  }

  // Step 4: blend into (almost) fully bright white
  target = CRGB(255, 240, 230);
  nblend(target, steps[2], 100); // ~60% white
  for(uint16_t i = 240; i < 256; i++) {
    start = steps[2];
    nblend(start, target, 255 * (i-240) / 16);
    firePalette->entries[i] = start;
  }
}


// Under development

void ScrollingSaturation(uint8_t nWhite, uint8_t nTrans, uint8_t nPure) {
  const uint8_t maxBlendAmount = 160;
  //const uint8_t whiteBrightness = 210;

  uint8_t period = nWhite + nPure + 2*nTrans;
  uint16_t extendedLEDCount = ((NUM_LEDS-1)/period+1)*period;
  //CRGB myWhite = CRGB(whiteBrightness, whiteBrightness, whiteBrightness);
  uint16_t i = 0;
  while(i < extendedLEDCount) {
    // progressively blend the LEDs toward white
    for(uint16_t j = 0; j < period - nPure; j++) {
      int16_t offset = 0;//baseCount*THROTTLE_MID_POINT/baseSpeed2;
      uint16_t idx = (i + offset) % extendedLEDCount;
      if(i++ >= extendedLEDCount) { return; }
      if(idx >= NUM_LEDS) continue;


      if(j >= 2*nTrans + nWhite) {
        // skip over nPure LEDs to leave them at full saturation
        i += nPure;
      }
      else {
        uint8_t whiteBrightness = (leds[idx].r + leds[idx].g + leds[idx].b) / 3;
        CRGB myWhite = CRGB(whiteBrightness, whiteBrightness, whiteBrightness);
        
        if(j < nTrans)
          nblend(&leds[idx], &myWhite, 1, maxBlendAmount * j / nTrans);
        else if(j < nTrans + nWhite)
          nblend(&leds[idx], &myWhite, 1, maxBlendAmount);
        else //if(j < 2*nTrans + nWhite)
          nblend(&leds[idx], &myWhite, 1, maxBlendAmount * (2*nTrans + nWhite - j) / nTrans);
      }
    }
  }
}


void DiscoFire() {
  
}

void Orbs() {
  const uint8_t centerPoint = 51;
  const uint8_t minRadius = 3;
  //const uint8_t maxRadius = 51;
  const uint8_t fadeRate = 64;
    
  uint8_t curRadius;
  CRGB colorToUse;
  
  int16_t radiusOffset = 0;//round((maxRadius-minRadius) * (sin8(baseCount % 255) - 128.0) / 128.0);
  if(radiusOffset < 0) {
    hsv2rgb_rainbow(pm.palette[2], colorToUse);
    curRadius = minRadius - radiusOffset;
  }
  else {
    hsv2rgb_rainbow(pm.palette[3], colorToUse);
    curRadius = minRadius + radiusOffset;
  }
  
  // Draw
  leds[centerPoint] = colorToUse;

  for(uint16_t i = 1; i <= curRadius; i++) {
    if(centerPoint >= i)
      leds[centerPoint - i] = colorToUse;
    if(centerPoint + i < NUM_LEDS)
      leds[centerPoint + i] = colorToUse;
  }

  for(CRGB & pixel : leds) pixel.fadeToBlackBy(fadeRate);
}

void ScrollingGlimmerBands() {
  const uint8_t glimmerPortion = 4;
  const uint8_t glimmerFloor = 180;
  CRGB colorPattern[baseParams.dimPeriod];
  uint8_t spacing = baseParams.dimPeriod - baseParams.numColors * baseParams.brightLength;

  uint16_t curPixel = 0;
  for(uint8_t i = 0; i < baseParams.numColors; i++) {
    for(uint8_t j = 0; j < baseParams.brightLength; j++) {
      hsv2rgb_rainbow(pm.palette[2 + i], colorPattern[curPixel]);
      if(random8(glimmerPortion) == 0) {
        colorPattern[curPixel] |= glimmerFloor;
      }
      curPixel++;
    }
    for(uint8_t j = 0; j < spacing; j++) {
      colorPattern[curPixel++] = CRGB::Black;
    }
  }
}

void CenterSpawn() {
  // Propogate out
  for(uint16_t i = 0; i < NUM_LEDS/2; i++) {
    leds[i] = leds[i+1];
    leds[NUM_LEDS-1 - i] = leds[NUM_LEDS-2 - i];
  }

  uint8_t firstCol = 0;//(baseCount / 128) % 6;
  uint8_t secondCol = (firstCol + 1) % 6;
  uint8_t blendAmount = 0;//2*(baseCount % 128);

  
  leds[NUM_LEDS/2 - 1] = HSV2RGB(nblend(pm.palette[firstCol], pm.palette[secondCol], blendAmount), leds_b[NUM_LEDS/2 - 1]);
  leds[NUM_LEDS/2] = leds[NUM_LEDS/2 - 1];
  leds_b[NUM_LEDS/2] = leds_b[NUM_LEDS/2 - 1];
}

/////////////////// Stackers ///////////////////////
#define NUM_STACKERS 4
struct_stacker stackers[NUM_STACKERS];
void Stacks(uint32_t curTime) {  
  static uint16_t rotationIndex = NUM_LEDS - 18;
  static uint8_t clearMode = InitStackers(rotationIndex, curTime);
  static uint32_t lastRotation = curTime;
  
  uint8_t moveSpeed = scaleParam((uint8_t)2*abs(baseParams.dimSpeed), uint8_t(20), uint8_t(REFRESH_RATE));
  int8_t rotationSpeed = scaleParam(baseParams.colorSpeed, int8_t(-REFRESH_RATE), int8_t(REFRESH_RATE));

  // Clear display
  for(uint16_t i = 0; i < NUM_LEDS; i++) { leds[i] = CRGB::Black; leds_b[i] = 180; }

  // Rotate stackers
  if(rotationSpeed != 0) {
    while(curTime - lastRotation >= FPS_TO_TIME(abs(rotationSpeed))) {
      lastRotation += FPS_TO_TIME(abs(rotationSpeed));
      for(uint8_t i = 0; i < NUM_STACKERS; i++) {
        if(rotationSpeed < 0) {
          stackers[i].startPixel--;
          if(stackers[i].startPixel == 0xFFFF) { stackers[i].startPixel = NUM_LEDS-1; }
          stackers[i].endPixel--;
          if(stackers[i].endPixel == 0xFFFF) { stackers[i].endPixel = NUM_LEDS-1; }
        }
        else {
          stackers[i].startPixel++;
          if(stackers[i].startPixel == NUM_LEDS) { stackers[i].startPixel = 0; }
          stackers[i].endPixel++;
          if(stackers[i].endPixel == NUM_LEDS) { stackers[i].endPixel = 0; }
        }
      }
    }
  }
  
  // Draw stackers
  bool done = true;
  for(uint8_t i = 0; i < NUM_STACKERS; i++) {
    stackers[i].moveSpeed = stackers[i].moveForward ? moveSpeed + rotationSpeed : abs(rotationSpeed - moveSpeed);
    done &= DrawStacker(&stackers[i], curTime);
  }

  if(done) {
    // Clear stackers
    if(ClearStackers(clearMode, curTime)) {
      // Reset animation and load new params
      clearMode = InitStackers(rotationIndex, curTime);
    }
  }
}
uint8_t InitStackers(uint16_t rotationIndex, uint32_t curTime) {
  uint8_t numColors = scaleParam(baseParams.numColors, 1, 4);
  uint8_t stackSize = scaleParam(baseParams.brightLength, 5, 15);
  uint8_t spacing = scaleParam(baseParams.transLength, 0, 4);
  spacing += spacing % 2; // Keep spacing even
    
  stackers[0].startPixel = (0 + rotationIndex + NUM_LEDS) % NUM_LEDS;
  stackers[0].endPixel = (NUM_LEDS/4 - 1 - spacing/2 + rotationIndex + NUM_LEDS) % NUM_LEDS;
  
  if(NUM_STACKERS > 1) {
    stackers[1].startPixel = (NUM_LEDS-1 + rotationIndex + NUM_LEDS) % NUM_LEDS;
    stackers[1].endPixel = (NUM_LEDS*3/4 + spacing/2 + rotationIndex + NUM_LEDS) % NUM_LEDS;

    if(NUM_STACKERS > 2) {
      stackers[2].startPixel = (NUM_LEDS/2 + rotationIndex + NUM_LEDS) % NUM_LEDS;
      stackers[2].endPixel = (NUM_LEDS*3/4 - 1 - spacing/2 + rotationIndex + NUM_LEDS) % NUM_LEDS;

      if(NUM_STACKERS > 3) {
        stackers[3].startPixel = (NUM_LEDS/2 - 1 + rotationIndex + NUM_LEDS) % NUM_LEDS;
        stackers[3].endPixel = (NUM_LEDS/4 + spacing/2 + rotationIndex + NUM_LEDS) % NUM_LEDS;
      }
    }
  }
    
  for(uint8_t i = 0; i < NUM_STACKERS; i++) {
    stackers[i].numStacks = 0;
    stackers[i].numColors = numColors;
    stackers[i].stackSize = stackSize;
    stackers[i].spacing = spacing;
    stackers[i].moveIndex = stackers[i].startPixel;
    stackers[i].moveForward = i % 2 == 0;
    stackers[i].lastMove = curTime;
  }

  // return clearMode
  return scaleParam(baseParams.displayMode, 0, 3);
}
bool DrawStacker(struct_stacker* s, uint32_t curTime) {
  uint16_t curPixel = s->endPixel;
  for(uint8_t i = 0; i < s->numStacks; i++) {
    // Draw stack
    for(uint8_t j = 0; j < s->stackSize; j++) {
      leds[curPixel] = pm.palette[i % s->numColors];
      if(curPixel == s->startPixel) { break; }
      if(s->moveForward) {
        curPixel--;
        if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
      }
      else {
        curPixel++;
        if(curPixel == NUM_LEDS) { curPixel = 0; }
      }
    }

    if(curPixel != s->startPixel) {
      // Draw spacing
      for(uint8_t j = 0; j < s->spacing; j++) {
        leds[curPixel] = CRGB::Black; //todo: superfluous line since we clear and redraw every cycle
        if(curPixel == s->startPixel) { break; }
        if(s->moveForward) {
          curPixel--;
          if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
        }
        else {
          curPixel++;
          if(curPixel == NUM_LEDS) { curPixel = 0; }
        }
      }
    }
  }

  if(curPixel == s->startPixel) { return true; }

  if(s->moveIndex == curPixel) {
    // End of movement for this stack; rotation moved into end position
    s->numStacks++;
    s->moveIndex = s->startPixel;
  }
  
  // Move
  while(curTime - s->lastMove >= FPS_TO_TIME(s->moveSpeed)) {
    s->lastMove += FPS_TO_TIME(s->moveSpeed);
    if(s->moveForward) {
      s->moveIndex++;
      if(s->moveIndex == NUM_LEDS) { s->moveIndex = 0; }
    }
    else {
      s->moveIndex--;
      if(s->moveIndex == 0xFFFF) { s->moveIndex = NUM_LEDS-1; }
    }
  }
  
  // Draw moving stack
  for(uint8_t i = 0; i < s->stackSize; i++) {
    uint16_t idx = s->moveForward ? (s->moveIndex - i + NUM_LEDS) % NUM_LEDS : (s->moveIndex + i) % NUM_LEDS;
    leds[idx] = pm.palette[s->numStacks % s->numColors];
    if(idx == s->startPixel) { break; }
  }

  if(s->moveIndex == curPixel) {
    // End of movement for this stack
    s->numStacks++;
    s->moveIndex = s->startPixel;
  }

  return false;
}
bool ClearStackers(uint8_t clearMode, uint32_t curTime) {
  static uint16_t clearMoveIndex = 0;
  
  if(clearMode == 0) { return true; }
  else if(clearMode == 1) {
    // Clear from endPos
    for(uint8_t i = 0; i < NUM_STACKERS; i++) {
      uint16_t curPixel = stackers[i].endPixel;
      if(stackers[i].moveForward) {
        for(uint8_t j = 0; j < clearMoveIndex; j++) {
          leds[curPixel--] = CRGB::Black;
          if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
        }
      }
      else {
        for(uint8_t j = 0; j < clearMoveIndex; j++) {
          leds[curPixel++] = CRGB::Black;
          if(curPixel == NUM_LEDS) { curPixel = 0; }
        }
      }
    }

    while(curTime - stackers[0].lastMove >= FPS_TO_TIME(stackers[0].moveSpeed)) {
      for(uint8_t i = 0; i < NUM_STACKERS; i++) { stackers[i].lastMove += FPS_TO_TIME(stackers[i].moveSpeed); }
      clearMoveIndex++;
      if(clearMoveIndex >= NUM_LEDS / NUM_STACKERS) { clearMoveIndex = 0; return true; }
    }
  }
  else if(clearMode == 2) {
    // Clear from startPos
    for(uint8_t i = 0; i < NUM_STACKERS; i++) {
      uint16_t curPixel = stackers[i].startPixel;
      if(stackers[i].moveForward) {
        for(uint8_t j = 0; j < clearMoveIndex; j++) {
          leds[curPixel++] = CRGB::Black;
          if(curPixel == NUM_LEDS) { curPixel = 0; }
        }
      }
      else {
        for(uint8_t j = 0; j < clearMoveIndex; j++) {
          leds[curPixel--] = CRGB::Black;
          if(curPixel == 0) { curPixel = NUM_LEDS-1; }
        }
      }
    }

    while(curTime - stackers[0].lastMove >= FPS_TO_TIME(stackers[0].moveSpeed)) {
      for(uint8_t i = 0; i < NUM_STACKERS; i++) { stackers[i].lastMove += FPS_TO_TIME(stackers[i].moveSpeed); }
      clearMoveIndex++;
      if(clearMoveIndex >= NUM_LEDS / NUM_STACKERS) { clearMoveIndex = 0; return true; }
    }
  }
  else if(clearMode == 3) {
    // Overwrite cleared stacks and draw moving black stack
    static uint8_t numClearedStacks = 0;
    
    for(uint8_t i = 0; i < NUM_STACKERS; i++) {
      uint16_t curPixel = stackers[i].endPixel;
      uint16_t limit = numClearedStacks * (stackers[i].stackSize + stackers[i].spacing);
      
      for(uint8_t j = 0; j < limit; j++) {
        if(stackers[i].moveForward) {
          leds[curPixel--] = CRGB::Black;
          if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
        }
        else {
          leds[curPixel++] = CRGB::Black;
          if(curPixel == NUM_LEDS) { curPixel = 0; }
        }
      }
  
      for(uint8_t j = 0; j < stackers[i].stackSize; j++) {
        uint16_t idx = stackers[i].moveForward ? (stackers[i].startPixel + clearMoveIndex - j + NUM_LEDS) % NUM_LEDS : (stackers[i].startPixel - clearMoveIndex + j + NUM_LEDS) % NUM_LEDS;
        leds[idx] = CRGB::Black;
        if(idx == stackers[i].startPixel) { break; }
      }
  
      // Move and check for end of clear area
      while((curTime - stackers[i].lastMove) >= FPS_TO_TIME(stackers[i].moveSpeed)) {
        stackers[i].lastMove += FPS_TO_TIME(stackers[i].moveSpeed);
        if(i == NUM_STACKERS-1) { clearMoveIndex++; }
        uint16_t limit = NUM_LEDS / NUM_STACKERS - stackers[i].spacing/2 - numClearedStacks*(stackers[i].stackSize + stackers[i].spacing);
        if(clearMoveIndex == limit) {
          if(i == NUM_STACKERS-1) {
            numClearedStacks++;
            clearMoveIndex = 0;
            if(numClearedStacks == stackers[i].numStacks) { numClearedStacks = 0; return true; }
          }
        }
      }
    }
  }

  return false;
}

/*
void Stacks(uint32_t curTime) {
  const uint8_t numSegments = 4;
  // Bands of color flow from spawn point to end point and stack up
  // numColors, dimSpeed=moveSpeed, colorSpeed=rotationSpeed, brightLength=bandThickness, transLength=spacing, displayMode=clearMode
  // dimPeriod, colorPeriod; pauseBetweenBands
  
  uint8_t nCol = scaleParam(baseParams.numColors, 1, 4);
  uint8_t stackSize = scaleParam(baseParams.brightLength, 5, 15);
  uint8_t spacing = scaleParam(baseParams.transLength, 0, 4);
  spacing += spacing % 2; // Keep spacing even
  uint8_t moveSpeed = scaleParam((uint8_t)2*abs(baseParams.dimSpeed), uint8_t(20), uint8_t(REFRESH_RATE));
  int8_t rotationSpeed = scaleParam(baseParams.colorSpeed, int8_t(-REFRESH_RATE), int8_t(REFRESH_RATE));
  uint8_t clearMode = scaleParam(baseParams.displayMode, 0, 3);

  // adjust moveSpeed by rotationSpeed to avoid flicker
  int16_t moveSpeedF = rotationSpeed + moveSpeed;
  int16_t moveSpeedR = abs(rotationSpeed - moveSpeed);
  
  static uint8_t stackColors[20]; // max stacks
  static uint8_t drawPhase = 0;
  static uint8_t numStacks = 0;
  static uint16_t movingIndexF = 0;
  static uint16_t movingIndexR = 0;
  static uint16_t clearMoveIndex = 0;
  static uint8_t numClearedStacks = 0;
  static uint16_t rotationIndex = NUM_LEDS-18;
  static uint16_t rotationOffset = 0;
  static uint32_t lastMoveTimeF = 0;
  static uint32_t lastMoveTimeR = 0;
  static uint32_t lastRotationTime = 0;
  static uint8_t curStackSize = stackSize;
  static uint8_t curSpacing = spacing;


  // Clear display each cycle
  for(uint16_t i = 0; i < NUM_LEDS; i++) { leds[i] = CRGB::Black; leds_b[i] = 180; }

  // Draw each segment
  for(uint8_t segment = 0; segment < numSegments; segment++) {
    bool moveForward;
    uint16_t startPos, endPos;
    if(segment == 0) { startPos = 0; endPos = NUM_LEDS/4 - 1 - curSpacing/2; moveForward = true; }
    else if(segment == 1) { startPos = NUM_LEDS-1; endPos = NUM_LEDS*3/4+curSpacing/2; moveForward = false; }
    else if(segment == 2) { startPos = NUM_LEDS/2; endPos = NUM_LEDS*3/4-1-curSpacing/2; moveForward = true; }
    else { startPos = NUM_LEDS/2-1; endPos = NUM_LEDS/4+curSpacing/2; moveForward = false; }

    startPos = (startPos + rotationIndex) % NUM_LEDS;
    endPos = (endPos + rotationIndex) % NUM_LEDS;
    
    // Draw stacks
    uint16_t curPixel = endPos;
    if(moveForward) {
      for(uint8_t i = 0; i < numStacks; i++) {
        for(uint8_t j = 0; j < curStackSize; j++) {
          leds[curPixel--] = pm.palette[stackColors[i]];
          if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
        }
        for(uint8_t j = 0; j < curSpacing; j++) {
          leds[curPixel--] = CRGB::Black; //todo: superfluous line since we clear and redraw every cycle
          if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
        }
      }

      if(numSegments*numStacks*(curStackSize+curSpacing) < NUM_LEDS) {
        // Draw moving stack
        for(uint8_t i = 0; i < curStackSize; i++) {
          uint16_t idx = (startPos + movingIndexF - i + rotationOffset + NUM_LEDS) % NUM_LEDS;
          leds[idx] = pm.palette[stackColors[numStacks]];
          if(idx == startPos) { break; }
        }
      }
    }
    else {
      for(uint8_t i = 0; i < numStacks; i++) {
        for(uint8_t j = 0; j < curStackSize; j++) {
          leds[curPixel++] = pm.palette[stackColors[i]];
          if(curPixel == NUM_LEDS) { curPixel = 0; }
        }
        for(uint8_t j = 0; j < curSpacing; j++) {
          leds[curPixel++] = CRGB::Black; //todo: superfluous line since we clear and redraw every cycle
          if(curPixel == NUM_LEDS) { curPixel = 0; }
        }
      }

      if(numSegments*numStacks*(curStackSize+curSpacing) < NUM_LEDS) {
        // Draw moving stack
        for(uint8_t i = 0; i < curStackSize; i++) {
          uint16_t idx = (startPos - movingIndexR + i + rotationOffset + NUM_LEDS) % NUM_LEDS;
          leds[idx] = pm.palette[stackColors[numStacks]];
          if(idx == startPos) { break; }
        }
      }
    }

    if(drawPhase == 0) {
      // Move new stack and check for move completion
      if(moveForward) {
        while((curTime - lastMoveTimeF) >= FPS_TO_TIME(moveSpeedF)) {
          if(lastMoveTimeF == 0) { lastMoveTimeF = curTime; } else { lastMoveTimeF += FPS_TO_TIME(moveSpeedF); }
          movingIndexF++;
        }
      }
      else {
        while((curTime - lastMoveTimeR) >= FPS_TO_TIME(moveSpeedR)) {
          if(lastMoveTimeR == 0) { lastMoveTimeR = curTime; } else { lastMoveTimeR += FPS_TO_TIME(moveSpeedR); }
          movingIndexR++;
        }
      }

      //if(segment == 0) PRINTLN((startPos + movingIndexF + rotationOffset + NUM_LEDS) % NUM_LEDS) + " / " + endPos);
      if(segment == 3 && (startPos - movingIndexR + rotationOffset + NUM_LEDS) % NUM_LEDS == (endPos + numStacks*(curStackSize+curSpacing) + NUM_LEDS) % NUM_LEDS) {
        if(numSegments*(numStacks+1)*(curStackSize+curSpacing) < NUM_LEDS) {
          numStacks++;
          movingIndexF = 0;
          movingIndexR = 0;
          rotationOffset = 0;
          stackColors[numStacks] = numStacks % nCol;
        }          
        else {
          // End of building phase
          if(clearMode == 0) {
            curStackSize = stackSize;
            curSpacing = spacing;
            numStacks = 0;
            movingIndexF = 0;
            movingIndexR = 0;
            rotationOffset = 0;
          }
          else {
            drawPhase = clearMode;
            clearMoveIndex = 0;
            rotationOffset = 0;
            movingIndexF--; // Undo moving stack movement
            movingIndexR--; // Undo moving stack movement
          }
        }
      }
    }
    else if(drawPhase == 1 || drawPhase == 2) {
      if(drawPhase == 1) {
        // Clear from endPos
        uint16_t curPixel = endPos;
        if(moveForward) {
          for(uint8_t i = 0; i < clearMoveIndex; i++) {
            leds[curPixel--] = CRGB::Black;
            if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
          }
        }
        else {
          for(uint8_t i = 0; i < clearMoveIndex; i++) {
            leds[curPixel++] = CRGB::Black;
            if(curPixel == NUM_LEDS) { curPixel = 0; }
          }
        }
      }
      else if(drawPhase == 2) {
        // Clear from startPos
        uint16_t curPixel = startPos;
        if(moveForward) {
          for(uint8_t i = 0; i < clearMoveIndex; i++) {
            leds[curPixel++] = CRGB::Black;
            if(curPixel == NUM_LEDS) { curPixel = 0; }
          }
        }
        else {
          for(uint8_t i = 0; i < clearMoveIndex; i++) {
            leds[curPixel--] = CRGB::Black;
            if(curPixel == 0) { curPixel = NUM_LEDS-1; }
          }
        }
      }

      // Move and check for end of clear
      while((curTime - lastMoveTimeF) >= FPS_TO_TIME(moveSpeedF)) {
        lastMoveTimeF += FPS_TO_TIME(moveSpeedF);
        lastMoveTimeR += FPS_TO_TIME(moveSpeedF);
        clearMoveIndex++;
        if(clearMoveIndex >= NUM_LEDS/numSegments) {
          drawPhase = 0;
          numStacks = 0;
          movingIndexF = 0;
          movingIndexR = 0;
          clearMoveIndex = 0;
          numClearedStacks = 0;
          curStackSize = stackSize;
          curSpacing = spacing;
        }
      }
    }
    else if(clearMode == 3) {
      // Overwrite cleared stacks and draw moving black stack
      uint16_t curPixel = endPos;
      uint16_t limit = numClearedStacks * (curStackSize + curSpacing);
      if(moveForward) {
        for(uint8_t i = 0; i < limit; i++) {
          leds[curPixel--] = CRGB::Black;
          if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
        }

        for(uint8_t i = 0; i < curStackSize; i++) {
          uint16_t idx = (startPos + clearMoveIndex - i + NUM_LEDS) % NUM_LEDS;
          leds[idx] = CRGB::Black;
          if(idx == startPos) { break; }
        }
      }
      else {
        for(uint8_t i = 0; i < limit; i++) {
          leds[curPixel++] = CRGB::Black;
          if(curPixel == NUM_LEDS) { curPixel = 0; }
        }

        for(uint8_t i = 0; i < curStackSize; i++) {
          uint16_t idx = (startPos - clearMoveIndex + i + NUM_LEDS) % NUM_LEDS;
          leds[idx] = CRGB::Black;
          if(idx == startPos) { break; }
        }
      }

      // Move and check for end of clear area
      while((curTime - lastMoveTimeF) >= FPS_TO_TIME(moveSpeedF)) {
        lastMoveTimeF += FPS_TO_TIME(moveSpeedF);
        lastMoveTimeR += FPS_TO_TIME(moveSpeedF);
        clearMoveIndex++;
        uint16_t limit = NUM_LEDS/numSegments - curSpacing/2 - numClearedStacks*(curStackSize + curSpacing);
        if(clearMoveIndex == limit) {
          numClearedStacks++;
          clearMoveIndex = 0;
          
          if(numClearedStacks > numStacks) {
            drawPhase = 0;
            numStacks = 0;
            movingIndexF = 0;
            movingIndexR = 0;
            numClearedStacks = 0;
            rotationOffset = 0;
            curStackSize = stackSize;
            curSpacing = spacing;
          }
        }
      }
    }
  }

  // Rotate entire display
  if(rotationSpeed != 0) {
    if(curTime - lastRotationTime > FPS_TO_TIME(abs(rotationSpeed))) {
      if(lastRotationTime == 0) { lastRotationTime = curTime; } else { lastRotationTime += FPS_TO_TIME(abs(rotationSpeed)); }
      if(rotationSpeed > 0) {
        rotationIndex = (rotationIndex+1) % NUM_LEDS;
        rotationOffset = (rotationOffset - 1 + NUM_LEDS) % NUM_LEDS;
      }
      else {
        rotationIndex = (rotationIndex - 1 + NUM_LEDS) % NUM_LEDS;
        rotationOffset = (rotationOffset+1) % NUM_LEDS;
      }
    }
  }
}
*/
void Collision() {
  // Comets flow from spawn point to end point and explode in a flash of white
  // numColors, brightSpeed, colorThickness
}

void PulseInPlace() {
  
}

void ColorExplosion(uint32_t curTime) {
  static uint32_t lastSpawnTime = curTime;
  
//dimPeriod, colorPeriod / splitPoint
#ifdef EXPLICIT_PARAMETERS
  uint8_t nCol = baseParams.numColors;
  uint8_t growRate = baseParams.colorSpeed;
  uint8_t fadeRate = baseParams.dimSpeed;
  uint8_t spawnRate = baseParams.brightLength;
  uint8_t growChance = baseParams.transLength;
  uint8_t growPoint = baseParams.displayMode;
  uint8_t overridePoint = baseParams.dimPeriod; // Hardly matters
#else
  uint8_t nCol = scaleParam(baseParams.numColors, 2, 5);
  uint8_t growRate = scaleParam(2*(uint8_t)abs(baseParams.colorSpeed), (uint8_t)2, (uint8_t)16);
  growRate += growRate % 2;
  uint8_t fadeRate = scaleParam(2*(uint8_t)abs(baseParams.dimSpeed), (uint8_t)2, min(6, growRate+2));
  fadeRate += fadeRate % 2;
  uint8_t spawnRate = scaleParam(baseParams.brightLength, 6, 120);
  uint8_t growChance = scaleParam(baseParams.transLength, 220, 250);
  uint8_t growPoint = growRate * scaleParam(baseParams.displayMode, 4, 15);
  uint8_t overridePoint = scaleParam(baseParams.dimPeriod, 1, 32); // Hardly matters
#endif

  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    if(leds_b[i] % 2 == 1) {
      // Increasing brightness with odd numbers
      if(leds_b[i] >= 255 - growRate) { leds_b[i] = 254; }
      else { leds_b[i] += growRate; }
      //todo: move the 0 and NUM_LEDS checks to single cases outside the loop, to avoid checking every pixel
      if(leds_b[i] >= growPoint && leds_b[i] - growRate < growPoint) {
        // Spread outward
        #define SPREAD_EXPLODE(x,y) if(leds_b[x] % 2 == 0 && leds_b[x] <= overridePoint) { leds_b[x] = 1; leds[x] = leds[y]; }
        if(random8() < growChance) {
          // Spread up
          if(i == NUM_LEDS-1) { SPREAD_EXPLODE(0, NUM_LEDS-1) }
          else { SPREAD_EXPLODE(i+1, i) }
        }
        if(random8() < growChance) {
          // Spread down
          if(i == 0) { SPREAD_EXPLODE(NUM_LEDS-1, 0) }
          else { SPREAD_EXPLODE(i-1, i) }
        }
      }
    }
    else if(leds_b[i] != 0){
      // Fade down even brightnesses
      if(leds_b[i] <= fadeRate) { leds_b[i] = 0; }
      else { leds_b[i] -= fadeRate; }
    }
  }

  // Randomly light spawnRate LEDs
  uint8_t nSpawn = (curTime - lastSpawnTime) * spawnRate / ONE_SEC;
  if(nSpawn > 0) {
    lastSpawnTime = curTime;
    for(uint8_t i = 0; i < nSpawn; i++) {
      uint16_t newCenter = random16(NUM_LEDS);
      if(leds_b[newCenter] == 0) {
        leds_b[newCenter] = 1;
        leds[newCenter] = pm.palette[random8(nCol)];
      }
    }
  }
}

void MovingStrobe() {
  
}

void Shutters() {
  
}

void StutterStepBands() {
  
}

void ColorCycle() {
  
}

void HotGlow() {
  
}
