#include "PatternScroller.h"

PatternScroller::PatternScroller() {
  dimParamChangeType = IMMEDIATE;//ONCE_PER_MOVE;
  colorParamChangeType = IMMEDIATE;
  
  targetColorPatternIndex = 0;
  targetDimPatternIndex = 0;
  oldDimPatternIndex = 0;
  oldColorPatternIndex = 0;
  randomDimPatternIndex = 0;
  randomColorPatternIndex = 0;
}

void PatternScroller::Init(PaletteManager* pm, GammaManager* gm, uint16_t _numLEDs, uint8_t dimPatternIndex, uint8_t colorPatternIndex, struct_base_show_params& params, uint32_t curTime) {
  pr.Init(pm, gm);

  numLEDs = _numLEDs;
  dimSpeed = params.dimSpeed;
  colorSpeed = params.colorSpeed;
  
  dimPeriod = pg.dimPeriod = params.dimPeriod;
  colorPeriod = pg.colorPeriod = params.colorPeriod;
  numColors = pg.numColors = params.numColors;
  brightLength = pg.brightLength = params.brightLength;
  
  transLength = pg.transLength = params.transLength;
  SetDisplayMode(dimPatternIndex, colorPatternIndex, curTime);

  pg.WriteDimPattern(dimPatternIndex, targetDimPattern);
  pr.SetDimPattern(targetDimPattern, dimPeriod);
  pg.WriteColorPattern(colorPatternIndex, targetColorPattern);
  pr.SetColorPattern(targetColorPattern, colorPeriod);

  //lastDimParamChange = curTime;
  //lastColorParamChange = curTime;
  lastDimPatternChange = curTime;
  lastColorPatternChange = curTime;
  lastDimMove = curTime;
  lastColorMove = curTime;
}

bool PatternScroller::Update(uint32_t curTime) {
// Returns true if dim pattern moved
  if(WalkColorParams(curTime)) {
    if(oldColorPatternIndex != 0xFF) { pg.WriteColorPattern(oldColorPatternIndex, oldColorPattern); }
    pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);
    BlendColorPattern(curTime);
  }

  if(WalkDimParams(curTime)) {
    if(oldDimPatternIndex != 0xFF) { pg.WriteDimPattern(oldDimPatternIndex, oldDimPattern); }
    pg.WriteDimPattern(targetDimPatternIndex, targetDimPattern);
    BlendDimPattern(curTime);
  }

  if(curTime - lastDimPatternChange >= dimPauseLength) {
    BlendDimPattern(curTime);
  }
  
  if(curTime - lastColorPatternChange >= colorPauseLength) {
    BlendColorPattern(curTime);
  }
  
  pr.SetColorPattern(curColorPattern, colorPeriod);
  pr.SetDimPattern(curDimPattern, dimPeriod);

  return ScrollPatterns(curTime);
}

void PatternScroller::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastDimMove += amount;
  //lastDimParamChange += amount;
  //lastColorParamChange += amount;
  lastDimPatternChange += amount;
  lastColorPatternChange += amount;
}

void PatternScroller::SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs) {
  pr.SetCRGBs(target, target_b, numLEDs);
}

void PatternScroller::SetDisplayMode(uint8_t dimPatternIndex, uint8_t colorPatternIndex, uint32_t curTime) {
  if(targetColorPatternIndex != colorPatternIndex) {
    // New dim pattern target; this was not the cause of a blend completing.  Mark oldIndex as 0xFF to signal this.
    memcpy(oldColorPattern, curColorPattern, colorPeriod);
    oldColorPatternIndex = 0xFF;// targetColorPatternIndex;
    targetColorPatternIndex = colorPatternIndex;
    pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);

    lastColorPatternChange = curTime;
  }

  if(targetDimPatternIndex != dimPatternIndex) {
    // New dim pattern target; this was not the cause of a blend completing.  Mark oldIndex as 0xFF to signal this.
    uint8_t lastDimPattern = oldDimPatternIndex;
    oldDimPatternIndex = 0xFF;// targetDimPatternIndex;

    memcpy(oldDimPattern, curDimPattern, dimPeriod);
    targetDimPatternIndex = dimPatternIndex;

    if(dimPatternIndex == NUM_DIM_PATTERNS-1) {
      do { randomDimPatternIndex = random8(NUM_DIM_PATTERNS-1); } while(randomDimPatternIndex == lastDimPattern);
      pg.WriteDimPattern(randomDimPatternIndex, targetDimPattern);
    }
    else {
      pg.WriteDimPattern(targetDimPatternIndex, targetDimPattern);
    }

    lastDimPatternChange = curTime;
  }
}


bool PatternScroller::WalkColorParams(uint32_t curTime) {
  bool updateMade = false;

  if(colorParamChangeType == IMMEDIATE) {
    // Instantly update params
    if(pg.numColors != numColors) {
      updateMade = true;
      pg.numColors = numColors;
    }
  }
  else if(IsReadyForColorMove(curTime)) {
    if(colorParamChangeType == ONCE_PER_MOVE || IsStartOfColorPattern()) {
      // Gradually update params in sync with movement
      if(pg.numColors < numColors) {
        pg.numColors++;
        updateMade = true;
      }
      else if(pg.numColors > numColors) {
        pg.numColors--;
        updateMade = true;
      }
    }
  }

  return updateMade;
}

bool PatternScroller::WalkDimParams(uint32_t curTime) {
  bool updateMade = false;
  
  if(dimParamChangeType == IMMEDIATE) {
    // Instantly update params
    if((pg.brightLength != brightLength) || (pg.transLength != transLength)) {
      updateMade = true;
      pg.brightLength = brightLength;
      pg.transLength = transLength;
    }
  }
  else if(IsReadyForDimMove(curTime)) {
    if(dimParamChangeType == ONCE_PER_MOVE || IsStartOfDimPattern()) {
      // Gradually update params in sync with movement    
      if(pg.brightLength < brightLength) {
        pg.brightLength++;
        updateMade = true;
      }
      else if(pg.brightLength > brightLength) {
        pg.brightLength--;
        updateMade = true;
      }
      else if(pg.transLength < transLength) {
        pg.transLength++;
        updateMade = true;
      }
      else if(pg.transLength > transLength) {
        pg.transLength--;
        updateMade = true;
      }
    }
  }

  return updateMade;
}

void PatternScroller::BlendColorPattern(uint32_t curTime) {
  // debug: Do an actual blend
  memcpy(curColorPattern, targetColorPattern, sizeof(PRGB)*colorPeriod);
}

void PatternScroller::BlendDimPattern(uint32_t curTime) {
  uint32_t transitionTime = curTime - lastDimPatternChange - dimPauseLength;
  if(transitionTime < dimBlendLength) {
    uint8_t blendAmount = 255 * transitionTime / dimBlendLength;//debug: changed from 256, why?

    for(uint8_t i = 0; i < dimPeriod; i++) {
      curDimPattern[i] = (255 - blendAmount) * oldDimPattern[i] / 255 + blendAmount * targetDimPattern[i] / 255;
    }

    pr.SetDimPattern(targetDimPattern, dimPeriod);
  }
  else {
    // Blending just finished
    oldDimPatternIndex = targetDimPatternIndex;
    memcpy(oldDimPattern, targetDimPattern, dimPeriod);
    memcpy(curDimPattern, targetDimPattern, dimPeriod);
    
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

bool PatternScroller::IsReadyForDimMove(uint32_t curTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(dimSpeed == 0) { return true; }
  
  uint32_t stepSize = ONE_SECOND / abs(dimSpeed);
  return curTime - lastDimMove >= stepSize;
}

bool PatternScroller::IsReadyForColorMove(uint32_t curTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(colorSpeed == 0) { return true; }
  
  uint32_t stepSize = ONE_SECOND / abs(colorSpeed);
  return curTime - lastColorMove >= stepSize;
}

bool PatternScroller::IsStartOfDimPattern() {
  if(dimSpeed == 0) { return false; }
  else if(dimSpeed > 0) { return pr.GetDimIndexFirst() == 0; }
  else { return pr.GetDimIndexFirst() == dimPeriod - (numLEDs % dimPeriod); }
}

bool PatternScroller::IsStartOfColorPattern() {
  if(colorSpeed == 0) { return false; }
  else if(colorSpeed > 0) { return pr.GetColorIndexFirst() == 0; }
  else { return pr.GetDimIndexFirst() == colorPeriod - (numLEDs % colorPeriod); }
}

bool PatternScroller::ScrollPatterns(uint32_t curTime) {
  bool dimMoved = false;

  // Move dim pattern
  if(dimSpeed == 0) {
    lastDimMove = curTime;
  }
  else {
    if(IsReadyForDimMove(curTime)) {
      dimMoved = true;
      pr.ScrollDimPattern(dimSpeed > 0);
      lastDimMove += ONE_SECOND / abs(dimSpeed);
    }
  }

  // Move color pattern
  if(colorSpeed == 0) {
    lastColorMove = curTime;
  }
  else {
    if(IsReadyForColorMove(curTime)) {
      pr.ScrollColorPattern(colorSpeed > 0);
      lastColorMove += ONE_SECOND / abs(colorSpeed);
    }
  }

  return dimMoved;
}

