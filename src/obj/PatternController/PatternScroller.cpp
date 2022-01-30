#include "obj/PatternController/PatternScroller.h"
#include "Util.h"

//todo: Speed up by only do WriteDimPattern() during blend function if possible; or also when not calling blend on an update

//todo: these settings were sitting around; are they vestigial?
// Software settings
#define PATTERN_PARAM_CHANGE_DISTANCE    0// NUM_LEDS
#define BRIGHTNESS_PARAM_CHANGE_DISTANCE 0// NUM_LEDS

PatternScroller::PatternScroller() {
  brightness = 255;
  
  baseDimParamChangeType = BaseDimParamChangeType::Manual;
  changeDimParamsWithMovement = true;
  enableDoubleBrightMove = false;

  dimPatternChangeType = DimPatternChangeType::Preferred;//CENTER;//GROW_F;//GROW_R;//WORM;//FREEZE;
    
  // Todo: Look into why these affect startup
  oldDimPatternIndex = int(DimPatternName::Comet_F); // Should never be 0
  targetDimPatternIndex = int(DimPatternName::Comet_R);
  randomDimPatternIndex = int(DimPatternName::Slide_L); // Should never be 0
  
  targetColorPatternIndex = int(ColorPatternName::Gradient);
  randomColorPatternIndex = int(ColorPatternName::Blocks);

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
uint16_t PatternScroller::getDimPeriod() { return dimPeriod; }
uint16_t PatternScroller::getColorPeriod() { return colorPeriod; }
void PatternScroller::setColorPeriod(uint16_t value) {
  colorPeriod = value;
}
uint32_t PatternScroller::getDimBlendLength() { return dimBlendLength; }
void PatternScroller::setDimBlendLength(uint32_t value) {
  if((*curTime > lastDimPatternChange + dimPauseLength) && dimBlendOn) { // Overkill because of random mode
    float blendPerc = dimBlendLength == 0 ? 0 : float(*curTime - lastDimPatternChange - dimPauseLength) / dimBlendLength;
    lastDimPatternChange = *curTime - blendPerc*value - dimPauseLength; // Preserve same blend %
  }
  dimBlendLength = value;
}
uint32_t PatternScroller::getColorBlendLength() { return colorBlendLength; }
void PatternScroller::setColorBlendLength(uint32_t value) {
  if((*curTime > lastColorPatternChange + colorPauseLength) && colorBlendOn) { // Overkill because of random mode
    float blendPerc = colorBlendLength == 0 ? 0 : float(*curTime - lastColorPatternChange - colorPauseLength) / colorBlendLength;
    lastColorPatternChange = *curTime - blendPerc*value - colorPauseLength; // Preserve same blend %
  }
  colorBlendLength = value;
}
uint32_t PatternScroller::getDimPauseLength() { return dimPauseLength; }
void PatternScroller::setDimPauseLength(uint32_t value) {
  if(*curTime >= lastDimPatternChange + dimPauseLength) { if(dimBlendOn) { lastDimPatternChange -= (value - dimPauseLength); } }// if blending already
  else if(*curTime >= lastDimPatternChange + value) { lastDimPatternChange = *curTime - value; } // if paused and reducing pause past the start point
  dimPauseLength = value;
}
uint32_t PatternScroller::getColorPauseLength() { return colorPauseLength; }
void PatternScroller::setColorPauseLength(uint32_t value) {
  if(*curTime >= lastColorPatternChange + colorPauseLength) { if(colorBlendOn) { lastColorPatternChange -= (value - colorPauseLength); } }// if blending already
  else if(*curTime >= lastColorPatternChange + value) { lastColorPatternChange = *curTime - value; } // if paused and reducing pause past the start point
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
    if(*curTime >= lastColorMove + stepSize) { lastColorMove = *curTime - stepSize; }
  }
  colorSpeed = value;
}
uint8_t PatternScroller::getTargetDimPatternIndex() { return isRandomDimPattern() ? randomDimPatternIndex : targetDimPatternIndex; }
uint8_t PatternScroller::getTargetColorPatternIndex() { return isRandomColorPattern() ? randomColorPatternIndex : targetColorPatternIndex; }
uint8_t PatternScroller::getNumBlanks() { return dimPeriod - MIN_SCROLLER_LIT - 2*dimPattern.transLength - dimPattern.brightLength; }
void PatternScroller::setDisplayMode(uint8_t displayMode) {
  if(displayMode >= NUM_DIM_PATTERNS * NUM_COLOR_PATTERNS) { THROW_DUMP("Invalid display mode", displayMode) }
  uint8_t dimPatternIndex = displayMode % NUM_DIM_PATTERNS;
  uint8_t colorPatternIndex = displayMode / NUM_DIM_PATTERNS;

  if(targetColorPatternIndex != colorPatternIndex) {
    bool isTimeForChange = *curTime >= lastColorPatternChange + colorPauseLength;
    if(isRandomColorPattern() && !beginColorBlend) {
      if(isTimeForChange) {
        // Currently on or blending toward randomColorPatternIndex
        targetColorPatternIndex = randomColorPatternIndex;
      }
      else {
        // Have not started blending yet
        targetColorPatternIndex = colorPatternIndex;
        memcpy(targetColorPattern, oldColorPattern, sizeof(CRGB)*numLEDs);
        colorBlendOn = false;
      }
    }
    else if(beginColorBlend || (!colorBlendOn && isTimeForChange)) { //TODO: Change this to actually switch in the middle of a blend
      // Begin blending into new colorPattern; even if in the middle of a blend
      colorBlendOn = true;
      memcpy(oldColorPattern, curColorPattern, sizeof(CRGB)*numLEDs);
      targetColorPatternIndex = colorPatternIndex;
      uint8_t prevPattern = randomColorPatternIndex;
      if(isRandomColorPattern()) { do { randomColorPatternIndex = random8(1, NUM_COLOR_PATTERNS); } while (randomColorPatternIndex == prevPattern); }
      WriteColorPattern(getTargetColorPatternIndex(), targetColorPattern);
      lastColorPatternChange = *curTime - colorPauseLength; // Might already be blending
    }
  }
  beginColorBlend = false;

  if(targetDimPatternIndex != dimPatternIndex) {
    bool isTimeForChange = *curTime >= lastDimPatternChange + dimPauseLength;
    if(isRandomDimPattern() && !beginDimBlend) {
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
    else if(beginDimBlend || (!dimBlendOn && isTimeForChange)) {
      // Don't accept new targets while blending
      dimBlendOn = true;
      oldDimPatternIndex = targetDimPatternIndex;
      memcpy(oldDimPattern, targetDimPattern, dimPeriod);
      targetDimPatternIndex = dimPatternIndex;
      if(isRandomDimPattern()) { do { randomDimPatternIndex = random8(1, NUM_DIM_PATTERNS); } while (randomDimPatternIndex == oldDimPatternIndex); }
      WriteDimPattern(getTargetDimPatternIndex(), targetDimPattern);
      lastDimPatternChange = *curTime - dimPauseLength;
    }
  }
  beginDimBlend = false;
}
void PatternScroller::setDimIndexOffset(uint16_t value) { dimIndexFirst = value; }
uint16_t PatternScroller::getDimIndexOffset() { return dimIndexFirst; }
void PatternScroller::setColorIndexOffset(uint16_t value) { colorIndexFirst = value; }
uint16_t PatternScroller::getColorIndexOffset() { return colorIndexFirst; }

void PatternScroller::setManualBlocks(uint8_t* _colorIndexes, uint8_t _numColorIndexes, uint16_t _dimPeriod) {
  colorPattern.setManualBlocks(_colorIndexes, _numColorIndexes, _dimPeriod);
}
uint8_t* PatternScroller::getManualBlocks() { return colorPattern.getManualBlocks(); }

//*****************************************************************
//******************* Simple boolean functions ********************
//*****************************************************************
bool PatternScroller::isReadyForDimMove() {
  // Returns true if this cycle is going to move the pattern
  if(dimSpeed == 0) { return false; }
  
  uint32_t stepSize = FPS_TO_TIME(abs(dimSpeed));
  if(*curTime >= lastDimMove + stepSize) {
    dimParamWalkedThisCycle = false;
    return true;
  }

  return false;
}
bool PatternScroller::isHalfwayToDimMove() {
  // Only return true once per cycle
  if(dimSpeed == 0) { return false; }
  if(dimParamWalkedThisCycle) { return false; }
  
  uint32_t stepSize = FPS_TO_TIME(abs(dimSpeed));
  if(*curTime >= lastDimMove + stepSize/2) { dimParamWalkedThisCycle = true; }
  return dimParamWalkedThisCycle;
}
bool PatternScroller::isStartOfDimPattern() {
  if(dimSpeed == 0) { return false; }
  else if(isMovingForward()) { return dimIndexFirst == 0; }
  else { return dimIndexFirst == numLEDs % dimPeriod; }
}
bool PatternScroller::isRandomDimPattern() {
  #ifdef DEBUG_COLOR_PATTERNS
    return false;
  #endif
  return targetDimPatternIndex == uint8_t(DimPatternName::Random);
}
bool PatternScroller::isRandomColorPattern() {
  return targetColorPatternIndex == uint8_t(ColorPatternName::Random);
}
bool PatternScroller::isMovingForward() {
  return dimSpeed > 0;
}


//*****************************************************************
//************************ Object overhead ************************
//*****************************************************************
void PatternScroller::Init(struct_scroller_params& params, uint32_t* _curTime, PaletteManager* _pm, GammaManager* _gm, uint16_t _numLEDs) {
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
  WriteColorPattern(getTargetColorPatternIndex(), targetColorPattern);
  memcpy(curColorPattern, targetColorPattern, sizeof(CRGB)*numLEDs);
  memcpy(oldColorPattern, targetColorPattern, sizeof(CRGB)*numLEDs);
  oldDimPatternIndex = getTargetDimPatternIndex();

  uint8_t tempArr = { 0 };
  setManualBlocks(&tempArr, 1, numLEDs);

  lastDimPatternChange = *curTime;
  lastColorPatternChange = *curTime;
  lastDimMove = *curTime;
  lastColorMove = *curTime;
}
void PatternScroller::Clone(PatternScroller* source, struct_scroller_params& params) {
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
  if(*curTime >= colorPauseLength + lastColorPatternChange) {
    if(WalkColorParams()) {
      memcpy(oldColorPattern, curColorPattern, sizeof(CRGB)*numLEDs);
      colorBlendOn = true;
      lastColorPatternChange = *curTime - colorPauseLength; // Might already be blending
    }
  }

  if(colorBlendOn) {
    WriteColorPattern(getTargetColorPatternIndex(), targetColorPattern);
    BlendColorPattern();
  }
  else {
    WriteColorPattern(getTargetColorPatternIndex(), curColorPattern);
  }

  // Dim changing - Always walk params, because they are being slow-walked to their true targets. Pattern changing respects pauseLength.
  // While randomDimPattern, dimBlendOn is always on; even during pauseLength period
  if(WalkDimParams(shiftAmount)) {
    #ifdef JUMP_DIM_PARAMS
      if(!changeDimParamsWithMovement){
        int8_t deltaShift;
        do {
          deltaShift = 0;
          WalkDimParams(deltaShift);
          shiftAmount += deltaShift;
        } while(deltaShift != 0);
      }
    #endif
    WriteDimPattern(getTargetDimPatternIndex(), targetDimPattern);
    WriteDimPattern(oldDimPatternIndex, oldDimPattern);
  }

  if(*curTime >= lastDimPatternChange + dimPauseLength) {
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

  if(colorPattern.colorPeriod != colorPeriod) {
    //THROW_DUMP("colorPeriod mismatch!", colorPattern.colorPeriod)
    //DUMP(colorPeriod)
    updateMade = true;
    colorPattern.colorPeriod = colorPeriod;
  }

  return updateMade;
}

int8_t PatternScroller::GetDimParamDelta() {
  // Using delta = 5/-5 to signal an actual change of 1/-1, but with bright and trans changing in opposite directions
  // Return 4 to signal trans update + 2x bright update in the opposite direction

  switch(baseDimParamChangeType) {
    case BaseDimParamChangeType::Manual:
    {
      int8_t delta = 0;
      if     (dimPattern.brightLength < brightLength) { delta = 1; }
      else if(dimPattern.brightLength > brightLength) { delta = -1; }

      if (dimPattern.transLength < transLength) {
        if(delta == -1) {
          if(enableDoubleBrightMove && (dimPattern.brightLength > brightLength + 1)) { return 4; }
          else { return 5; }
        }
        else { return delta+2; }
      }
      else if(dimPattern.transLength > transLength) {
        if(delta == 1) {
          if(enableDoubleBrightMove && (dimPattern.brightLength + 1 < brightLength)) { return 4; }
          else { return -5; }
        }
        else { return delta-2; }
      }
      else { return delta; }
    }
    case BaseDimParamChangeType::Opposite:
    {
      if(dimPattern.brightLength < brightLength) { return -5; }
      else if(dimPattern.brightLength > brightLength) { return 5; }
      else { return 0; }
    }
    case BaseDimParamChangeType::Matched:
    {
      if(dimPattern.brightLength < brightLength) { return 3; }
      else if(dimPattern.brightLength > brightLength) { return -3; }
      else { return 0; }
    }
    case BaseDimParamChangeType::OppDouble:
    {
      if(dimPattern.brightLength+1 < brightLength) { return 4; }
      else if(dimPattern.brightLength > brightLength+1) { return 4; }
      else { return 0; }
    }
    default:
      THROW_DUMP("Unrecognized baseDimParamChangeType.", int(baseDimParamChangeType))
      return 0;
  }
}

bool PatternScroller::StepDimParams(bool onDownbeat) {
  //If sync'd with movement, and not time for a move, return false.
  if(changeDimParamsWithMovement) {
    if(onDownbeat) { if(!isReadyForDimMove() ) { return false; } }
    else           { if(!isHalfwayToDimMove()) { return false; } }
  }
  
  // Step params this cycle. Delta is known to be non-zero.
  switch(baseDimParamChangeType) {
    case BaseDimParamChangeType::Manual:
    {
      if(dimPattern.brightLength < brightLength) { dimPattern.brightLength++; }
      else if(dimPattern.brightLength > brightLength) { dimPattern.brightLength--; }
      
      if(dimPattern.transLength < transLength) {
        dimPattern.transLength++;
        if(enableDoubleBrightMove && dimPattern.brightLength > brightLength) {
          dimPattern.brightLength--;
        }
      }
      else if(dimPattern.transLength > transLength) {
        dimPattern.transLength--;
        if(enableDoubleBrightMove && dimPattern.brightLength < brightLength) {
          dimPattern.brightLength++;
        }
      }
      return true;
    }
    case BaseDimParamChangeType::Opposite:
    {
      if(dimPattern.brightLength < brightLength) {
        dimPattern.brightLength++;
        dimPattern.transLength = (dimPattern.dimPeriod-MIN_SCROLLER_LIT_PLUS_ONE)/3 - dimPattern.brightLength;
      }
      else if(dimPattern.brightLength > brightLength) {
        dimPattern.brightLength--;
        dimPattern.transLength = (dimPattern.dimPeriod-MIN_SCROLLER_LIT_PLUS_ONE)/3 - dimPattern.brightLength;
      }
      return true;
    }
    case BaseDimParamChangeType::Matched:
    {
      if(dimPattern.brightLength < brightLength) {
        dimPattern.brightLength++;
        dimPattern.transLength = dimPattern.brightLength;
      }
      else if(dimPattern.brightLength > brightLength) {
        dimPattern.brightLength--;
        dimPattern.transLength = dimPattern.brightLength;
      }
      return true;
    }
    case BaseDimParamChangeType::OppDouble:
    {
      if(dimPattern.brightLength+1 < brightLength) {
        dimPattern.brightLength += 2;
        if(dimPattern.transLength > 0) { dimPattern.transLength--; }
      }
      else if(dimPattern.brightLength > brightLength+1) {
        dimPattern.brightLength -= 2;
        if(dimPattern.transLength < (dimPattern.dimPeriod-MIN_SCROLLER_LIT_PLUS_ONE)/3) { dimPattern.transLength++; }
      }
    }
    default:
      THROW_DUMP("Unrecognized baseDimParamChangeType.", int(baseDimParamChangeType))
      return false;
  }
}

bool PatternScroller::WalkDimParams(int8_t& shiftAmount) {
  #define WALK_DIM(downbeat, x) if(StepDimParams(downbeat)) { ScrollPatternsWithoutTimer(x); shiftAmount += x; }
  static bool blendParamsOn = false;
  static DimPatternChangeType changeMode = DimPatternChangeType::Center;
  
  shiftAmount = 0; // Initialize to 0 and then increment/decrement - returns to PatternScroller to affect splitIndex
  
  int8_t delta = GetDimParamDelta();  // Set delta, then do the adjustment and movement accordingly
  
  if(delta == 0) {
    // No changes
    blendParamsOn = false;
    return false;
  }
  else if(delta == 4) {
    // Trading 2 brightLength for 1 transLength
    WALK_DIM(true, 0)
    return true;
  }
  else {
    if(!blendParamsOn) {
      // Lock in the changeMode at the start of the blend
      blendParamsOn = true;
      if(dimPatternChangeType != DimPatternChangeType::Preferred) { changeMode = dimPatternChangeType; }
      else if(dimBlendOn && (*curTime >= lastDimPatternChange + dimPauseLength + dimBlendLength/2)) {
        changeMode = GetPreferredDimPatternChangeType(getTargetDimPatternIndex(), delta);
      }
      else {
        changeMode = GetPreferredDimPatternChangeType(oldDimPatternIndex, delta);
      }
    }

    switch(changeMode)
    {
      case DimPatternChangeType::Grow_F:
        switch(delta) {
          case 5:
          case 1:  if(isMovingForward()) { WALK_DIM(false,0) } else { WALK_DIM(false,-1) } break;
          case -5:
          case -1: if(isMovingForward()) { WALK_DIM(true,0)  } else { WALK_DIM(true,1)   } break;
          case 2:  if(isMovingForward()) { WALK_DIM(false,0) } else { WALK_DIM(false,-2) } break;
          case -2: if(isMovingForward()) { WALK_DIM(true,0)  } else { WALK_DIM(true,2)   } break;
          case 3:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,-1)  } break;
          case -3: if(isMovingForward()) { WALK_DIM(true,2)  } else { WALK_DIM(true,1)   } break;
          default: DUMP(delta)
        }
        break;
      case DimPatternChangeType::Grow_R:
        switch(delta) {
          case 5:
          case 1:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,0)  } break;
          case -5:
          case -1: if(isMovingForward()) { WALK_DIM(false,1) } else { WALK_DIM(false,0) } break;
          case 2:  if(isMovingForward()) { WALK_DIM(true,-2) } else { WALK_DIM(true,0)  } break;
          case -2: if(isMovingForward()) { WALK_DIM(false,2) } else { WALK_DIM(false,0) } break;
          case 3:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,-2) } break;
          case -3: if(isMovingForward()) { WALK_DIM(true,2)  } else { WALK_DIM(true,2)  } break;
          default: DUMP(delta)
        }
        break;
      case DimPatternChangeType::Worm:
        switch(delta) {
          case 5:
          case 1:  if(isMovingForward()) { WALK_DIM(false,0) } else { WALK_DIM(false,-1) } break;
          case -5:
          case -1: if(isMovingForward()) { WALK_DIM(false,1) } else { WALK_DIM(false,0)  } break;
          case 2:  if(isMovingForward()) { WALK_DIM(false,0) } else { WALK_DIM(false,-2) } break;
          case -2: if(isMovingForward()) { WALK_DIM(false,2) } else { WALK_DIM(false,0)  } break;
          case 3:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,-2)  } break;
          case -3: if(isMovingForward()) { WALK_DIM(true,2)  } else { WALK_DIM(true,1)   } break;
          default: DUMP(delta)
        }
        break;
      case DimPatternChangeType::Freeze:
        switch(delta) {
          case 5:
          case 1:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,0)  } break;
          case -5:
          case -1: if(isMovingForward()) { WALK_DIM(true,0)  } else { WALK_DIM(true,1)  } break;
          case 2:  if(isMovingForward()) { WALK_DIM(true,-2) } else { WALK_DIM(true,0)  } break;
          case -2: if(isMovingForward()) { WALK_DIM(true,0)  } else { WALK_DIM(true,2)  } break;
          case 3:  if(isMovingForward()) { WALK_DIM(true,-2) } else { WALK_DIM(true,-1) } break;
          case -3: if(isMovingForward()) { WALK_DIM(true,1)  } else { WALK_DIM(true,2)  } break;
          default: DUMP(delta)
        }
        break;
      case DimPatternChangeType::Center:
        switch(delta) {
          case 5:
          case 1:  if(dimPattern.brightLength % 2 == 0) { WALK_DIM(!isMovingForward(),0) } else { WALK_DIM(isMovingForward(),-1) } break;
          case -5:
          case -1: if(dimPattern.brightLength % 2 == 0) { WALK_DIM(!isMovingForward(),1) } else { WALK_DIM(isMovingForward(),0)  } break;
          case 2:  if(isMovingForward())                { WALK_DIM(true,-1) } else { WALK_DIM(true,-1) } break;
          case -2: if(isMovingForward())                { WALK_DIM(true,1)  } else { WALK_DIM(true,1)  } break;
          case 3:  if(dimPattern.brightLength % 2 == 0) { WALK_DIM(true,-1) } else { WALK_DIM(true,-2) } break;
          case -3: if(dimPattern.brightLength % 2 == 0) { WALK_DIM(true,2)  } else { WALK_DIM(true,1)  } break;
          default: DUMP(delta)
        }
        break;
      case DimPatternChangeType::Mix_F://worm on 1, center on 2
        switch(delta) {
          case 5:
          case 1:  if(isMovingForward()) { WALK_DIM(false,0) } else { WALK_DIM(false,-1) } break;
          case -5:
          case -1: if(isMovingForward()) { WALK_DIM(false,1) } else { WALK_DIM(false,0)  } break;
          case 2:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,-1)  } break;
          case -2: if(isMovingForward()) { WALK_DIM(true,1)  } else { WALK_DIM(true,1)   } break;
          case 3:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,-2)  } break;
          case -3: if(isMovingForward()) { WALK_DIM(true,2)  } else { WALK_DIM(true,1)   } break;
          default: DUMP(delta)
        }
        break;
      case DimPatternChangeType::Mix_R: // freeze on 1, center on 2
        switch(delta) {
          case 5:
          case 1:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,0)  } break;
          case -5:
          case -1: if(isMovingForward()) { WALK_DIM(true,0)  } else { WALK_DIM(true,1)  } break;
          case 2:  if(isMovingForward()) { WALK_DIM(true,-1) } else { WALK_DIM(true,-1) } break;
          case -2: if(isMovingForward()) { WALK_DIM(true,1)  } else { WALK_DIM(true,1)  } break;
          case 3:  if(isMovingForward()) { WALK_DIM(true,-2) } else { WALK_DIM(true,-1) } break;
          case -3: if(isMovingForward()) { WALK_DIM(true,1)  } else { WALK_DIM(true,2)  } break;
          default: DUMP(delta)
        }
        break;
      default:
        THROW_DUMP("unknown DimPatternChangeType!", int(dimPatternChangeType))
    }
  }

  ASSERT_EQUAL(dimPattern.dimPeriod, dimPeriod)
  return delta != 0;
}

bool PatternScroller::ScrollPatterns() {
  //uint32_t initCurTime = *curTime;

  bool dimMoved = false;
  // Move dim pattern
  if(dimSpeed == 0) {
    lastDimMove = *curTime;
  }
  else if(isReadyForDimMove()) {
    dimMoved = true;

    // Scroll dim pattern
    if(isMovingForward()) {
      if(--dimIndexFirst == 0xFFFF) { dimIndexFirst = dimPeriod - 1; }
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
  else if(*curTime >= lastColorMove + FPS_TO_TIME(abs(colorSpeed))) {
    if(colorSpeed > 0) {
      if(--colorIndexFirst == 0xFFFF) { colorIndexFirst = numLEDs - 1; }
    }
    else {
      if(++colorIndexFirst == numLEDs) { colorIndexFirst = 0; }
    }
    
    lastColorMove += FPS_TO_TIME(abs(colorSpeed));
  }

  return dimMoved;
}
void PatternScroller::ScrollPatternsWithoutTimer(int8_t moveAmount) {
  if(moveAmount > 0) {
    for(int i = 0; i < moveAmount; i++) {
      if(--dimIndexFirst == 0xFFFF) { dimIndexFirst = dimPeriod - 1; }
    }
  }
  else {
    for(int i = 0; i > moveAmount; i--) {
      if(++dimIndexFirst == dimPeriod) { dimIndexFirst = 0; }
    }
  }
}

void PatternScroller::BeginDimBlend() {
  beginDimBlend = true;
  lastDimPatternChange = *curTime - dimPauseLength;
}

void PatternScroller::BeginColorBlend() {
  beginColorBlend = true;
  lastColorPatternChange = *curTime - colorPauseLength;
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
    
    ASSERT_LESSTHAN(curDimIndex, dimPeriod)
    ASSERT_LESSTHAN(curColorIndex, numLEDs)

    if(++curColorIndex == numLEDs) { curColorIndex = 0; }
    if(++curDimIndex == dimPeriod) { curDimIndex = 0; }
  }
}
void PatternScroller::BlendColorPattern() {
  uint32_t transitionTime = *curTime - lastColorPatternChange;
  if(transitionTime < colorPauseLength) { return; }
  if(transitionTime < colorPauseLength + colorBlendLength) {
    float blendAmount = 255.0 * float(transitionTime - colorPauseLength) / colorBlendLength;

    for(uint16_t i = 0; i < numLEDs; i++) {
      curColorPattern[i] = Gamma->Blend(oldColorPattern[i], targetColorPattern[i], blendAmount);
    }
  }
  else {
    // Blending just finished
    if(isRandomColorPattern()) {
      memcpy(oldColorPattern, curColorPattern, sizeof(CRGB)*numLEDs);
      uint8_t previousPattern = randomColorPatternIndex;
      do { randomColorPatternIndex = random8(1, NUM_COLOR_PATTERNS); } while (randomColorPatternIndex == previousPattern);
      WriteColorPattern(randomColorPatternIndex, targetColorPattern);
    }
    else {
      memcpy(oldColorPattern, targetColorPattern, sizeof(CRGB)*numLEDs);
      memcpy(curColorPattern, targetColorPattern, sizeof(CRGB)*numLEDs);
      colorBlendOn = false;
      WriteColorPattern(getTargetColorPatternIndex(), targetColorPattern); // It's possible that this isn't needed
    }

    lastColorPatternChange = *curTime;
  }
}
void PatternScroller::BlendDimPattern() {
  if(*curTime < dimBlendLength + lastDimPatternChange + dimPauseLength) {
    float blendAmount = float(*curTime - lastDimPatternChange - dimPauseLength) / dimBlendLength;
    for(uint16_t i = 0; i < dimPeriod; i++) {
      curDimPattern[i] = (1 - blendAmount) * oldDimPattern[i] + blendAmount * targetDimPattern[i];
    }
  }
  else {
    // Blending just finished
    if(isRandomDimPattern()) {
      oldDimPatternIndex = randomDimPatternIndex;
      do { randomDimPatternIndex = random8(1, NUM_DIM_PATTERNS); } while (randomDimPatternIndex == oldDimPatternIndex);
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

DimPatternChangeType PatternScroller::GetPreferredDimPatternChangeType(uint8_t patternIndex, int8_t delta) {
  // delta can be +/- 5. If so, the actual change was +/- 1, but both bright and trans were updated
  
  PatternType patternType = dimPattern.getPatternType(DimPatternName(patternIndex));
  switch(patternType) {
    case PatternType::Symmetric:
      //return (!changeDimParamsWithMovement || abs(delta)==2) ? DimPatternChangeType::Center : DimPatternChangeType::Worm;
    case PatternType::AllCenter:
      return DimPatternChangeType::Center;
    case PatternType::Front:
      return isMovingForward() ? DimPatternChangeType::Grow_R : DimPatternChangeType::Grow_F;
    case PatternType::Reverse:
      return isMovingForward() ? DimPatternChangeType::Grow_F : DimPatternChangeType::Grow_R;
    case PatternType::Front3:
      return abs(delta)==5 ? DimPatternChangeType::Worm : abs(delta)==2 ? DimPatternChangeType::Center : (isMovingForward() ? DimPatternChangeType::Worm : DimPatternChangeType::Grow_F); 
    case PatternType::Reverse3:
      return abs(delta)==5 ? DimPatternChangeType::Worm : abs(delta)==2 ? DimPatternChangeType::Center : (isMovingForward() ? DimPatternChangeType::Grow_R : DimPatternChangeType::Worm);
    case PatternType::MixR:
      return isMovingForward() ? DimPatternChangeType::Mix_R : DimPatternChangeType::Mix_F;
    default:
      THROW_DUMP(F("Invalid dimPattern used"), int(patternType))
  }
  
  return delta == 1 ? DimPatternChangeType::Worm : DimPatternChangeType::Center;
}

void PatternScroller::WriteDimPattern(uint8_t patternIndex, uint8_t* outputArray) {
  // All patterns have a length (excluding spacing) of this
  uint8_t brightPeriod = 2*dimPattern.transLength + dimPattern.brightLength + MIN_SCROLLER_LIT;
  if(brightPeriod > dimPattern.dimPeriod) { THROW_DUMP("Out of bounds dimPeriod", brightPeriod) }
  dimPattern.Draw(DimPatternName(patternIndex), outputArray);
  //for(uint8_t i =0; i < dimPeriod; i++) { Serial.println(String(i) + ": " + outputArray[i]);}
}

void PatternScroller::WriteColorPattern(uint8_t patternIndex, CRGB* outputArray) {
  for(int i = 0; i < numLEDs; i += colorPattern.colorPeriod) {
    colorPattern.Draw(ColorPatternName(patternIndex), outputArray + i);
  }
  //for(uint8_t i =0; i < colorPeriod; i++) { Serial.println(String(i) + ": (" + outputArray[i].r + ", " + outputArray[i].g + ", " + outputArray[i].b + ")"); }
}

void PatternScroller::SyncLastMovedTimes() { lastColorMove = lastDimMove; }