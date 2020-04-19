#include "PatternScrolling\PatternController.h"
#include "Util.h"

uint8_t PatternController::getBrightness() { return ps1.brightness; }
void PatternController::setBrightness(uint8_t brightness) { ps1.brightness = ps2.brightness = brightness; }
uint32_t PatternController::getColorPauseLength() { return ps1.getColorPauseLength(); }
void PatternController::setColorPauseLength(uint32_t value) { ps1.setColorPauseLength(value); ps2.setColorPauseLength(value); }
uint32_t PatternController::getDimPauseLength() { return ps1.getDimPauseLength(); }
void PatternController::setDimPauseLength(uint32_t value) { ps1.setDimPauseLength(value); ps2.setDimPauseLength(value); }
uint32_t PatternController::getColorBlendLength() { return ps1.getColorBlendLength(); }
void PatternController::setColorBlendLength(uint32_t value) { ps1.setColorBlendLength(value); ps2.setColorBlendLength(value); }
uint32_t PatternController::getDimBlendLength() { return ps1.getDimBlendLength(); }
void PatternController::setDimBlendLength(uint32_t value) { ps1.setDimBlendLength(value); ps2.setDimBlendLength(value); }
DimPatternChangeType PatternController::getDimPatternChangeType() { return ps1.dimPatternChangeType; }

bool PatternController::getEnableDoubleBrightMove() { return ps1.enableDoubleBrightMove; }
void PatternController::setEnableDoubleBrightMove(bool value) { ps1.enableDoubleBrightMove = ps2.enableDoubleBrightMove = value; }
bool PatternController::getChangeDimParamsWithMovement() { return ps1.changeDimParamsWithMovement; }
void PatternController::setBaseDimParamChangeType(BaseDimParamChangeType value) { ps1.baseDimParamChangeType = ps2.baseDimParamChangeType = value; }
void PatternController::setDimPatternChangeType(DimPatternChangeType value) { setDimPatternChangeType(value, ps1.changeDimParamsWithMovement); }
void PatternController::setDimPatternChangeType(bool changeDimParamsWithMovement) { setDimPatternChangeType(ps1.dimPatternChangeType, changeDimParamsWithMovement); }
void PatternController::setDimPatternChangeType(DimPatternChangeType value, bool changeDimParamsWithMovement) {
  ps1.dimPatternChangeType = value;
  ps1.changeDimParamsWithMovement = changeDimParamsWithMovement;
  ps2.dimPatternChangeType = value;
  ps2.changeDimParamsWithMovement = changeDimParamsWithMovement;
}

void PatternController::Init(uint16_t _numLEDs, uint32_t* curTime, struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, const uint16_t* _allowedDimPeriods, const uint16_t* _allowedColorPeriods) {
  numLEDs = _numLEDs;
  allowedDimPeriods = _allowedDimPeriods;
  allowedColorPeriods = _allowedColorPeriods;

  struct_base_show_params scaledParams;
  ScaleParams(params, scaledParams);

  dimSpeed = scaledParams.dimSpeed;
  colorSpeed = scaledParams.colorSpeed;
  ps1.Init(scaledParams, curTime, pm, gm, numLEDs);
  ps2.Init(scaledParams, curTime, pm, gm, numLEDs);

  // Init PatternScrollers
  setDimBlendLength(INIT_PATTERN_CONTROLLER_DIM_BLEND_LENGTH);
  setColorBlendLength(INIT_PATTERN_CONTROLLER_COLOR_BLEND_LENGTH);
  setDimPauseLength(INIT_PATTERN_CONTROLLER_DIM_PAUSE_LENGTH);
  setColorPauseLength(INIT_PATTERN_CONTROLLER_COLOR_PAUSE_LENGTH);
  setBrightness(INIT_PATTERN_SCROLLER_BRIGHTNESS);
  
  setEnableDoubleBrightMove(INIT_ENABLE_DOUBLE_BRIGHT_MOVE);
  setDimPatternChangeType(INIT_DIM_PARAM_CHANGE_TYPE, INIT_CHANGE_DIM_PARAMS_WITH_MOVEMENT);
  setBaseDimParamChangeType(INIT_BASE_DIM_PARAM_CHANGE_TYPE);

  ps = &ps1;
  secondary = &ps2;
}

void PatternController::SkipTime(uint32_t amount) {
  ps1.SkipTime(amount);
  ps2.SkipTime(amount);
}

void PatternController::Update(struct_base_show_params& params, CRGB* target, uint8_t* target_b) {
  struct_base_show_params scaledParams;
  ScaleParams(params, scaledParams);
  
  dimSpeed = scaledParams.dimSpeed;
  colorSpeed = scaledParams.colorSpeed;
  WalkSpeeds();

  // Check for changes in dimPeriod and colorPeriod. If so, scaling will be off because its based on period.
  // So correct param scaling for old periods and be ready to start splitting to bring in the new pattern.
  if(ps->getDimPeriod() != scaledParams.dimPeriod || ps->getColorPeriod() != scaledParams.colorPeriod) {
    if(!splitDisplay && dimSpeed != 0 && ps->isStartOfDimPattern() && ps->isReadyForDimMove()) {
      StartSplit(scaledParams);
    }
    ScaleParams(params, scaledParams, ps->getDimPeriod(), ps->getColorPeriod()); // Re-scale params with old periods
  }

  if(ps->getDimSpeed() != secondary->getDimSpeed()) { THROW(ps->getDimSpeed() + " DOES NOT EQUAL " + secondary->getDimSpeed()); }

  // Update primary PatternScroller
  ps->numColors = scaledParams.numColors;
  ps->brightLength = scaledParams.brightLength;
  ps->transLength = scaledParams.transLength;
  ps->setDisplayMode(scaledParams.displayMode);
  int8_t dummy = 0;
  bool psMoved = ps->Update(dummy);

  if(!splitDisplay) {
    ps->SetCRGBs(target, target_b, numLEDs);
  }
  else {
    // Update secondary PatternScroller, using its existing periods
    ScaleParams(params, scaledParams, secondary->getDimPeriod(), secondary->getColorPeriod());
    secondary->numColors = scaledParams.numColors;
    secondary->brightLength = scaledParams.brightLength;
    secondary->transLength = scaledParams.transLength;
    secondary->setDisplayMode(scaledParams.displayMode);
    int8_t shiftAmount = 0;
    bool secMoved = secondary->Update(shiftAmount);

    // Scroll split point (must be before drawing)
    // splitIndex is part of the higher pattern
    // Always move the split point with the dim pattern, even if a change in colorPeriod is being applied
    if(psMoved) {
      if(!secMoved) { THROW(F("ps moved and secondary didn't")) }
      if(ps->getDimSpeed() > 0) { splitIndex++; }
      else if(ps->getDimSpeed() < 0) { splitIndex--; }
      else { THROW(F("Error: dimMovedLastUpdate==true while dimSpeed == 0")) }
    }
    else if(secMoved) { THROW(F("Error: secondary moved and ps didn't")) }

    // Handle splitPoint movement from paramChanges; determined by the amount it shifted in secondary pattern
    splitIndex += shiftAmount;
    
    if(!secondaryScrollerIsLow)  {
      if(splitIndex >= numLEDs) { splitIndex = numLEDs; EndSplit(false); }
      else if(splitIndex <= 0) { splitIndex = numLEDs; EndSplit(true); } // Would be splitIndex==0, but just draw primary after EndSplit()

      secondary->SetCRGBs(target, target_b, numLEDs, splitIndex);
      ps->SetCRGBs(target, target_b, splitIndex);
    }
    else {
      // Treat the final empty pixels of the secondary pattern as transparent; don't end split until they clear
      uint16_t ledsToSkip = splitIndex;
      uint8_t transparent = secondary->getNumBlanks();
      if(splitIndex >= numLEDs + transparent) { ledsToSkip = 0; EndSplit(true); } // Would be splitIndex==numLEDs, but just draw primary after EndSplit()
      else if(splitIndex <= 0) { ledsToSkip = 0; EndSplit(false); }
      else { ledsToSkip = transparent >= ledsToSkip ? 0 : ledsToSkip-transparent; }

      ps->SetCRGBs(target, target_b, numLEDs, ledsToSkip);
      secondary->SetCRGBs(target, target_b, ledsToSkip);
    }
  }
}

void PatternController::ScaleParams(struct_base_show_params& params, struct_base_show_params& output, uint8_t dim_period, uint8_t color_period) {
  // You can optionally call this function with a specific dim_period and color_period, to scale params accordingly even when struct has a new period in it
  if(dim_period == 0) {
    #ifdef EXPLICIT_PARAMETERS
      output.dimPeriod = params.dimPeriod;
    #else
      output.dimPeriod = allowedDimPeriods[scaleParam(params.dimPeriod, 0, NUM_ALLOWED_DIM_PERIODS-1)];
    #endif
  }
  else {
    output.dimPeriod = dim_period;
  }
  
  if(color_period == 0) {
    #ifdef EXPLICIT_PARAMETERS
      output.colorPeriod = params.colorPeriod;
    #else
      output.colorPeriod = allowedColorPeriods[scaleParam(params.colorPeriod, 0, NUM_ALLOWED_COLOR_PERIODS-1)];
    #endif
  }
  else {
    output.colorPeriod = color_period;
  }
  
  #ifdef EXPLICIT_PARAMETERS
    output.displayMode = params.displayMode;
    output.numColors = params.numColors;
    output.transLength = params.transLength;
    output.brightLength = params.brightLength;
    output.dimSpeed = params.dimSpeed;
    output.colorSpeed = params.colorSpeed;
  #else
    #ifdef ALLOW_ZERO_SPEED
      uint8_t abs_dimSpeed = scaleParam((uint8_t)abs(params.dimSpeed), 0, 127);
    #else
      uint8_t abs_dimSpeed = scaleParam((uint8_t)abs(params.dimSpeed), 20, 127);
    #endif
    output.dimSpeed = abs_dimSpeed * (params.dimSpeed >= 0 ? 1 : -1);

    #if 0
    // Bound colorSpeed based on dimSpeed
    int8_t colorSpeed_lower, colorSpeed_upper;
    if(output.dimSpeed > 0) {
      colorSpeed_lower = dimSpeed / -4;
      colorSpeed_upper = dimSpeed * 3/2;
    }
    else {
      colorSpeed_upper = dimSpeed * 3/2;
      colorSpeed_upper = dimSpeed / -4;
    }
    #endif
    
    output.colorSpeed = output.dimSpeed/2;//scaleParam(params.colorSpeed, colorSpeed_lower, colorSpeed_upper);
    output.displayMode = scaleParam(params.displayMode, 0, NUM_DIM_PATTERNS * NUM_COLOR_PATTERNS - 1);
    output.numColors = scaleParam(params.numColors, 2, PALETTE_SIZE-1);

    #ifdef PAD_PATTERN_PIXELS
      uint8_t extraPixels = (output.dimPeriod-10) % 3;
      uint8_t transUpperLimit =  (output.dimPeriod-10)/3 + (extraPixels==2 ? 1 : 0);
      uint8_t brightUpperLimit = (output.dimPeriod-10)/3 + (extraPixels==1 ? 1 : 0);
    #else
      uint8_t transUpperLimit =  (output.dimPeriod-10)/3;
      uint8_t brightUpperLimit = (output.dimPeriod-10)/3;
    #endif

    output.transLength =  scaleParam(params.transLength,  0, transUpperLimit);
    output.brightLength = scaleParam(params.brightLength, 0, brightUpperLimit);
    //DEBUG(params.brightLength+"/"+output.brightLength + ", " + params.transLength+"/"+output.transLength);
  #endif
}

void PatternController::StartSplit(struct_base_show_params& params) {
  DEBUG_PATTERN_SPLITS("Start Split");
  splitDisplay = true;
  splitIndex = ps->getDimSpeed() > 0 ? 0 : numLEDs;
  secondary->Clone(ps, params);
  secondaryScrollerIsLow = ps->getDimSpeed() > 0;
}

void PatternController::EndSplit(bool swap) {
  splitDisplay = false;
  if(swap) {
    PatternScroller* swap = ps;
    ps = secondary;
    secondary = swap;
    secondaryScrollerIsLow = !secondaryScrollerIsLow;
    DEBUG_PATTERN_SPLITS("SWAP!");
  }
  DEBUG_PATTERN_SPLITS("End Split");
}

void PatternController::WalkSpeeds() {
  // Perform everything on ps1, then copy to ps2. They should always match to be fully sync'd in movements

  #ifdef EXPLICIT_PARAMETERS
    ps1.setDimSpeed(dimSpeed);
    ps1.setColorSpeed(colorSpeed);
  #else
    // Gradually update speeds in the PatternScrollers; slow down at lower levels
    int8_t initSpeed = ps1.getDimSpeed();
    if(initSpeed != dimSpeed) {
      uint8_t absSpeed = abs(initSpeed);
      if(absSpeed < 5) {
        // From 10% chance to 4%
        if(random8(50) < absSpeed+2) {
          if(initSpeed < dimSpeed) { ps1.setDimSpeed(initSpeed+1); }
          else { ps1.setDimSpeed(initSpeed-1); }
        }
      }
      else {
        // From 33% chance to 10%
        if(random16(530) < absSpeed+48) {
          if(initSpeed < dimSpeed) { ps1.setDimSpeed(initSpeed+1); }
          else { ps1.setDimSpeed(initSpeed-1); }
        }
      }
    }

    initSpeed = ps1.getColorSpeed();
    if(initSpeed != colorSpeed) {
      uint8_t absSpeed = abs(initSpeed);
      if(absSpeed < 5) {
        // From 10% chance to 4%
        if(random8(50) <= absSpeed+2) {
          if(initSpeed < colorSpeed) { ps1.setColorSpeed(initSpeed+1); }
          else { ps1.setColorSpeed(initSpeed-1); }
        }
      }
      else {
        // From 33% chance to 10%
        if(random16(530) < absSpeed+48) {
          if(initSpeed < colorSpeed) { ps1.setColorSpeed(initSpeed+1); }
          else { ps1.setColorSpeed(initSpeed-1); }
        }
      }
    }
  #endif

  // Always match speeds
  ps2.setDimSpeed(ps1.getDimSpeed());
  ps2.setColorSpeed(ps1.getColorSpeed());
}
