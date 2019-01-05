#include "PatternController.h"

PatternController::PatternController() {
}

uint32_t PatternController::GetColorPauseLength() {
  return ps1.GetColorPauseLength();
}
void PatternController::SetColorPauseLength(uint32_t value, uint32_t curTime) {
  ps1.SetColorPauseLength(value, curTime);
  ps2.SetColorPauseLength(value, curTime);
}

uint32_t PatternController::GetDimPauseLength() {
  return ps1.GetDimPauseLength();
}
void PatternController::SetDimPauseLength(uint32_t value, uint32_t curTime) {
  ps1.SetDimPauseLength(value, curTime);
  ps2.SetDimPauseLength(value, curTime);
}

uint32_t PatternController::GetColorBlendLength() {
  return ps1.GetColorBlendLength();
}
void PatternController::SetColorBlendLength(uint32_t value) {
  ps1.SetColorBlendLength(value);
  ps2.SetColorBlendLength(value);
}

uint32_t PatternController::GetDimBlendLength() {
  return ps1.GetDimBlendLength();
}
void PatternController::SetDimBlendLength(uint32_t value) {
  ps1.SetDimBlendLength(value);
  ps2.SetDimBlendLength(value);
}

param_change_type PatternController::GetDimParamChangeType() {
  return ps1.dimParamChangeType;
}
bool PatternController::GetChangeDimParamsWithMovement() {
  return ps1.changeDimParamsWithMovement;
}
void PatternController::SetDimParamChangeType(param_change_type value) {
  SetDimParamChangeType(value, ps1.changeDimParamsWithMovement);
}
void PatternController::SetDimParamChangeType(bool changeDimParamsWithMovement) {
  SetDimParamChangeType(ps1.dimParamChangeType, changeDimParamsWithMovement);
}
void PatternController::SetDimParamChangeType(param_change_type value, bool changeDimParamsWithMovement) {
  ps1.dimParamChangeType = value;
  ps1.changeDimParamsWithMovement = changeDimParamsWithMovement;
  ps2.dimParamChangeType = value;
  ps2.changeDimParamsWithMovement = changeDimParamsWithMovement;
}
bool PatternController::GetEnableDoubleBrightMove() { return ps1.enableDoubleBrightMove; }
void PatternController::SetEnableDoubleBrightMove(bool value) { ps1.enableDoubleBrightMove = value; ps2.enableDoubleBrightMove = value; }

void PatternController::SetBrightness(uint8_t brightness) {
  ps1.brightness = brightness;
  ps2.brightness = brightness;
}

void PatternController::Init(uint16_t _numLEDs, struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, const uint16_t* _allowedDimPeriods, const uint16_t* _allowedColorPeriods, uint32_t curTime) {
  // Init PatternRepeaters
  SetDimBlendLength(INIT_PATTERN_CONTROLLER_DIM_BLEND_LENGTH);
  SetColorBlendLength(INIT_PATTERN_CONTROLLER_COLOR_BLEND_LENGTH);
  SetDimPauseLength(INIT_PATTERN_CONTROLLER_DIM_PAUSE_LENGTH, curTime);
  SetColorPauseLength(INIT_PATTERN_CONTROLLER_COLOR_PAUSE_LENGTH, curTime);
  SetDimParamChangeType(INIT_DIM_PARAM_CHANGE_TYPE, INIT_CHANGE_DIM_PARAMS_WITH_MOVEMENT);
  SetBrightness(INIT_PATTERN_SCROLLER_BRIGHTNESS);
  SetEnableDoubleBrightMove(INIT_ENABLE_DOUBLE_BRIGHT_MOVE);

  numLEDs = _numLEDs;
  allowedDimPeriods = _allowedDimPeriods;
  allowedColorPeriods = _allowedColorPeriods;

  struct_base_show_params scaledParams;
  ScaleParams(params, scaledParams);

  dimSpeed = scaledParams.dimSpeed;
  colorSpeed = scaledParams.colorSpeed;
  ps1.Init(scaledParams, curTime, pm, gm, numLEDs);
  ps2.Init(scaledParams, curTime, pm, gm, numLEDs);

  ps = &ps1;
  secondary = &ps2;
}

void PatternController::SkipTime(uint32_t amount) {
  ps1.SkipTime(amount);
  ps2.SkipTime(amount);
}

void PatternController::Update(struct_base_show_params& params, CRGB* target, uint8_t* target_b, uint32_t curTime) {
  struct_base_show_params scaledParams;
  ScaleParams(params, scaledParams);
  
  dimSpeed = scaledParams.dimSpeed;
  colorSpeed = scaledParams.colorSpeed;
  WalkSpeeds(curTime);

  // Check for changes in dimPeriod and colorPeriod. If so, correct param scaling for old periods and be ready to start splitting to bring in the new pattern
  if(ps->GetDimPeriod() != scaledParams.dimPeriod || ps->GetColorPeriod() != scaledParams.colorPeriod) {
    if(!splitDisplay && dimSpeed != 0 && ps->IsStartOfDimPattern()) {
      StartSplit(scaledParams, curTime);
    }
    ScaleParams(params, scaledParams, ps->GetDimPeriod(), ps->GetColorPeriod()); // Re-scale params with old periods
  }

  if(ps->GetDimSpeed() != secondary->GetDimSpeed()) { THROW(String(ps->GetDimSpeed()) + " DOES NOT EQUAL " + String(secondary->GetDimSpeed())); }

  // Update primary PatternScroller
  ps->numColors = scaledParams.numColors;
  //ps->colorPeriod = scaledParams.colorPeriod;
  ps->brightLength = scaledParams.brightLength;
  ps->transLength = scaledParams.transLength;
  ps->SetDisplayMode(scaledParams.displayMode, curTime);
  bool psMoved = ps->Update(curTime);

  if(!splitDisplay) {
    ps->SetCRGBs(target, target_b, numLEDs);
  }
  else {
    // Update secondary PatternScroller, using its existing periods
    ScaleParams(params, scaledParams, secondary->GetDimPeriod(), secondary->GetColorPeriod());
    secondary->numColors = scaledParams.numColors;
    //secondary->colorPeriod = scaledParams.colorPeriod; // Don't update colorPeriod if already splitting
    secondary->brightLength = scaledParams.brightLength;
    secondary->transLength = scaledParams.transLength;
    secondary->SetDisplayMode(scaledParams.displayMode, curTime);
    bool secMoved = secondary->Update(curTime);

    if(secondaryScrollerIsLow) {
      //ps->SetCRGBs(&target[splitIndex], &target_b[splitIndex], numLEDs - splitIndex);
      ps->SetCRGBs(target, target_b, numLEDs); // debug: this is wasteful but simple. Otherwise pattern will move 2 pixels per move. Just overwrite the pixels in next line
      secondary->SetCRGBs(target, target_b, splitIndex);
    }
    else {
      //secondary->SetCRGBs(&target[splitIndex], &target_b[splitIndex], numLEDs - splitIndex);
      secondary->SetCRGBs(target, target_b, numLEDs); // debug: this is wasteful but simple. Otherwise pattern will move 2 pixels per move. Just overwrite the pixels in next line
      ps->SetCRGBs(target, target_b, splitIndex);
    }

    // Always move the split point with the dim pattern, even if a change in colorPeriod is being applied
    if(psMoved) {
      if(!secMoved) { THROW("ps moved and secondary didn't") }
      
      if(ps->GetDimSpeed() > 0) {
        splitIndex++;
        if(splitIndex == numLEDs) { EndSplit(); }
      }
      else if(ps->GetDimSpeed() < 0) {
        splitIndex--;
        if(splitIndex == 0) { EndSplit(); }
      }
      else {
        THROW("Error: dimMovedLastUpdate==true while dimSpeed == 0")
      }
    }
    else if(secMoved) { THROW("Error: secondary moved and ps didn't") }
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
    uint8_t abs_dimSpeed = scaleParam((uint8_t)abs(params.dimSpeed), 20, 127);
    output.dimSpeed = abs_dimSpeed * (params.dimSpeed >= 0 ? 1 : -1);
    
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
    
    output.colorSpeed = output.dimSpeed/2;//scaleParam(params.colorSpeed, colorSpeed_lower, colorSpeed_upper);
    output.displayMode = scaleParam(params.displayMode, 0, (NUM_DIM_PATTERNS+1) * NUM_COLOR_PATTERNS - 1);
    output.numColors = scaleParam(params.numColors, 2, PALETTE_SIZE-1);
    output.transLength = scaleParam(params.transLength, 0, (output.dimPeriod-10) / 3);
    output.brightLength = scaleParam(params.brightLength, 0, (output.dimPeriod-10) / 3);
    //Serial.println(String(params.brightLength)+"/"+String(output.brightLength) + ", " + String(params.transLength)+"/"+String(output.transLength));
  #endif
}

void PatternController::StartSplit(struct_base_show_params& params, uint32_t curTime) {
  Serial.println("Start Split");
  splitDisplay = true;
  splitIndex = ps->GetDimSpeed() > 0 ? 0 : numLEDs-1;

  secondary->Clone(ps, params, curTime);
  //secondary->Init(params, curTime);
  //secondary->SyncMovement(ps);

  secondaryScrollerIsLow = ps->GetDimSpeed() > 0;
}

void PatternController::EndSplit() {
  splitDisplay = false;
  if(ps->GetDimSpeed() > 0 && secondaryScrollerIsLow || ps->GetDimSpeed() < 0 && !secondaryScrollerIsLow) {
    PatternScroller* swap = ps;
    ps = secondary;
    secondary = swap;
    Serial.println("SWAP!");
  }
  Serial.println("End Split");
}

void PatternController::WalkSpeeds(uint32_t curTime) {
// Perform everything on ps1, then copy to ps2. They should always match to be fully sync'd in movements

  #ifdef EXPLICIT_PARAMETERS
    ps1.SetDimSpeed(dimSpeed, curTime);
    ps1.SetColorSpeed(colorSpeed, curTime);
  #else
    // Gradually update speeds even if not ready for a pattern change; slow down at lower levels
    int8_t initSpeed = ps1.GetDimSpeed();
    if(initSpeed != dimSpeed) {
      uint8_t absSpeed = abs(initSpeed);
      if(absSpeed < 5) {
        // From 10% chance to 4%
        if(random8(50) < absSpeed+2) {
          if(initSpeed < dimSpeed) { ps1.SetDimSpeed(initSpeed+1, curTime); }
          else { ps1.SetDimSpeed(initSpeed-1, curTime); }
        }
      }
      else {
        // From 33% chance to 10%
        if(random16(530) < absSpeed+48) {
          if(initSpeed < dimSpeed) { ps1.SetDimSpeed(initSpeed+1, curTime); }
          else { ps1.SetDimSpeed(initSpeed-1, curTime); }
        }
      }
    }

    initSpeed = ps1.GetColorSpeed();
    if(initSpeed != colorSpeed) {
      uint8_t absSpeed = abs(initSpeed);
      if(absSpeed < 5) {
        // From 10% chance to 4%
        if(random8(50) <= absSpeed+2) {
          if(initSpeed < colorSpeed) { ps1.SetColorSpeed(initSpeed+1, curTime); }
          else { ps1.SetColorSpeed(initSpeed-1, curTime); }
        }
      }
      else {
        // From 33% chance to 10%
        if(random16(530) < absSpeed+48) {
          if(initSpeed < colorSpeed) { ps1.SetColorSpeed(initSpeed+1, curTime); }
          else { ps1.SetColorSpeed(initSpeed-1, curTime); }
        }
      }
    }
  #endif

  // Always match speeds
  ps2.SetDimSpeed(ps1.GetDimSpeed(), curTime);
  ps2.SetColorSpeed(ps1.GetColorSpeed(), curTime);
}

