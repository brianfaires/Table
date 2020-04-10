#include "PatternScrolling\PatternScroller.h"
#include "Util.h"

//todo: Speed up by only do WriteDimPattern() during blend function if possible; or also when not calling blend on an update

//todo: these settings were sitting around; are they vestigial?
// Software settings
#define PATTERN_PARAM_CHANGE_DISTANCE    0// NUM_LEDS
#define BRIGHTNESS_PARAM_CHANGE_DISTANCE 0// NUM_LEDS



PatternScroller::PatternScroller() {
  dimParamChangeType = PREFERRED;//CENTER;//GROW_F;//GROW_R;//WORM;//FREEZE;
  changeDimParamsWithMovement = true;
  enableDoubleBrightMove = false;
  
  oldDimPatternIndex = 1; // Should never be 0
  targetColorPatternIndex = 0;
  targetDimPatternIndex = 1; // Todo: clean these up; this is required to get random to actually start up correctly; happened when changing from 15->0
  randomDimPatternIndex = 1; // Should never be 0
  randomColorPatternIndex = 0;

  colorPeriod = 1;
  curColorPattern[0] = CRGB(0, 0, 100);
  colorIndexFirst = 0;
  
  dimPeriod = 1;
  curDimPattern[0] = 255;
  dimIndexFirst = 0;
  
  brightness = 255;

}

//*****************************************************************
//*************************** Accessors ***************************
//*****************************************************************
uint8_t PatternScroller::getDimPeriod() { return dimPeriod; }
uint8_t PatternScroller::getColorPeriod() { return colorPeriod; }
uint32_t PatternScroller::getDimBlendLength() { return dimBlendLength; }
void PatternScroller::setDimBlendLength(uint32_t value) {
  if(dimBlendOn) {
    float blendPerc = dimBlendLength == 0 ? 0 : (*curTime - lastDimPatternChange - dimPauseLength) / dimBlendLength;
    lastDimPatternChange = *curTime - blendPerc*value - dimPauseLength; // Preserve same blend %
  }
  dimBlendLength = value;
}
uint32_t PatternScroller::getColorBlendLength() { return colorBlendLength; }
void PatternScroller::setColorBlendLength(uint32_t value) {
  if(colorBlendOn) {
    float blendPerc = colorBlendLength == 0 ? 0 : (*curTime - lastColorPatternChange - colorPauseLength) / colorBlendLength;
    lastColorPatternChange = *curTime - blendPerc*value - colorPauseLength; // Preserve same blend %
  }
  colorBlendLength = value;
}
uint32_t PatternScroller::getDimPauseLength() { return dimPauseLength; }
void PatternScroller::setDimPauseLength(uint32_t value) {
  if(*curTime - lastDimPatternChange >= dimPauseLength) { if(dimBlendOn) { lastDimPatternChange -= (value - dimPauseLength); } }// if blending already
  else if(*curTime - lastDimPatternChange >= value) { lastDimPatternChange = *curTime - value; } // if paused and reducing pause past the start point
  dimPauseLength = value;
}
uint32_t PatternScroller::getColorPauseLength() { return colorPauseLength; }
void PatternScroller::setColorPauseLength(uint32_t value) {
  if(*curTime - lastColorPatternChange >= colorPauseLength) { if(colorBlendOn) { lastColorPatternChange -= (value - colorPauseLength); } }// if blending already
  else if(*curTime - lastColorPatternChange >= value) { lastColorPatternChange = *curTime - value; } // if paused and reducing pause past the start point
  colorPauseLength = value;
}
int8_t PatternScroller::getDimSpeed() { return dimSpeed; }
void PatternScroller::setDimSpeed(int8_t value) {
  if(abs(value) > abs(dimSpeed)) {
    // Watch out for rapidly making multiple moves when increasing speed
    uint32_t stepSize = FPS_TO_TIME(abs(value));
    if(*curTime - lastDimMove >= stepSize) { lastDimMove = *curTime - stepSize; }
  }
  dimSpeed = value;
}
int8_t PatternScroller::getColorSpeed() { return colorSpeed; }
void PatternScroller::setColorSpeed(int8_t value) {
  if(abs(value) > abs(colorSpeed)) {
    // Watch out for rapidly making multiple moves when increasing speed
    uint32_t stepSize = FPS_TO_TIME(abs(value));
    if(*curTime - lastColorMove >= stepSize) { lastColorMove = *curTime - stepSize; }
  }
  colorSpeed = value;
}
uint8_t PatternScroller::getTargetDimPatternIndex() { return IsRandomDimPattern() ? randomDimPatternIndex : targetDimPatternIndex; }
void PatternScroller::setDisplayMode(uint8_t displayMode) {
  if(displayMode >= NUM_DIM_PATTERNS * NUM_COLOR_PATTERNS) { THROW_DUMP("Invalid display mode", displayMode) }
  uint8_t dimPatternIndex = displayMode % NUM_DIM_PATTERNS;
  uint8_t colorPatternIndex = displayMode / NUM_DIM_PATTERNS;

  if(targetColorPatternIndex != colorPatternIndex) {
    if(*curTime - lastColorPatternChange >= colorPauseLength) {
      // Begin blending into new colorPattern
      memcpy(oldColorPattern, curColorPattern, sizeof(CRGB)*colorPeriod);
      targetColorPatternIndex = colorPatternIndex;
      WriteColorPattern(targetColorPatternIndex, targetColorPattern);
      colorBlendOn = true;
      lastColorPatternChange = *curTime - colorPauseLength; // Might already be blending
    }
  }

  if(targetDimPatternIndex != dimPatternIndex) {
    bool isTimeForChange = *curTime - lastDimPatternChange >= dimPauseLength;
    if(IsRandomDimPattern()) {
      if(isTimeForChange) {
        // Currently on or blending toward randomDimPatternIndex
        targetDimPatternIndex = randomDimPatternIndex;
      }
      else {
        // Have not started blending yet
        targetDimPatternIndex = oldDimPatternIndex;
        memcpy(targetDimPattern, oldDimPattern, dimPeriod);
        dimBlendOn = false;
      }
    }    
    else if(!dimBlendOn && isTimeForChange) {
      dimBlendOn = true;
      oldDimPatternIndex = targetDimPatternIndex;
      memcpy(oldDimPattern, targetDimPattern, dimPeriod);
      targetDimPatternIndex = dimPatternIndex;
      if(IsRandomDimPattern()) { do { randomDimPatternIndex = random8(1, NUM_DIM_PATTERNS-1); } while (randomDimPatternIndex == oldDimPatternIndex); }
      WriteDimPattern(getTargetDimPatternIndex(), targetDimPattern);
      lastDimPatternChange = *curTime - dimPauseLength;
    }
  }
}


//*****************************************************************
//******************* Simple boolean functions ********************
//*****************************************************************
bool dimParamWalkedThisCycle = false;
bool PatternScroller::IsReadyForDimMove() {
  // Returns true if this cycle is going to move the pattern
  if(dimSpeed == 0) { return false; }
  
  uint32_t stepSize = FPS_TO_TIME(abs(dimSpeed));
  if(*curTime - lastDimMove >= stepSize) {
    dimParamWalkedThisCycle = false;
    return true;
  }

  return false;
}
bool PatternScroller::IsHalfwayToDimMove() {
  // Only return true once per cycle
  if(dimSpeed == 0) { return false; }
  if(dimParamWalkedThisCycle) { return false; }
  
  uint32_t stepSize = FPS_TO_TIME(abs(dimSpeed));
  if(*curTime - lastDimMove >= stepSize/2) { dimParamWalkedThisCycle = true; }
  return dimParamWalkedThisCycle;
}
bool PatternScroller::IsStartOfDimPattern() {
  if(dimSpeed == 0) { return false; }
  else if(dimSpeed > 0) { return dimIndexFirst == 0; }
  else { return dimIndexFirst == numLEDs % dimPeriod; }
}
bool PatternScroller::IsRandomDimPattern() {
  return targetDimPatternIndex == 0;
}


//*****************************************************************
//************************ Object overhead ************************
//*****************************************************************
void PatternScroller::Init(struct_base_show_params& params, uint32_t* _curTime, PaletteManager* _pm, GammaManager* _gm, uint16_t _numLEDs) {
  if(_pm) { pm = _pm; }
  if(_gm) { Gamma = _gm; }
  if(_pm || _gm) { ColorPattern::Init(pm, Gamma); }
  if(_numLEDs > 0) { numLEDs = _numLEDs; }
  if(_curTime) { curTime = _curTime; }
  
  colorIndexFirst = 0;
  dimIndexFirst = 0;

  dimSpeed = params.dimSpeed;
  colorSpeed = params.colorSpeed;
  
  dimPeriod = DimPattern::dimPeriod = params.dimPeriod;
  brightLength = DimPattern::brightLength = params.brightLength;
  transLength = DimPattern::transLength = params.transLength;
  colorPeriod = ColorPattern::colorPeriod = params.colorPeriod;
  numColors = ColorPattern::numColors = params.numColors;

  uint32_t tempDim = dimPauseLength;
  uint32_t tempColor = colorPauseLength;
  dimPauseLength = 0;
  colorPauseLength = 0;
  setDisplayMode(params.displayMode); // 0 pause length ensures blending begins immediately
  dimPauseLength = tempDim;
  colorPauseLength = tempColor;
  
  WriteDimPattern(getTargetDimPatternIndex(), targetDimPattern);
  memcpy(curDimPattern, targetDimPattern, dimPeriod);
  memcpy(oldDimPattern, targetDimPattern, dimPeriod);
  WriteColorPattern(targetColorPatternIndex, targetColorPattern);
  memcpy(curColorPattern, targetColorPattern, sizeof(CRGB)*colorPeriod);
  memcpy(oldColorPattern, targetColorPattern, sizeof(CRGB)*colorPeriod);
  oldDimPatternIndex = getTargetDimPatternIndex();

  lastDimPatternChange = *curTime;
  lastColorPatternChange = *curTime;
  lastDimMove = *curTime;
  lastColorMove = *curTime;
}
void PatternScroller::Clone(PatternScroller* source, struct_base_show_params& params) {
  randomDimPatternIndex = source->randomDimPatternIndex;
  randomColorPatternIndex = source->randomColorPatternIndex;
  Init(params);
  dimSpeed = source->dimSpeed;
  colorSpeed = source->colorSpeed;
  lastDimMove = source->lastDimMove;
  lastColorMove = source->lastColorMove;
  lastDimPatternChange = source->lastDimPatternChange;
  lastColorPatternChange = source->lastColorPatternChange;
}
void PatternScroller::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastDimMove += amount;
  lastDimPatternChange += amount;
  lastColorPatternChange += amount;
}


//*****************************************************************
//*********************** Walking/Scrolling ***********************
//*****************************************************************
bool PatternScroller::Update() { // Returns true if dim pattern moved
  // Color changing - params and pattern can only change after pauseLength, because they both force a fade.
  if(*curTime - lastColorPatternChange >= colorPauseLength) {
    if(WalkColorParams()) {
      memcpy(oldColorPattern, curColorPattern, sizeof(CRGB)*colorPeriod);
      colorBlendOn = true;
      lastColorPatternChange = *curTime - colorPauseLength; // Might already be blending
    }
  }

  if(colorBlendOn) {
    WriteColorPattern(targetColorPatternIndex, targetColorPattern);
    BlendColorPattern();
  }
  else {
    WriteColorPattern(targetColorPatternIndex, curColorPattern);
  }


  // Dim changing - Always walk params, because they are being slow-walked to their true targets. Pattern changing respects pauseLength.
  // While randomDimPattern, dimBlendOn is always on; even during pauseLength period
  if(WalkDimParams()) {
    WriteDimPattern(getTargetDimPatternIndex(), targetDimPattern);
    WriteDimPattern(oldDimPatternIndex, oldDimPattern);
  }

  if(*curTime - lastDimPatternChange >= dimPauseLength) {
    if(dimBlendOn) {
      WriteDimPattern(getTargetDimPatternIndex(), targetDimPattern); // These need to be written again in case this was triggered by SetDisplayMode()
      WriteDimPattern(oldDimPatternIndex, oldDimPattern);
      BlendDimPattern();
    }
    else {
      memcpy(curDimPattern, targetDimPattern, dimPeriod);
    }
  }
  else {
    memcpy(curDimPattern, oldDimPattern, dimPeriod);
  }

  return ScrollPatterns();
}
bool PatternScroller::WalkColorParams() {
  bool updateMade = false;

  // Instantly update params
  if(ColorPattern::numColors != numColors) {
    updateMade = true;
    ColorPattern::numColors = numColors;
  }

  if(ColorPattern::colorPeriod != colorPeriod) { THROW("colorPeriod mismatch!") }

  return updateMade;
}
bool PatternScroller::WalkDimParams() {
  static bool blendParamsOn = false;
  static param_change_type changeType = CENTER;

  #define ADJUST_BOTH() if(DimPattern::brightLength < brightLength) { DimPattern::brightLength++; } \
                        else if(DimPattern::brightLength > brightLength) { DimPattern::brightLength--; } \
                        if(DimPattern::transLength < transLength) { DimPattern::transLength++; if(enableDoubleBrightMove && DimPattern::brightLength > brightLength) { DimPattern::brightLength--; } } \
                        else if(DimPattern::transLength > transLength) { DimPattern::transLength--; if(enableDoubleBrightMove && DimPattern::brightLength < brightLength) { DimPattern::brightLength++; } } 
  
  #define SCROLL_BACK() ScrollPatternsWithoutTimer(false);
  #define SCROLL_FORWARD() ScrollPatternsWithoutTimer(true);
  #define ADJ_DOWNBEAT() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() }
  #define ADJ_UPBEAT() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() }
  #define ADJ_DOWNBEAT_INC() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() SCROLL_FORWARD() }
  #define ADJ_DOWNBEAT_INC2() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() SCROLL_FORWARD() SCROLL_FORWARD() }
  #define ADJ_DOWNBEAT_DEC() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() SCROLL_BACK() }
  #define ADJ_DOWNBEAT_DEC2() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() SCROLL_BACK() SCROLL_BACK()  }
  #define ADJ_UPBEAT_INC() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() SCROLL_FORWARD() }
  #define ADJ_UPBEAT_INC2() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() SCROLL_FORWARD() SCROLL_FORWARD() }
  #define ADJ_UPBEAT_DEC() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() SCROLL_BACK() }
  #define ADJ_UPBEAT_DEC2() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() SCROLL_BACK() SCROLL_BACK() }
  

  int8_t delta = 0;
  if     (DimPattern::brightLength < brightLength)             { delta = 1; }
  else if(DimPattern::brightLength > brightLength)             { delta = -1; }
  // Using delta = 5/-5 to signal an actual change of 1/-1, but with bright and trans changing in opposite directions
  if     (DimPattern::transLength < transLength) { delta = delta==-1 ? 5 : delta+2; }
  else if(DimPattern::transLength > transLength) { delta = delta==1 ? -5 : delta-2; }
  // Allow brightLength to be double updated only when offsetting transLength so delta=0; Using delta=4 to signal this
  if(enableDoubleBrightMove) {
    if(delta == 5 && DimPattern::brightLength > brightLength + 1) { delta = 4; }
    else if(delta == -5 && DimPattern::brightLength + 1 < brightLength) { delta = 4; }
  }

  if(delta == 0) {
    // No changes
    blendParamsOn = false;
  }
  else if(delta == 4) {
    // Trading 2 brightLength for 1 transLength
    ADJ_DOWNBEAT()
  }
  else {
    if(!blendParamsOn) {
      // Lock in the changeType at the start of the blend
      blendParamsOn = true;
      if(dimParamChangeType != PREFERRED) { changeType = dimParamChangeType; }
      else if(dimBlendOn && *curTime - lastDimPatternChange >= dimPauseLength + dimBlendLength/2) {
        changeType = GetPreferredDimParamChangeType(getTargetDimPatternIndex(), delta);
      }
      else {
        changeType = GetPreferredDimParamChangeType(oldDimPatternIndex, delta);
      }
    }
  
    if(changeType == GROW_F) {
      switch(delta) {
        case 5:
        case 1:  if(dimSpeed > 0) { ADJ_UPBEAT() } else { ADJ_UPBEAT_DEC() } break;
        case -5:
        case -1: if(dimSpeed > 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_INC() } break;
        case 2:  if(dimSpeed > 0) { ADJ_UPBEAT() } else { ADJ_UPBEAT_DEC2() } break;
        case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_INC2() } break;
        case 3:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC2() } else { ADJ_DOWNBEAT_DEC() } break;
        case -3: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT_INC() } break;
        default: DUMP(delta)
      }
    }
    else if(changeType == GROW_R) {
      switch(delta) {
        case 5:
        case 1:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT() } break;
        case -5:
        case -1: if(dimSpeed > 0) { ADJ_UPBEAT_INC() } else { ADJ_UPBEAT() } break;
        case 2:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC2() } else { ADJ_DOWNBEAT() } break;
        case -2: if(dimSpeed > 0) { ADJ_UPBEAT_INC2() } else { ADJ_UPBEAT() } break;
        case 3:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC2() } break;
        case -3: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT_INC2() } break;
        default: DUMP(delta)
      }
    }
    else if(changeType == WORM) {
      switch(delta) {
        case 5:
        case 1:  if(dimSpeed > 0) { ADJ_UPBEAT() } else { ADJ_UPBEAT_DEC() } break;
        case -5:
        case -1: if(dimSpeed > 0) { ADJ_UPBEAT_INC() } else { ADJ_UPBEAT() } break;
        case 2:  if(dimSpeed > 0) { ADJ_UPBEAT() } else { ADJ_UPBEAT_DEC2() } break;
        case -2: if(dimSpeed > 0) { ADJ_UPBEAT_INC2() } else { ADJ_UPBEAT() } break;
        case 3:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC2() } break;
        case -3: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT_INC() } break;
        default: DUMP(delta)
      }
    }
    else if(changeType == FREEZE) {
      switch(delta) {
        case 5:
        case 1:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT() } break;
        case -5:
        case -1: if(dimSpeed > 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_INC() } break;
        case 2:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC2() } else { ADJ_DOWNBEAT() } break;
        case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_INC2() } break;
        case 3:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC2() } else { ADJ_DOWNBEAT_DEC() } break;
        case -3: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT_INC2() } break;
        default: DUMP(delta)
      }
    }
    else if(changeType == CENTER) {
      switch(delta) {
        case 5:
        case 1: if(DimPattern::brightLength % 2 == 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_DEC() } break;
        case -5:
        case -1: if(DimPattern::brightLength % 2 == 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT() } break;
        case 2:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC() } break;
        case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT_INC() } break;
        case 3:  if(DimPattern::brightLength % 2 == 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC2() } break;
        case -3: if(DimPattern::brightLength % 2 == 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT_INC() } break;
        default: DUMP(delta)
      }
    }
    else {
      THROW("unknown dimParamChangeType!")
      DUMP(dimParamChangeType)
    }
  }
      
  if(DimPattern::dimPeriod != dimPeriod) { THROW("dimPeriod mismatch without a split!") }
  return delta != 0;
}
bool PatternScroller::ScrollPatterns() {
  bool dimMoved = false;
  // Move dim pattern
  if(dimSpeed == 0) {
    lastDimMove = *curTime;
  }
  else if(IsReadyForDimMove()) {
    dimMoved = true;
    // Scroll dim pattern
    if(dimSpeed > 0) {
      if(--dimIndexFirst == 0xFF) { dimIndexFirst = dimPeriod - 1; }
    }
    else {
      if(++dimIndexFirst == dimPeriod) { dimIndexFirst = 0; }
    }

  //  static uint32_t lastActualDimMove = 0;
  //  PRINTLN((*curTime - lastActualDimMove) + "\t" + FPS_TO_TIME(abs(dimSpeed)))
  //  lastActualDimMove = *curTime;
    lastDimMove += FPS_TO_TIME(abs(dimSpeed));
  }

  // Move color pattern
  if(colorSpeed == 0) {
    lastColorMove = *curTime;
  }
  else if(*curTime - lastColorMove >= FPS_TO_TIME(abs(colorSpeed))) {
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
void PatternScroller::ScrollPatternsWithoutTimer(bool moveForward) {
  if(moveForward) {
    if(--dimIndexFirst == 0xFF) { dimIndexFirst = dimPeriod - 1; }
  }
  else {
    if(++dimIndexFirst == dimPeriod) { dimIndexFirst = 0; }
  }
}


//*****************************************************************
//************************ Display/Blending ***********************
//*****************************************************************
void PatternScroller::SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs) {
  uint16_t curColorIndex = colorIndexFirst;
  uint16_t curDimIndex = dimIndexFirst;

  for(uint16_t i = 0; i < numLEDs; i++) {
    if(curDimPattern[curDimIndex] == 0) { target_b[i] = 0; }
    else {
      target[i] = curColorPattern[curColorIndex];
      uint16_t pixelBrightness = uint16_t(curDimPattern[curDimIndex]) * uint16_t(brightness+1) / 0x100;
      target_b[i] = max(1, pixelBrightness & 0xFF);
    } 
  
    if(curDimIndex >= dimPeriod) { THROW("ERROR: SetCRGBs(): curDimIndex out of bounds: " + curDimIndex + " / " + dimPeriod) }
    if(curColorIndex >= colorPeriod) { THROW("ERROR: SetCRGBs(): curColorIndex out of bounds: " + curColorIndex + " / " + colorPeriod) }

    if(++curColorIndex == colorPeriod) { curColorIndex = 0; }
    if(++curDimIndex == dimPeriod) { curDimIndex = 0; }
  }
}
void PatternScroller::BlendColorPattern() {
  uint32_t transitionTime = *curTime - lastColorPatternChange - colorPauseLength;
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
    lastColorPatternChange = *curTime;
    colorBlendOn = false;
    WriteColorPattern(targetColorPatternIndex, targetColorPattern); // Todo: It's possible that this isn't needed; and if so, targetColorPattern and curColorPattern could be merged?
  }
}
void PatternScroller::BlendDimPattern() {
  uint32_t transitionTime = *curTime - lastDimPatternChange - dimPauseLength;
  if(transitionTime < dimBlendLength) {
    uint8_t blendAmount = 255 * transitionTime / dimBlendLength;

    for(uint8_t i = 0; i < dimPeriod; i++) {
      curDimPattern[i] = (255 - blendAmount) * oldDimPattern[i] / 255 + blendAmount * targetDimPattern[i] / 255;
    }
  }
  else {
    // Blending just finished
    if(IsRandomDimPattern()) {
      oldDimPatternIndex = randomDimPatternIndex;
      do { randomDimPatternIndex = random8(1, NUM_DIM_PATTERNS-1); } while (randomDimPatternIndex == oldDimPatternIndex);
      WriteDimPattern(randomDimPatternIndex, targetDimPattern);
    }
    else {
      dimBlendOn = false;
      oldDimPatternIndex = targetDimPatternIndex;
      WriteDimPattern(targetDimPatternIndex, targetDimPattern);
    }

    WriteDimPattern(oldDimPatternIndex, oldDimPattern);
    memcpy(curDimPattern, oldDimPattern, dimPeriod);
    lastDimPatternChange = *curTime;
  }
}


param_change_type PatternScroller::GetPreferredDimParamChangeType(uint8_t patternIndex, int8_t delta) {
  // delta can be +/- 3. If so, the actual change was +/- 1, but both bright and trans were updated
  
  switch(allDimPatterns[patternIndex]->patternType) {
    case PatternType::SYMMETRIC:
      return !changeDimParamsWithMovement ? CENTER : abs(delta)==2 ? CENTER : WORM;
    case PatternType::FRONT:
      return dimSpeed > 0 ? GROW_R : GROW_F;
    case PatternType::REVERSE:
      return dimSpeed < 0 ? GROW_R : GROW_F;
    case PatternType::FRONT3:
      return abs(delta)==3 ? WORM : abs(delta)==2 ? CENTER : (dimSpeed > 0 ? GROW_F : WORM); 
    case PatternType::REVERSE3:
      return abs(delta)==3 ? WORM : abs(delta)==2 ? CENTER : (dimSpeed > 0 ? GROW_R : WORM);

  }
  
  return delta == 1 ? WORM : CENTER;
}





void PatternScroller::WriteDimPattern(uint8_t patternIndex, uint8_t* outputArray) {
  // All patterns have a length (excluding spacing) of this
  uint8_t brightPeriod = 2*DimPattern::transLength + DimPattern::brightLength + 9;
  if(brightPeriod > DimPattern::dimPeriod) { THROW_DUMP("Out of bounds dimPeriod", brightPeriod) }
  allDimPatterns[patternIndex]->Draw(outputArray);
  //for(uint8_t i =0; i < dimPeriod; i++) { Serial.println(String(i) + ": " + outputArray[i]);}
}

void PatternScroller::WriteColorPattern(uint8_t patternIndex, CRGB* outputArray) {
  allColorPatterns[patternIndex]->Draw(outputArray);
  //for(uint8_t i =0; i < colorPeriod; i++) { Serial.println(String(i) + ": (" + outputArray[i].r + ", " + outputArray[i].g + ", " + outputArray[i].b + ")"); }
}
