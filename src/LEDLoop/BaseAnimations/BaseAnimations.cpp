#include "LEDLoop/LEDLoop.h"
#include "Definitions.h"

// Base animations

// Under development

void LEDLoop::ScrollingSaturation(uint8_t nWhite, uint8_t nTrans, uint8_t nPure) {
  const uint8_t maxBlendAmount = 160;
  //const uint8_t whiteBrightness = 210;

  uint8_t period = nWhite + nPure + 2*nTrans;
  uint16_t extendedLEDCount = ((numLEDs-1)/period+1)*period;
  //CRGB myWhite = CRGB(whiteBrightness, whiteBrightness, whiteBrightness);
  uint16_t i = 0;
  while(i < extendedLEDCount) {
    // progressively blend the LEDs toward white
    for(uint16_t j = 0; j < period - nPure; j++) {
      int16_t offset = 0;//baseCount*THROTTLE_MID_POINT/baseSpeed2;
      uint16_t idx = (i + offset) % extendedLEDCount;
      if(i++ >= extendedLEDCount) { return; }
      if(idx >= numLEDs) continue;


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


void LEDLoop::Orbs() {
  const uint8_t centerPoint = 51;
  const uint8_t minRadius = 3;
  //const uint8_t maxRadius = 51;
  const uint8_t fadeRate = 64;
    
  uint8_t curRadius;
  CRGB colorToUse;
  
  int16_t radiusOffset = 0;//round((maxRadius-minRadius) * (sin8(baseCount % 255) - 128.0) / 128.0);
  if(radiusOffset < 0) {
    hsv2rgb_rainbow(pm->palette[2], colorToUse);
    curRadius = minRadius - radiusOffset;
  }
  else {
    hsv2rgb_rainbow(pm->palette[3], colorToUse);
    curRadius = minRadius + radiusOffset;
  }
  
  // Draw
  leds[centerPoint] = colorToUse;

  for(uint16_t i = 1; i <= curRadius; i++) {
    if(centerPoint >= i)
      leds[centerPoint - i] = colorToUse;
    if(centerPoint + i < numLEDs)
      leds[centerPoint + i] = colorToUse;
  }

  for(CRGB & pixel : leds) pixel.fadeToBlackBy(fadeRate);
}

void LEDLoop::ScrollingGlimmerBands() {
  const uint8_t glimmerPortion = 4;
  const uint8_t glimmerFloor = 180;
  CRGB colorPattern[baseParams.dimPeriod];
  uint8_t spacing = baseParams.dimPeriod - baseParams.numColors * baseParams.brightLength;

  uint16_t curPixel = 0;
  for(uint8_t i = 0; i < baseParams.numColors; i++) {
    for(uint8_t j = 0; j < baseParams.brightLength; j++) {
      hsv2rgb_rainbow(pm->palette[2 + i], colorPattern[curPixel]);
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

void LEDLoop::CenterSpawn() {
  // Propogate out
  for(uint16_t i = 0; i < numLEDs/2; i++) {
    leds[i] = leds[i+1];
    leds[numLEDs-1 - i] = leds[numLEDs-2 - i];
  }

  uint8_t firstCol = 0;//(baseCount / 128) % 6;
  uint8_t secondCol = (firstCol + 1) % 6;
  uint8_t blendAmount = 0;//2*(baseCount % 128);

  
  leds[numLEDs/2 - 1] = HSV2RGB(nblend(pm->palette[firstCol], pm->palette[secondCol], blendAmount), leds_b[numLEDs/2 - 1]);
  leds[numLEDs/2] = leds[numLEDs/2 - 1];
  leds_b[numLEDs/2] = leds_b[numLEDs/2 - 1];
}

/*
void LEDLoop::Stacks() {
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

  // adjust moveSpeed by rotationSpeed to avoid LEDLoop::flicker
  int16_t moveSpeedF = rotationSpeed + moveSpeed;
  int16_t moveSpeedR = abs(rotationSpeed - moveSpeed);
  
  static uint8_t stackColors[20]; // max stacks
  static uint8_t drawPhase = 0;
  static uint8_t numStacks = 0;
  static uint16_t movingIndexF = 0;
  static uint16_t movingIndexR = 0;
  static uint16_t clearMoveIndex = 0;
  static uint8_t numClearedStacks = 0;
  static uint16_t rotationIndex = numLEDs-18;
  static uint16_t rotationOffset = 0;
  static uint32_t lastMoveTimeF = 0;
  static uint32_t lastMoveTimeR = 0;
  static uint32_t lastRotationTime = 0;
  static uint8_t curStackSize = stackSize;
  static uint8_t curSpacing = spacing;


  // Clear display each cycle
  for(uint16_t i = 0; i < numLEDs; i++) { leds[i] = CRGB::Black; leds_b[i] = 180; }

  // Draw each segment
  for(uint8_t segment = 0; segment < numSegments; segment++) {
    bool moveForward;
    uint16_t startPos, endPos;
    if(segment == 0) { startPos = 0; endPos = numLEDs/4 - 1 - curSpacing/2; moveForward = true; }
    else if(segment == 1) { startPos = numLEDs-1; endPos = numLEDs*3/4+curSpacing/2; moveForward = false; }
    else if(segment == 2) { startPos = numLEDs/2; endPos = numLEDs*3/4-1-curSpacing/2; moveForward = true; }
    else { startPos = numLEDs/2-1; endPos = numLEDs/4+curSpacing/2; moveForward = false; }

    startPos = (startPos + rotationIndex) % numLEDs;
    endPos = (endPos + rotationIndex) % numLEDs;
    
    // Draw stacks
    uint16_t curPixel = endPos;
    if(moveForward) {
      for(uint8_t i = 0; i < numStacks; i++) {
        for(uint8_t j = 0; j < curStackSize; j++) {
          leds[curPixel--] = pm->palette[stackColors[i]];
          if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
        }
        for(uint8_t j = 0; j < curSpacing; j++) {
          leds[curPixel--] = CRGB::Black; //todo: superfluous line since we clear and redraw every cycle
          if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
        }
      }

      if(numSegments*numStacks*(curStackSize+curSpacing) < numLEDs) {
        // Draw moving stack
        for(uint8_t i = 0; i < curStackSize; i++) {
          uint16_t idx = (startPos + movingIndexF - i + rotationOffset + numLEDs) % numLEDs;
          leds[idx] = pm->palette[stackColors[numStacks]];
          if(idx == startPos) { break; }
        }
      }
    }
    else {
      for(uint8_t i = 0; i < numStacks; i++) {
        for(uint8_t j = 0; j < curStackSize; j++) {
          leds[curPixel++] = pm->palette[stackColors[i]];
          if(curPixel == numLEDs) { curPixel = 0; }
        }
        for(uint8_t j = 0; j < curSpacing; j++) {
          leds[curPixel++] = CRGB::Black; //todo: superfluous line since we clear and redraw every cycle
          if(curPixel == numLEDs) { curPixel = 0; }
        }
      }

      if(numSegments*numStacks*(curStackSize+curSpacing) < numLEDs) {
        // Draw moving stack
        for(uint8_t i = 0; i < curStackSize; i++) {
          uint16_t idx = (startPos - movingIndexR + i + rotationOffset + numLEDs) % numLEDs;
          leds[idx] = pm->palette[stackColors[numStacks]];
          if(idx == startPos) { break; }
        }
      }
    }

    if(drawPhase == 0) {
      // Move new stack and check for move completion
      if(moveForward) {
        while((timing.now - lastMoveTimeF) >= FPS_TO_TIME(moveSpeedF)) {
          if(lastMoveTimeF == 0) { lastMoveTimeF = timing.now; } else { lastMoveTimeF += FPS_TO_TIME(moveSpeedF); }
          movingIndexF++;
        }
      }
      else {
        while((timing.now - lastMoveTimeR) >= FPS_TO_TIME(moveSpeedR)) {
          if(lastMoveTimeR == 0) { lastMoveTimeR = timing.now; } else { lastMoveTimeR += FPS_TO_TIME(moveSpeedR); }
          movingIndexR++;
        }
      }

      //if(segment == 0) PRINTLN((startPos + movingIndexF + rotationOffset + numLEDs) % numLEDs) + " / " + endPos);
      if(segment == 3 && (startPos - movingIndexR + rotationOffset + numLEDs) % numLEDs == (endPos + numStacks*(curStackSize+curSpacing) + numLEDs) % numLEDs) {
        if(numSegments*(numStacks+1)*(curStackSize+curSpacing) < numLEDs) {
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
            if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
          }
        }
        else {
          for(uint8_t i = 0; i < clearMoveIndex; i++) {
            leds[curPixel++] = CRGB::Black;
            if(curPixel == numLEDs) { curPixel = 0; }
          }
        }
      }
      else if(drawPhase == 2) {
        // Clear from startPos
        uint16_t curPixel = startPos;
        if(moveForward) {
          for(uint8_t i = 0; i < clearMoveIndex; i++) {
            leds[curPixel++] = CRGB::Black;
            if(curPixel == numLEDs) { curPixel = 0; }
          }
        }
        else {
          for(uint8_t i = 0; i < clearMoveIndex; i++) {
            leds[curPixel--] = CRGB::Black;
            if(curPixel == 0) { curPixel = numLEDs-1; }
          }
        }
      }

      // Move and check for end of clear
      while((timing.now - lastMoveTimeF) >= FPS_TO_TIME(moveSpeedF)) {
        lastMoveTimeF += FPS_TO_TIME(moveSpeedF);
        lastMoveTimeR += FPS_TO_TIME(moveSpeedF);
        clearMoveIndex++;
        if(clearMoveIndex >= numLEDs/numSegments) {
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
          if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
        }

        for(uint8_t i = 0; i < curStackSize; i++) {
          uint16_t idx = (startPos + clearMoveIndex - i + numLEDs) % numLEDs;
          leds[idx] = CRGB::Black;
          if(idx == startPos) { break; }
        }
      }
      else {
        for(uint8_t i = 0; i < limit; i++) {
          leds[curPixel++] = CRGB::Black;
          if(curPixel == numLEDs) { curPixel = 0; }
        }

        for(uint8_t i = 0; i < curStackSize; i++) {
          uint16_t idx = (startPos - clearMoveIndex + i + numLEDs) % numLEDs;
          leds[idx] = CRGB::Black;
          if(idx == startPos) { break; }
        }
      }

      // Move and check for end of clear area
      while((timing.now - lastMoveTimeF) >= FPS_TO_TIME(moveSpeedF)) {
        lastMoveTimeF += FPS_TO_TIME(moveSpeedF);
        lastMoveTimeR += FPS_TO_TIME(moveSpeedF);
        clearMoveIndex++;
        uint16_t limit = numLEDs/numSegments - curSpacing/2 - numClearedStacks*(curStackSize + curSpacing);
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
    if(timing.now - lastRotationTime > FPS_TO_TIME(abs(rotationSpeed))) {
      if(lastRotationTime == 0) { lastRotationTime = timing.now; } else { lastRotationTime += FPS_TO_TIME(abs(rotationSpeed)); }
      if(rotationSpeed > 0) {
        rotationIndex = (rotationIndex+1) % numLEDs;
        rotationOffset = (rotationOffset - 1 + numLEDs) % numLEDs;
      }
      else {
        rotationIndex = (rotationIndex - 1 + numLEDs) % numLEDs;
        rotationOffset = (rotationOffset+1) % numLEDs;
      }
    }
  }
}
*/
void LEDLoop::Collision() {
  // Comets flow from spawn point to end point and explode in a flash of white
  // numColors, dimSpeed, colorThickness
}

void LEDLoop::PulseInPlace() {
  
}

void LEDLoop::ColorExplosion() {
  static uint32_t lastSpawnTime = timing.now;
  
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
  uint8_t spawnRate = 0;//scaleParam(baseParams.brightLength, 6, 120);
  uint8_t growChance = 0;//scaleParam(baseParams.transLength, 220, 250);
  uint8_t growPoint = growRate * scaleParam(baseParams.displayMode, 4, 15);
  uint8_t overridePoint = scaleParam(baseParams.dimPeriod, 1, 32); // Hardly matters
#endif

  for(uint16_t i = 0; i < numLEDs; i++) {
    if(leds_b[i] % 2 == 1) {
      // Increasing brightness with odd numbers
      if(leds_b[i] >= 255 - growRate) { leds_b[i] = 254; }
      else { leds_b[i] += growRate; }
      //todo: move the 0 and numLEDs checks to single cases outside the loop, to avoid LEDLoop::checking every pixel
      if(leds_b[i] >= growPoint && leds_b[i] - growRate < growPoint) {
        // Spread outward
        #define SPREAD_EXPLODE(x,y) if(leds_b[x] % 2 == 0 && leds_b[x] <= overridePoint) { leds_b[x] = 1; leds[x] = leds[y]; }
        if(random8() < growChance) {
          // Spread up
          if(i == numLEDs-1) { SPREAD_EXPLODE(0, numLEDs-1) }
          else { SPREAD_EXPLODE(i+1, i) }
        }
        if(random8() < growChance) {
          // Spread down
          if(i == 0) { SPREAD_EXPLODE(numLEDs-1, 0) }
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
  uint8_t nSpawn = (timing.now - lastSpawnTime) * spawnRate / ONE_SEC;
  if(nSpawn > 0) {
    lastSpawnTime = timing.now;
    for(uint8_t i = 0; i < nSpawn; i++) {
      uint16_t newCenter = random16(numLEDs);
      if(leds_b[newCenter] == 0) {
        leds_b[newCenter] = 1;
        leds[newCenter] = pm->palette[random8(nCol)];
      }
    }
  }
}

void LEDLoop::MovingStrobe() {
  
}


void LEDLoop::ColorCycle() {
  
}

void LEDLoop::HotGlow() {
  
}
