#include "PatternScroller.h"
// debug: Speed up by only do pg.WriteDimPattern() during blend function if possible; or also when not calling blend on an update

PatternScroller::PatternScroller() {
  dimParamChangeType = CENTER;//GROW_F;//GROW_R;//WORM;//FREEZE;
  changeDimParamASAP = true;
  
  oldDimPatternIndex = 0;
  targetColorPatternIndex = 0;
  targetDimPatternIndex = 0;
  randomDimPatternIndex = 0;
  randomColorPatternIndex = 0;

  colorPeriod = 1;
  curColorPattern[0] = CRGB(0, 0, 100);
  colorIndexFirst = 0;
  
  dimPeriod = 1;
  curDimPattern[0] = 255;
  dimIndexFirst = 0;
  
  brightness = 255;

  dimBlendLength_queued = false;
  colorBlendLength_queued = false;
}

uint8_t PatternScroller::GetColorPeriod() { return colorPeriod; }
uint8_t PatternScroller::GetDimPeriod() { return dimPeriod; }
uint32_t PatternScroller::GetDimBlendLength() { return dimBlendLength_queued ? dimBlendLength_q : dimBlendLength; }
void PatternScroller::SetDimBlendLength(uint32_t value) {
  if(dimBlendOn) {
    dimBlendLength_queued = true;
    dimBlendLength_q = value;
  }
  else {
    dimBlendLength = value;
  }
}
uint32_t PatternScroller::GetColorBlendLength() { return colorBlendLength_queued ? colorBlendLength_q : colorBlendLength; }
void PatternScroller::SetColorBlendLength(uint32_t value) {
  if(colorBlendOn) {
    colorBlendLength_queued = true;
    colorBlendLength_q = value;
  }
  else {
    colorBlendLength = value;
  }
}
uint32_t PatternScroller::GetDimPauseLength() { return dimPauseLength; }
void PatternScroller::SetDimPauseLength(uint32_t value, uint32_t curTime) {
  if(curTime - dimPauseLength > lastDimPatternChange) { if(dimBlendOn) { lastDimPatternChange -= (value - dimPauseLength); } }// if blending already
  else { if(curTime - value >= lastDimPatternChange) { lastDimPatternChange = curTime - value; } } // if paused and reducing pause past the start point
  dimPauseLength = value;
}
uint32_t PatternScroller::GetColorPauseLength() { return colorPauseLength; }
void PatternScroller::SetColorPauseLength(uint32_t value, uint32_t curTime) {
  if(curTime - colorPauseLength > lastColorPatternChange) { if(colorBlendOn) { lastColorPatternChange -= (value - colorPauseLength); } }// if blending already
  else { if(curTime - value >= lastColorPatternChange) { lastColorPatternChange = curTime - value; } } // if paused and reducing pause past the start point
  colorPauseLength = value;
}
int8_t PatternScroller::GetColorSpeed() { return colorSpeed; }
void PatternScroller::SetColorSpeed(int8_t value, uint32_t curTime) {
  if(abs(value) > abs(colorSpeed)) {
    // Watch out for rapidly making multiple moves when increasing speed
    uint32_t stepSize = FPS_TO_TIME(abs(value));
    if(curTime - lastColorMove >= stepSize) { lastColorMove = curTime - stepSize; }
  }
  colorSpeed = value;
}
int8_t PatternScroller::GetDimSpeed() { return dimSpeed; }
void PatternScroller::SetDimSpeed(int8_t value, uint32_t curTime) {
  if(abs(value) > abs(dimSpeed)) {
    // Watch out for rapidly making multiple moves when increasing speed
    uint32_t stepSize = FPS_TO_TIME(abs(value));
    if(curTime - lastDimMove >= stepSize) { lastDimMove = curTime - stepSize; }
  }
  dimSpeed = value;
}
    
void PatternScroller::Init(struct_base_show_params& params, uint32_t curTime, PaletteManager* _pm, GammaManager* gm, uint16_t _numLEDs) {
  if(_pm) { pm = _pm; }
  if(gm) { Gamma = gm; }
  if(_pm || gm) { pg.Init(pm, gm); }
  if(_numLEDs > 0) { numLEDs = _numLEDs; }
  
  colorIndexFirst = 0;
  dimIndexFirst = 0;

  dimSpeed = params.dimSpeed;
  colorSpeed = params.colorSpeed;
  
  dimPeriod = pg.dimPeriod = params.dimPeriod;
  colorPeriod = pg.colorPeriod = params.colorPeriod;
  numColors = pg.numColors = params.numColors;
  brightLength = pg.brightLength = params.brightLength;
  transLength = pg.transLength = params.transLength;

  uint32_t tempDim = dimPauseLength;
  uint32_t tempColor = colorPauseLength;
  dimPauseLength = 0;
  colorPauseLength = 0;
  SetDisplayMode(params.displayMode, curTime);
  dimPauseLength = tempDim;
  colorPauseLength = tempColor;
  
  pg.WriteDimPattern(GetTargetDimPatternIndex(), targetDimPattern);
  memcpy(curDimPattern, targetDimPattern, dimPeriod);
  memcpy(oldDimPattern, targetDimPattern, dimPeriod);
  pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);
  memcpy(curColorPattern, targetColorPattern, sizeof(CRGB)*colorPeriod);
  memcpy(oldColorPattern, targetColorPattern, sizeof(CRGB)*colorPeriod);

  lastDimPatternChange = curTime;
  lastColorPatternChange = curTime;
  lastDimMove = curTime;
  lastColorMove = curTime;
}

void PatternScroller::Clone(PatternScroller* source, struct_base_show_params& params, uint32_t curTime) {
  randomDimPatternIndex = source->randomDimPatternIndex;
  randomColorPatternIndex = source->randomColorPatternIndex;
  Init(params, curTime);
  lastDimMove = source->lastDimMove;
  lastColorMove = source->lastColorMove;
}

bool PatternScroller::Update(uint32_t curTime) {
// Returns true if dim pattern moved
  if(curTime - lastColorPatternChange >= colorPauseLength) {
    if(WalkColorParams(curTime)) {
      memcpy(oldColorPattern, curColorPattern, sizeof(CRGB)*colorPeriod);
      colorBlendOn = true;
      lastColorPatternChange = curTime - colorPauseLength;
    }
  }

  if(colorBlendOn) {
    pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);
    BlendColorPattern(curTime);
  }
  else {
    pg.WriteColorPattern(targetColorPatternIndex, curColorPattern);
  }


  if(WalkDimParams(curTime)) {
    pg.WriteDimPattern(GetTargetDimPatternIndex(), targetDimPattern);
    pg.WriteDimPattern(oldDimPatternIndex, oldDimPattern);
  }

  if(curTime - lastDimPatternChange >= dimPauseLength) {
    if(dimBlendOn) {
      pg.WriteDimPattern(GetTargetDimPatternIndex(), targetDimPattern);
      pg.WriteDimPattern(oldDimPatternIndex, oldDimPattern);
      BlendDimPattern(curTime);
    }
    else {
      memcpy(curDimPattern, targetDimPattern, dimPeriod);
    }
  }
  else {
    memcpy(curDimPattern, oldDimPattern, dimPeriod);
  }

  return ScrollPatterns(curTime);
}

void PatternScroller::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastDimMove += amount;
  lastDimPatternChange += amount;
  lastColorPatternChange += amount;
}

void PatternScroller::SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs) {
  uint16_t curColorIndex = colorIndexFirst;
  uint16_t curDimIndex = dimIndexFirst;

  for(uint16_t i = 0; i < numLEDs; i++) {
    if(curDimPattern[curDimIndex] == 0) { target_b[i] = 0; }
    else {
      target[i] = curColorPattern[curColorIndex];
      uint16_t pixelBrightness = (uint16_t)curDimPattern[curDimIndex] * (uint16_t)(brightness+1) / 0x100;
      target_b[i] = max(1, pixelBrightness & 0xFF); //debug: += 127?
      //Serial.println(String(i) + ": " + String(target[i].r) + ": " + String(target[i].g) + ", " + String(target[i].b));
    } 
  
    if(curDimIndex >= dimPeriod) { THROW("ERROR: SetCRGBs(): curDimIndex out of bounds: " + String(curDimIndex) + " / " + String(dimPeriod)) }
    if(curColorIndex >= colorPeriod) { THROW("ERROR: SetCRGBs(): curColorIndex out of bounds: " + String(curColorIndex) + " / " + String(colorPeriod)) }

    if(++curColorIndex == colorPeriod) { curColorIndex = 0; }
    if(++curDimIndex == dimPeriod) { curDimIndex = 0; }
  }
}

void PatternScroller::SetDisplayMode(uint8_t displayMode, uint32_t curTime) {
  uint8_t dimPatternIndex = displayMode % (NUM_DIM_PATTERNS+1); // debug: this logic isn't transparent: assumes that PatternScroller knows the # of display modes is this equation instead of setting indexes explicitly
  uint8_t colorPatternIndex = displayMode / (NUM_DIM_PATTERNS+1);
  
  if(targetColorPatternIndex != colorPatternIndex && (curTime - lastColorPatternChange >= colorPauseLength)) {
    memcpy(oldColorPattern, curColorPattern, sizeof(CRGB)*colorPeriod);
    targetColorPatternIndex = colorPatternIndex;

    pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);
    colorBlendOn = true;
    lastColorPatternChange = curTime - colorPauseLength;
  }

  if(targetDimPatternIndex != dimPatternIndex) {
    if(IsRandomDimPattern()) {
      if(curTime - lastDimPatternChange < dimPauseLength) {
        targetDimPatternIndex = oldDimPatternIndex;
        memcpy(targetDimPattern, oldDimPattern, dimPeriod);
        dimBlendOn = false;
      }
      else {
        targetDimPatternIndex = randomDimPatternIndex;
      }
    }
    else if(curTime - lastDimPatternChange >= dimPauseLength && !dimBlendOn) {
      oldDimPatternIndex = GetTargetDimPatternIndex();
      memcpy(oldDimPattern, targetDimPattern, dimPeriod);
      targetDimPatternIndex = dimPatternIndex;
      if(IsRandomDimPattern()) { do { randomDimPatternIndex = random8(NUM_DIM_PATTERNS); } while (randomDimPatternIndex == oldDimPatternIndex); }
      pg.WriteDimPattern(GetTargetDimPatternIndex(), targetDimPattern);
      dimBlendOn = true;
      lastDimPatternChange = curTime - dimPauseLength;
    }
  }
}

bool PatternScroller::WalkColorParams(uint32_t curTime) {
  bool updateMade = false;

  // Instantly update params
  if(pg.numColors != numColors) {
    updateMade = true;
    pg.numColors = numColors;
  }

  if(pg.colorPeriod != colorPeriod) { THROW(colorPeriod mismatch!) }

  return updateMade;
}

bool PatternScroller::WalkDimParams(uint32_t curTime) {
#define ADJUST_BOTH() if(pg.brightLength < brightLength) { pg.brightLength++; } \
                      else if(pg.brightLength > brightLength) { pg.brightLength--; } \
                      if(pg.transLength < transLength && pg.brightLength >= brightLength) { pg.transLength++; } \
                      else if(pg.transLength > transLength && pg.brightLength <= brightLength) { pg.transLength--; } // debug: should remove second half of if so moves of 3 are allowed?

#define SCROLL_BACK() ScrollPatternsWithoutTimer(false);
#define SCROLL_FORWARD() ScrollPatternsWithoutTimer(true);
#define ADJ_DOWNBEAT() if(changeDimParamASAP || IsReadyForDimMove(curTime)) { ADJUST_BOTH() }
#define ADJ_UPBEAT() if(changeDimParamASAP || IsHalfwayToDimMove(curTime)) { ADJUST_BOTH() }
#define ADJ_DOWNBEAT_INC() if(changeDimParamASAP || IsReadyForDimMove(curTime)) { ADJUST_BOTH() SCROLL_FORWARD() }
#define ADJ_DOWNBEAT_INC2() if(changeDimParamASAP || IsReadyForDimMove(curTime)) { ADJUST_BOTH() SCROLL_FORWARD() SCROLL_FORWARD() }
#define ADJ_DOWNBEAT_DEC() if(changeDimParamASAP || IsReadyForDimMove(curTime)) { ADJUST_BOTH() SCROLL_BACK() }
#define ADJ_DOWNBEAT_DEC2() if(changeDimParamASAP || IsReadyForDimMove(curTime)) { ADJUST_BOTH() SCROLL_BACK() SCROLL_BACK()  }
#define ADJ_UPBEAT_INC() if(changeDimParamASAP || IsHalfwayToDimMove(curTime)) { ADJUST_BOTH() SCROLL_FORWARD() }
#define ADJ_UPBEAT_DEC() if(changeDimParamASAP || IsHalfwayToDimMove(curTime)) { ADJUST_BOTH() SCROLL_BACK() }

// debug: For cases 3/-3, consider 1,-2/-2,1
  int8_t delta = 0;
  if     (pg.brightLength < brightLength)             { delta++; }
  else if(pg.brightLength > brightLength)             { delta--; }
  if     (pg.transLength < transLength && delta <= 0) { delta+=2;  }
  else if(pg.transLength > transLength && delta >= 0) { delta-=2;  }
  
  if(delta == 0) { ADJ_DOWNBEAT() }
  else if(dimParamChangeType == GROW_F) {
    switch(delta) {
      case 1:  if(dimSpeed > 0) { ADJ_UPBEAT() } else { ADJ_UPBEAT_DEC() } break;
      case -1: if(dimSpeed > 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_INC() } break;
      case 2:  if(dimSpeed > 0) { ADJ_UPBEAT() } else { ADJ_DOWNBEAT_DEC2() } break; // debug: in this and worm, should you use upbeat when moving 2 ever?
      case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_INC2() } break;
      //case 3:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC2() } else { ADJ_DOWNBEAT_DEC() } break;
      //case -3: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT_INC() } break;
      default: DUMP(delta)
    }
  }
  else if(dimParamChangeType == GROW_R) {
    switch(delta) {
      case 1:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT() } break;
      case -1: if(dimSpeed > 0) { ADJ_UPBEAT_INC() } else { ADJ_UPBEAT() } break;
      case 2:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC2() } else { ADJ_DOWNBEAT() } break;
      case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT() } break;
      //case 3:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC2() } break;
      //case -3: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT_INC2() } break;
      default: DUMP(delta)
    }
  }
  else if(dimParamChangeType == WORM) {
    switch(delta) {
      case 1:  if(dimSpeed > 0) { ADJ_UPBEAT() } else { ADJ_UPBEAT_DEC() } break;
      case -1: if(dimSpeed > 0) { ADJ_UPBEAT_INC() } else { ADJ_UPBEAT() } break;
      case 2:  if(dimSpeed > 0) { ADJ_UPBEAT() } else { ADJ_DOWNBEAT_DEC2() } break;
      case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT() } break;
      //case 3:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC2() } break;
      //case -3: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT_INC() } break;
      default: DUMP(delta)
    }
  }
  else if(dimParamChangeType == FREEZE) {
    switch(delta) {
      case 1:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT() } break;
      case -1: if(dimSpeed > 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_INC() } break;
      case 2:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC2() } else { ADJ_DOWNBEAT() } break;
      case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_INC2() } break;
      //case 3:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC2() } else { ADJ_DOWNBEAT_DEC() } break;
      //case -3: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT_INC2() } break;
      default: DUMP(delta)
    }
  }
  else if(dimParamChangeType == CENTER) {
    switch(delta) {
      case 1:
        if(pg.brightLength % 2 == 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_DEC() } break;
      case -1: 
        if(pg.brightLength % 2 == 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT() } break;
      case 2:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC() } break;
      case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT_INC() } break;
      //case 3:
        //if(pg.brightLength % 2 == 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC2() } break;
      //case -3:
        //if(pg.brightLength % 2 == 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT_INC() } break;
      default: DUMP(delta)
    }
  }
  else {
    THROW(unknown dimParamChangeType!)
    DUMP(dimParamChangeType)
  }
      
  if(pg.dimPeriod != dimPeriod) { THROW(dimPeriod mismatch without a split!) }
  return delta != 0;
}

void PatternScroller::BlendColorPattern(uint32_t curTime) {
  uint32_t transitionTime = curTime - lastColorPatternChange - colorPauseLength;
  if(transitionTime < colorBlendLength) {
    uint8_t blendAmount = 255 * transitionTime / colorBlendLength;

    for(uint8_t i = 0; i < colorPeriod; i++) {
      curColorPattern[i] = Gamma->Blend(oldColorPattern[i], targetColorPattern[i], blendAmount);
    }
  }
  else {
    // Blending just finished
    memcpy(oldColorPattern, targetColorPattern, sizeof(CRGB)*colorPeriod);
    memcpy(curColorPattern, targetColorPattern, sizeof(CRGB)*colorPeriod);

    pg.WriteColorPattern(targetColorPatternIndex, targetColorPattern);

    lastColorPatternChange = curTime;
    colorBlendOn = false;
    if(colorBlendLength_queued) {
      colorBlendLength = colorBlendLength_q;
      colorBlendLength_queued = false;
    }
  }
}

void PatternScroller::BlendDimPattern(uint32_t curTime) {
  uint32_t transitionTime = curTime - lastDimPatternChange - dimPauseLength;
  if(transitionTime < dimBlendLength) {
    uint8_t blendAmount = 255 * transitionTime / dimBlendLength;

    for(uint8_t i = 0; i < dimPeriod; i++) {
      curDimPattern[i] = (255 - blendAmount) * oldDimPattern[i] / 255 + blendAmount * targetDimPattern[i] / 255;
    }
  }
  else {
    // Blending just finished
    if(dimBlendLength_queued) {
      dimBlendLength = dimBlendLength_q;
      dimBlendLength_queued = false;
    }
    
    if(IsRandomDimPattern()) {
      oldDimPatternIndex = randomDimPatternIndex;
      do { randomDimPatternIndex = random8(NUM_DIM_PATTERNS); } while (randomDimPatternIndex == oldDimPatternIndex);
      pg.WriteDimPattern(randomDimPatternIndex, targetDimPattern);
    }
    else {
      dimBlendOn = false;
      oldDimPatternIndex = targetDimPatternIndex;
      pg.WriteDimPattern(targetDimPatternIndex, targetDimPattern);
    }

    pg.WriteDimPattern(oldDimPatternIndex, oldDimPattern);
    memcpy(curDimPattern, oldDimPattern, dimPeriod);
    lastDimPatternChange = curTime;
  }
}

bool PatternScroller::IsReadyForDimMove(uint32_t curTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(dimSpeed == 0) { return false; }
  
  uint32_t stepSize = FPS_TO_TIME(abs(dimSpeed));
  return curTime - lastDimMove >= stepSize;
}

bool PatternScroller::IsHalfwayToDimMove(uint32_t curTime) {
  // Only return true once per cycle
  if(dimSpeed == 0) { return false; }
  if(dimParamWalkedThisCycle) { return false; }
  
  uint32_t stepSize = FPS_TO_TIME(abs(dimSpeed));
  if(curTime - lastDimMove >= stepSize/2) { dimParamWalkedThisCycle = true; }
  return dimParamWalkedThisCycle;
}

bool PatternScroller::IsStartOfDimPattern() {
  if(dimSpeed == 0) { return false; }
  else if(dimSpeed > 0) { return dimIndexFirst == 0; }
  else { return dimIndexFirst == numLEDs % dimPeriod; }
}

uint8_t PatternScroller::GetTargetDimPatternIndex() {
  if(IsRandomDimPattern()) { return randomDimPatternIndex; }
  else { return targetDimPatternIndex; }
}

bool PatternScroller::IsRandomDimPattern() {
  return targetDimPatternIndex == NUM_DIM_PATTERNS;
}

bool PatternScroller::ScrollPatterns(uint32_t curTime) {
  bool dimMoved = false;

  // Move dim pattern
  if(dimSpeed == 0) {
    lastDimMove = curTime;
  }
  else if(IsReadyForDimMove(curTime)) {
    dimParamWalkedThisCycle = false;
    dimMoved = true;
    // Scroll dim pattern
    if(dimSpeed > 0) {
      if(--dimIndexFirst == 0xFF) { dimIndexFirst = dimPeriod - 1; }
    }
    else {
      if(++dimIndexFirst == dimPeriod) { dimIndexFirst = 0; }
    }
    
    lastDimMove += FPS_TO_TIME(abs(dimSpeed));
  }

  // Move color pattern
  if(colorSpeed == 0) {
    lastColorMove = curTime;
  }
  else if(curTime - lastColorMove >= FPS_TO_TIME(abs(colorSpeed))) {
    if(colorSpeed > 0) {
      if(--colorIndexFirst == 0xFF) { colorIndexFirst = colorPeriod - 1; }
    }
    else {
      if(++colorIndexFirst == colorPeriod) { colorIndexFirst = 0; }
    }
    
    lastColorMove += FPS_TO_TIME(abs(colorSpeed));
  }

  return dimMoved;
}

bool PatternScroller::ScrollPatternsWithoutTimer(bool moveForward) {
  if(moveForward) {
    if(--dimIndexFirst == 0xFF) { dimIndexFirst = dimPeriod - 1; }
  }
  else {
    if(++dimIndexFirst == dimPeriod) { dimIndexFirst = 0; }
  }
}

