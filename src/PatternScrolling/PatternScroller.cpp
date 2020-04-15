#include "PatternScrolling\PatternScroller.h"
#include "Util.h"

//todo: Speed up by only do WriteDimPattern() during blend function if possible; or also when not calling blend on an update

//todo: these settings were sitting around; are they vestigial?
// Software settings
#define PATTERN_PARAM_CHANGE_DISTANCE    0// NUM_LEDS
#define BRIGHTNESS_PARAM_CHANGE_DISTANCE 0// NUM_LEDS

PatternScroller::PatternScroller() {
  brightness = 255;
  dimParamChangeMode = DimParamChangeMode::Preferred;//CENTER;//GROW_F;//GROW_R;//WORM;//FREEZE;
  changeDimParamsWithMovement = true;
  enableDoubleBrightMove = false;
  
  // Todo: Look into why these affect startup
  oldDimPatternIndex = int(DimPatternName::Comet_F); // Should never be 0
  targetDimPatternIndex = int(DimPatternName::Comet_R);
  randomDimPatternIndex = int(DimPatternName::Slide_L); // Should never be 0
  
  targetColorPatternIndex = 0;
  randomColorPatternIndex = 0;

  colorPeriod = 1;
  curColorPattern[0] = CRGB(0, 0, 100);
  colorIndexFirst = 0;
  
  dimPeriod = 1;
  curDimPattern[0] = 255;
  dimIndexFirst = 0;
}

//*****************************************************************
//*************************** Accessors ***************************
//*****************************************************************
uint8_t PatternScroller::getDimPeriod() { return dimPeriod; }
uint8_t PatternScroller::getColorPeriod() { return colorPeriod; }
uint32_t PatternScroller::getDimBlendLength() { return dimBlendLength; }
void PatternScroller::setDimBlendLength(uint32_t value) {
  if((*curTime - lastDimPatternChange > dimPauseLength) && dimBlendOn) { // Overkill because of random mode
    float blendPerc = dimBlendLength == 0 ? 0 : float(*curTime - lastDimPatternChange - dimPauseLength) / dimBlendLength;
    lastDimPatternChange = *curTime - blendPerc*value - dimPauseLength; // Preserve same blend %
  }
  dimBlendLength = value;
}
uint32_t PatternScroller::getColorBlendLength() { return colorBlendLength; }
void PatternScroller::setColorBlendLength(uint32_t value) {
  if((*curTime - lastColorPatternChange > colorPauseLength) && colorBlendOn) { // Overkill because of random mode
    float blendPerc = colorBlendLength == 0 ? 0 : float(*curTime - lastColorPatternChange - colorPauseLength) / colorBlendLength;
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
uint8_t PatternScroller::getNumBlanks() { return dimPeriod - 9 - 2*dimPattern.transLength - dimPattern.brightLength; }
void PatternScroller::setDisplayMode(uint8_t displayMode) {
  if(displayMode >= NUM_DIM_PATTERNS * NUM_COLOR_PATTERNS) { THROW_DUMP("Invalid display mode", displayMode) }
  uint8_t dimPatternIndex = displayMode % NUM_DIM_PATTERNS;
  uint8_t colorPatternIndex = displayMode / NUM_DIM_PATTERNS;

  if(targetColorPatternIndex != colorPatternIndex) {
    if(*curTime - lastColorPatternChange >= colorPauseLength) {
      // Begin blending into new colorPattern; even if in the middle of a blend
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
      // Don't accept new targets while blending
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
  #ifdef DEBUG_COLOR_PATTERNS
    return false;
  #endif
  return targetDimPatternIndex == uint8_t(DimPatternName::Random);
}


//*****************************************************************
//************************ Object overhead ************************
//*****************************************************************
void PatternScroller::Init(struct_base_show_params& params, uint32_t* _curTime, PaletteManager* _pm, GammaManager* _gm, uint16_t _numLEDs) {
  if(_pm) { pm = _pm; }
  if(_gm) { Gamma = _gm; }
  if(_pm || _gm) { colorPattern.Init(pm, Gamma); }
  if(_numLEDs > 0) { numLEDs = _numLEDs; }
  if(_curTime) { curTime = _curTime; }
  
  colorIndexFirst = 0;
  dimIndexFirst = 0;

  dimSpeed = params.dimSpeed;
  colorSpeed = params.colorSpeed;
  
  dimPeriod = dimPattern.dimPeriod = params.dimPeriod;
  brightLength = dimPattern.brightLength = params.brightLength;
  transLength = dimPattern.transLength = params.transLength;
  colorPeriod = colorPattern.colorPeriod = params.colorPeriod;
  numColors = colorPattern.numColors = params.numColors;

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
bool PatternScroller::Update(int8_t& shiftAmount) { // Returns true if dim pattern moved
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
  if(WalkDimParams(shiftAmount)) {
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
  if(colorPattern.numColors != numColors) {
    updateMade = true;
    colorPattern.numColors = numColors;
  }

  if(colorPattern.colorPeriod != colorPeriod) { THROW("colorPeriod mismatch!") }

  return updateMade;
}
bool PatternScroller::WalkDimParams(int8_t& shiftAmount) {
  static bool blendParamsOn = false;
  static DimParamChangeMode changeMode = DimParamChangeMode::Center;

  #define ADJUST_BOTH() if(dimPattern.brightLength < brightLength) { dimPattern.brightLength++; } \
                        else if(dimPattern.brightLength > brightLength) { dimPattern.brightLength--; } \
                        if(dimPattern.transLength < transLength) { dimPattern.transLength++; if(enableDoubleBrightMove && dimPattern.brightLength > brightLength) { dimPattern.brightLength--; } } \
                        else if(dimPattern.transLength > transLength) { dimPattern.transLength--; if(enableDoubleBrightMove && dimPattern.brightLength < brightLength) { dimPattern.brightLength++; } } 
  
  #define SCROLL_BACK() ScrollPatternsWithoutTimer(false); shiftAmount--;
  #define SCROLL_FORWARD() ScrollPatternsWithoutTimer(true); shiftAmount++;
  #define ADJ_DOWNBEAT() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() }
  #define ADJ_UPBEAT() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() }
  #define ADJ_DOWNBEAT_INC() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() SCROLL_FORWARD() }
  #define ADJ_DOWNBEAT_INC2() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() SCROLL_FORWARD() SCROLL_FORWARD() }
  #define ADJ_DOWNBEAT_DEC() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() SCROLL_BACK() }
  #define ADJ_DOWNBEAT_DEC2() if(!changeDimParamsWithMovement || IsReadyForDimMove()) { ADJUST_BOTH() SCROLL_BACK() SCROLL_BACK() }
  #define ADJ_UPBEAT_INC() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() SCROLL_FORWARD() }
  #define ADJ_UPBEAT_INC2() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() SCROLL_FORWARD() SCROLL_FORWARD() }
  #define ADJ_UPBEAT_DEC() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() SCROLL_BACK() }
  #define ADJ_UPBEAT_DEC2() if(!changeDimParamsWithMovement || IsHalfwayToDimMove()) { ADJUST_BOTH() SCROLL_BACK() SCROLL_BACK() }
  
  shiftAmount = 0; // Initialize to 0 and then increment/decrement - returns to PatternScroller to affect splitIndex
  int8_t delta = 0;
  if     (dimPattern.brightLength < brightLength)             { delta = 1; }
  else if(dimPattern.brightLength > brightLength)             { delta = -1; }
  // Using delta = 5/-5 to signal an actual change of 1/-1, but with bright and trans changing in opposite directions
  if     (dimPattern.transLength < transLength) { delta = delta==-1 ? 5 : delta+2; }
  else if(dimPattern.transLength > transLength) { delta = delta==1 ? -5 : delta-2; }
  // Allow brightLength to be double updated only when offsetting transLength so delta=0; Using delta=4 to signal this
  if(enableDoubleBrightMove) {
    if(delta == 5 && dimPattern.brightLength > brightLength + 1) { delta = 4; }
    else if(delta == -5 && dimPattern.brightLength + 1 < brightLength) { delta = 4; }
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
      // Lock in the changeMode at the start of the blend
      blendParamsOn = true;
      if(dimParamChangeMode != DimParamChangeMode::Preferred) { changeMode = dimParamChangeMode; }
      else if(dimBlendOn && (*curTime - lastDimPatternChange - dimBlendLength/2 >= dimPauseLength)) {
        changeMode = GetPreferredDimParamChangeMode(getTargetDimPatternIndex(), delta);
      }
      else {
        changeMode = GetPreferredDimParamChangeMode(oldDimPatternIndex, delta);
      }
    }
  
    switch(changeMode)
    {
      case DimParamChangeMode::Grow_F:
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
        break;
      case DimParamChangeMode::Grow_R:
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
        break;
      case DimParamChangeMode::Worm:
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
        break;
      case DimParamChangeMode::Freeze:
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
        break;
      case DimParamChangeMode::Center:
        switch(delta) {
          case 5:
          case 1: if(dimPattern.brightLength % 2 == 0) { ADJ_DOWNBEAT() } else { ADJ_DOWNBEAT_DEC() } break;
          case -5:
          case -1: if(dimPattern.brightLength % 2 == 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT() } break;
          case 2:  if(dimSpeed > 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC() } break;
          case -2: if(dimSpeed > 0) { ADJ_DOWNBEAT_INC() } else { ADJ_DOWNBEAT_INC() } break;
          case 3:  if(dimPattern.brightLength % 2 == 0) { ADJ_DOWNBEAT_DEC() } else { ADJ_DOWNBEAT_DEC2() } break;
          case -3: if(dimPattern.brightLength % 2 == 0) { ADJ_DOWNBEAT_INC2() } else { ADJ_DOWNBEAT_INC() } break;
          default: DUMP(delta)
        }
        break;
      case DimParamChangeMode::Mix_F://worm on bright and freeze on trans, and vice versa for Mix_R
        break;
      case DimParamChangeMode::Mix_R:
        break;
      default:
        THROW("unknown dimParamChangeMode!")
        DUMP(int(dimParamChangeMode))
    }
  }
      
  if(dimPattern.dimPeriod != dimPeriod) { THROW("dimPeriod mismatch without a split!") }
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
void PatternScroller::SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs, uint16_t numLEDsToSkip) {
  uint16_t curColorIndex = colorIndexFirst;
  uint16_t curDimIndex = dimIndexFirst;

  for(uint16_t i = 0; i < numLEDs; i++) {
    if(i >= numLEDsToSkip) {
      if(curDimPattern[curDimIndex] == 0) { target_b[i] = 0; }
      else {
        target[i] = curColorPattern[curColorIndex];
        uint16_t pixelBrightness = uint16_t(curDimPattern[curDimIndex]) * uint16_t(brightness+1) / 0x100;
        target_b[i] = max(1, pixelBrightness & 0xFF);
      }
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
    float blendAmount = 255.0 * float(transitionTime) / colorBlendLength;

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
    WriteColorPattern(targetColorPatternIndex, targetColorPattern); // It's possible that this isn't needed
  }
}
void PatternScroller::BlendDimPattern() {
  uint32_t transitionTime = *curTime - lastDimPatternChange - dimPauseLength;
  if(transitionTime < dimBlendLength) {
    float blendAmount = float(transitionTime) / dimBlendLength;
    for(uint8_t i = 0; i < dimPeriod; i++) {
      curDimPattern[i] = (1 - blendAmount) * oldDimPattern[i] + blendAmount * targetDimPattern[i];
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

DimParamChangeMode PatternScroller::GetPreferredDimParamChangeMode(uint8_t patternIndex, int8_t delta) {
  // delta can be +/- 5. If so, the actual change was +/- 1, but both bright and trans were updated
  
  PatternType patternType = dimPattern.getPatternType(DimPatternName(patternIndex));
  switch(patternType) {
    case PatternType::Symmetric:
      return !(changeDimParamsWithMovement || abs(delta)==2) ? DimParamChangeMode::Center : DimParamChangeMode::Worm;
    case PatternType::Front:
      return dimSpeed > 0 ? DimParamChangeMode::Grow_R : DimParamChangeMode::Grow_F;
    case PatternType::Reverse:
      return dimSpeed < 0 ? DimParamChangeMode::Grow_R : DimParamChangeMode::Grow_F;
    case PatternType::Front3:
      return abs(delta)==5 ? DimParamChangeMode::Worm : abs(delta)==2 ? DimParamChangeMode::Center : (dimSpeed > 0 ? DimParamChangeMode::Grow_F : DimParamChangeMode::Worm); 
    case PatternType::Reverse3:
      return abs(delta)==5 ? DimParamChangeMode::Worm : abs(delta)==2 ? DimParamChangeMode::Center : (dimSpeed > 0 ? DimParamChangeMode::Grow_R : DimParamChangeMode::Worm);
    default:
      THROW_DUMP(F("Invalid dimPattern used"), int(patternType))
  }
  
  return delta == 1 ? DimParamChangeMode::Worm : DimParamChangeMode::Center;
}

void PatternScroller::WriteDimPattern(uint8_t patternIndex, uint8_t* outputArray) {
  // All patterns have a length (excluding spacing) of this
  uint8_t brightPeriod = 2*dimPattern.transLength + dimPattern.brightLength + 9;
  if(brightPeriod > dimPattern.dimPeriod) { THROW_DUMP("Out of bounds dimPeriod", brightPeriod) }
  dimPattern.Draw(DimPatternName(patternIndex), outputArray);
  //for(uint8_t i =0; i < dimPeriod; i++) { Serial.println(String(i) + ": " + outputArray[i]);}
}

void PatternScroller::WriteColorPattern(uint8_t patternIndex, CRGB* outputArray) {
  colorPattern.Draw(ColorPatternName(patternIndex), outputArray);
  //for(uint8_t i =0; i < colorPeriod; i++) { Serial.println(String(i) + ": (" + outputArray[i].r + ", " + outputArray[i].g + ", " + outputArray[i].b + ")"); }
}
