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

void Stacks(uint32_t curTime) {
  const uint8_t numStackers = 4;
  // Bands of color flow from spawn point to end point and stack up
  // numColors, dimSpeed=moveSpeed, colorSpeed=pauseBetweenBands, brightLength=bandThickness, transLength=spacing
  // displayMode, dimPeriod, colorPeriod
  uint8_t nCol = scaleParam(baseParams.numColors, 1, 4);
  uint8_t stackSize = scaleParam(baseParams.brightLength, 5, 15);
  uint8_t mySpeed = scaleParam((uint8_t)2*abs(baseParams.dimSpeed), (uint8_t)20, uint8_t(REFRESH_RATE));
  uint8_t rotationSpeed = 0;//scaleParam((uint8_t)2*abs(baseParams.colorSpeed), (uint8_t)20, uint8_t(REFRESH_RATE));
  
  static uint8_t stackColors[20]; // max stacks
  static uint8_t numStacks = 0;
  static uint16_t movingStackIndex = 0;
  static uint16_t rotationIndex = NUM_LEDS-18;
  static uint32_t lastMoveTime = 0;
  static uint32_t lastRotationTime = 0;
  static uint8_t curStackSize = stackSize;

  for(uint16_t i = 0; i < NUM_LEDS; i++) { leds[i] = CRGB::Black; leds_b[i] = 255; }

  if(numStacks == NUM_LEDS / (numStackers*curStackSize)) {
    // Reset
    curStackSize = stackSize;
    numStacks = 0;
  }

  for(uint8_t segment = 0; segment < numStackers; segment++) {
    bool moveForward;
    uint16_t startPos, endPos;
    if(segment == 0) { startPos = 0; endPos = NUM_LEDS/4 - 1; moveForward = true; }
    else if(segment == 1) { startPos = NUM_LEDS-1; endPos = NUM_LEDS*3/4; moveForward = false; }
    else if(segment == 2) { startPos = NUM_LEDS/2; endPos = NUM_LEDS*3/4-1; moveForward = true; }
    else { startPos = NUM_LEDS/2-1; endPos = NUM_LEDS/4; moveForward = false; }

    startPos = (startPos + rotationIndex) % NUM_LEDS;
    endPos = (endPos + rotationIndex) % NUM_LEDS;
    
    // Draw stationary stacks
    uint16_t curPixel = endPos;
    if(moveForward) {
      for(uint8_t i = 0; i < numStacks; i++) {
        for(uint8_t j = 0; j < curStackSize; j++) {
          leds[curPixel--] = pm.palette[stackColors[i]];
          if(curPixel == 0xFFFF) { curPixel = NUM_LEDS-1; }
        }
      }
    }
    else {
      for(uint8_t i = 0; i < numStacks; i++) {
        for(uint8_t j = 0; j < curStackSize; j++) {
          leds[curPixel++] = pm.palette[stackColors[i]];
          if(curPixel == NUM_LEDS) { curPixel = 0; }
        }
      }
    }
  
    // Draw moving stack
    if(moveForward) {
      for(uint8_t i = 0; i < curStackSize; i++) {
        uint16_t idx = (startPos + movingStackIndex - i + NUM_LEDS) % NUM_LEDS;
        leds[idx] = pm.palette[stackColors[numStacks]];
        if(idx == startPos) { break; }
      }
    }
    else {
      for(uint8_t i = 0; i < curStackSize; i++) {
        uint16_t idx = (startPos - movingStackIndex + i + NUM_LEDS) % NUM_LEDS;
        leds[idx] = pm.palette[stackColors[numStacks]];
        if(idx == startPos) { break; }
      }
    }
  }

  // Move new stack and check for move completion
  if((curTime - lastMoveTime) > FPS_TO_TIME(mySpeed)) {
    lastMoveTime = curTime;
    movingStackIndex++;
    if(movingStackIndex == NUM_LEDS/numStackers - numStacks*curStackSize) {
      numStacks++;
      movingStackIndex = 0;
      stackColors[numStacks] = numStacks % nCol;
    }
  }

  // Rotate entire display
  if(abs(rotationSpeed) > 0 && (curTime - lastRotationTime) > FPS_TO_TIME(abs(rotationSpeed))) {
    lastRotationTime = curTime;
    if(rotationSpeed > 0) { rotationIndex = (rotationIndex+1) % NUM_LEDS; }
    else { rotationIndex = (rotationIndex+NUM_LEDS-1) % NUM_LEDS; }
  }
}

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
      // debug: move the 0 and NUM_LEDS checks to single cases outside the loop, to avoid checking every pixel
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

void ExpandingStackingDark() {
  
}

void StutterStepBands() {
  
}

void ColorCycle() {
  
}

void HotGlow() {
  
}


