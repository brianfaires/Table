#include "PatternController.h"

PatternController::PatternController() {
}

uint32_t PatternController::GetColorPauseLength() {
  return ps1.colorPauseLength;
}
void PatternController::SetColorPauseLength(uint32_t value) {
  ps1.colorPauseLength = value;
  ps2.colorPauseLength = value;
}

uint32_t PatternController::GetDimPauseLength() {
  return ps1.dimPauseLength;
}
void PatternController::SetDimPauseLength(uint32_t value) {
  ps1.dimPauseLength = value;
  ps2.dimPauseLength = value;
}

uint32_t PatternController::GetColorBlendLength() {
  return ps1.colorBlendLength;
}
void PatternController::SetColorBlendLength(uint32_t value) {
  ps1.colorBlendLength = value;
  ps2.colorBlendLength = value;
}

uint32_t PatternController::GetDimBlendLength() {
  return ps1.dimBlendLength;
}
void PatternController::SetDimBlendLength(uint32_t value) {
  ps1.dimBlendLength = value;
  ps2.dimBlendLength = value;
}

void PatternController::Init(uint16_t _numLEDs, struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, uint32_t curTime) {
  SetDimBlendLength(INIT_PATTERN_CONTROLLER_DIM_BLEND_LENGTH);
  SetColorBlendLength(INIT_PATTERN_CONTROLLER_COLOR_BLEND_LENGTH);
  SetDimPauseLength(INIT_PATTERN_CONTROLLER_DIM_PAUSE_LENGTH);
  SetColorPauseLength(INIT_PATTERN_CONTROLLER_COLOR_PAUSE_LENGTH);

  numLEDs = _numLEDs;

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
  WalkSpeeds();

  // Check for changes in dimPeriod and colorPeriod. If so, start splitting to bring in the new pattern
  if(!splitDisplay)
  if(ps->IsStartOfDimPattern() && dimSpeed != 0) {
    if((ps->dimPeriod != scaledParams.dimPeriod) || (ps->colorPeriod != scaledParams.colorPeriod)) {
      StartSplit(scaledParams, curTime);
    }
  }

  if(ps->dimSpeed != secondary->dimSpeed) { THROW(String(ps->dimSpeed) + " DOES NOT EQUAL " + String(secondary->dimSpeed)); }

  // Update primary PatternScroller
  ps->numColors = scaledParams.numColors;
  //ps->colorPeriod = scaledParams.colorPeriod;
  ps->brightLength = scaledParams.brightLength;
  ps->transLength = scaledParams.transLength;
  ps->SetDisplayMode(scaledParams, curTime);
  bool psMoved = ps->Update(curTime);

  if(!splitDisplay) {
    ps->SetCRGBs(target, target_b, numLEDs);
  }
  else {
    // Update secondary PatternScroller, using its existing periods
    ScaleParams(params, scaledParams, secondary->dimPeriod, secondary->colorPeriod);
    secondary->numColors = scaledParams.numColors;
    //secondary->colorPeriod = scaledParams.colorPeriod; // Don't update colorPeriod if already splitting
    secondary->brightLength = scaledParams.brightLength;
    secondary->transLength = scaledParams.transLength;
    secondary->SetDisplayMode(scaledParams, curTime);
    bool secMoved = secondary->Update(curTime);

    if(secondaryScrollerIsLow) {
      //ps->SetCRGBs(&target[splitIndex], &target_b[splitIndex], numLEDs - splitIndex);
      ps->SetCRGBs(target, target_b, numLEDs); // debug: this is wasteful but simple. Otherwise pattern will move 2 pixels per move. Just overwrite the pixels in next line
      secondary->SetCRGBs(target, target_b, splitIndex+1);
    }
    else {
      //secondary->SetCRGBs(&target[splitIndex], &target_b[splitIndex], numLEDs - splitIndex);
      secondary->SetCRGBs(target, target_b, numLEDs); // debug: this is wasteful but simple. Otherwise pattern will move 2 pixels per move. Just overwrite the pixels in next line
      ps->SetCRGBs(target, target_b, splitIndex+1);
    }

    // Always move the split point with the dim pattern, even if a change in colorPeriod is being applied
    if(psMoved) {
      if(!secMoved) { THROW("ps moved and secondary didn't") }
      
      if(dimSpeed > 0) {
        splitIndex++;
        if(splitIndex == numLEDs) { EndSplit(); }
      }
      else if(dimSpeed < 0) {
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
  if(dim_period == 0) {
    #ifdef EXPLICIT_PARAMETERS
      output.dimPeriod = params.dimPeriod;
    #else
      // debug: scale dim period here
    #endif
  }
  else {
    output.dimPeriod = dim_period;
  }
  
  if(color_period == 0) {
    #ifdef EXPLICIT_PARAMETERS
      output.colorPeriod = params.colorPeriod;
    #else
      // debug: scale color period here
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
    uint8_t abs_dimSpeed = scaleParam((uint8_t)abs(params.dimSpeed), 0, 63);
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
    output.displayMode = scaleParam(params.displayMode, 0, NUM_DIM_PATTERNS * NUM_COLOR_PATTERNS - 1);
    output.numColors = scaleParam(params.numColors, 2, PALETTE_SIZE-1);
    output.transLength = scaleParam(params.transLength, 4, 8);
    output.brightLength = scaleParam(params.brightLength, 0, output.dimPeriod - 2*output.transLength - 2);
  #endif
}

void PatternController::StartSplit(struct_base_show_params& params, uint32_t curTime) {
  Serial.println("Start Split");
  splitDisplay = true;
  splitIndex = dimSpeed > 0 ? 0 : numLEDs-1;

  secondary->Clone(ps, params, curTime);
  //secondary->Init(params, curTime);
  //secondary->SyncMovement(ps);

  secondaryScrollerIsLow = dimSpeed > 0;
}

void PatternController::EndSplit() {
  splitDisplay = false;
  if(dimSpeed > 0 && secondaryScrollerIsLow || dimSpeed < 0 && !secondaryScrollerIsLow) {
    PatternScroller* swap = ps;
    ps = secondary;
    secondary = swap;
    Serial.println("SWAP!");
  }
  Serial.println("End Split");
}

void PatternController::WalkSpeeds() {
// Perform everything on ps1, then copy to ps2. They should always match to be fully sync'd in movements

  #ifdef EXPLICIT_PARAMETERS
    ps1.dimSpeed = dimSpeed;
    ps1.colorSpeed = colorSpeed;
  #else
    // Gradually update speeds even if not ready for a pattern change; slow down at lower levels
    if(ps1.dimSpeed != dimSpeed) {
      uint8_t absSpeed = abs(ps1.dimSpeed);
      if(absSpeed < 5) {
        // From 10% chance to 4%
        if(random8(50) < absSpeed+2) {
          if(ps1.dimSpeed < dimSpeed) { ps1.dimSpeed++; }
          else { ps1.dimSpeed--; }
        }
      }
      else {
        // From 33% chance to 10%
        if(random16(530) < absSpeed+48) {
          if(ps1.dimSpeed < dimSpeed) { ps1.dimSpeed++; }
          else { ps1.dimSpeed--; }
        }
      }
    }
  
    if(ps1.colorSpeed != colorSpeed) {
      uint8_t absSpeed = abs(ps1.colorSpeed);
      if(absSpeed < 5) {
        // From 10% chance to 4%
        if(random8(50) <= absSpeed+1) {
          if(ps1.colorSpeed < colorSpeed) { ps1.colorSpeed++; }
          else { ps1.colorSpeed = ps1.colorSpeed--; }
        }
      }
      else {
        // From 33% chance to 10%
        if(random16(530) < absSpeed+48) {
          if(ps1.colorSpeed < colorSpeed) { ps1.colorSpeed++; }
          else { ps1.colorSpeed--; }
        }
      }
    }
  #endif

  // Always match speeds
  ps2.dimSpeed = ps1.dimSpeed;
  ps2.colorSpeed = ps1.colorSpeed;
}

