#include "obj/Stackers.h"
#include "Util.h"
#include "ArduinoTrace.h"

////////////// Public interface //////////////
void Stackers::Init(uint16_t _numLEDs, PaletteManager* _pm, CRGB* _leds, uint8_t* _leds_b, struct_base_show_params* _params, std::vector<uint16_t> _allowedDimPeriods, uint8_t _numAllowedDimPeriods, uint32_t* pCurTime) {
  numLEDs = _numLEDs;
  pm = _pm;
  leds = _leds;
  leds_b = _leds_b;
  allowedDimPeriods = _allowedDimPeriods;
  numAllowedDimPeriods = _numAllowedDimPeriods;
  curTime = pCurTime;
  params = _params;

  CreateStacks(); // Probably unnecessary now that isFirstCycleOfNewMode causes this anyway
}

void Stackers::SkipTime(uint32_t amount) {
  lastModeTransition += amount;
  lastMoveTime += amount;
}

uint8_t Stackers::GetDisplayMode() {
  uint8_t displayMode = -1; // Can always fall back on stutterStep by color
  if(stackMode == StackMode::StutterStepMinSmooth) {
    for(int i = numStacks/2; i > 1; i--) {
      if(numStacks % i == 0) { displayMode = i; }
    }
  }
  else if(stackMode == StackMode::StutterStepMaxSmooth) {
    for(int i = 2; i <= numStacks/2; i++) {
      if(numStacks % i == 0) { displayMode = i; }
    }
  }

  return displayMode;
}

void Stackers::ScaleParams(bool stackLengthOnly) {
  maxStackLength = MIN_STACK_LENGTH + scaleParam16(params->brightLength, 0, dimPeriod - MIN_STACK_LENGTH);
  
  if(!stackLengthOnly) {
    numColors = scaleParam(params->numColors, 2, PALETTE_SIZE);
    dimPeriod = allowedDimPeriods[scaleParam(params->dimPeriod, 0, numAllowedDimPeriods-1)];
  }
}

uint8_t Stackers::CreateStacks(uint8_t mode) {
  ScaleParams();
  
  // Sets up a full collection of stacks with max length
  stackMode = StackMode(mode % int(StackMode::Length));
  if(stackMode == StackMode::None) { stackMode = DEFAULT_STACK_MODE; }

  moveClockwise = DEFAULT_MOVE_CLOCKWISE;
  lastModeTransition = *curTime;
  lastMoveTime = *curTime;

  // Handle initialization specifics
  if(stackMode == StackMode::Shutters) {
    if(numStacks == 0) { stackLength = 0; }
    numStacks = numLEDs / dimPeriod;

    for(uint8_t i = 0; i < numStacks; i++) {
      stacks[i].length = 0;
      stacks[i].pixel = i * dimPeriod;
      stacks[i].color = i % numColors;
    }
  }
  else if(stackMode == StackMode::Stack5 || stackMode == StackMode::Stack4Mirror) {
    numStacks = 0;
  }
  else if(stackMode == StackMode::StutterStepMinSmooth || stackMode == StackMode::StutterStepMaxSmooth || stackMode == StackMode::StutterStepColors) {
    numStacks = numLEDs / dimPeriod;
    stackLength = maxStackLength;
  }

  // Draw stacks
  for(uint16_t i = 0; i < numStacks; i++) {
    stacks[i].pixel = i * dimPeriod;
    stacks[i].color = i % numColors;
  }  

  return numStacks;
}

uint8_t Stackers::GetScaledDimSpeed() {
  int8_t speed = params->dimSpeed;
  moveClockwise = speed < 0;
  if(speed == -128) { speed = -127; } 
  uint8_t absSpeed = 2 * abs(speed);
  if(absSpeed == 254) { absSpeed = 255; }
  return scale8(MAX_MOVE_SPEED, absSpeed);
}

void Stackers::Stacks() {
  static int displayMode = 0;

  uint8_t moveSpeed = GetScaledDimSpeed();

  moveThisCycle = false;
  uint16_t adjMoveSpeed = moveSpeed;
  if(stackMode == StackMode::Stack2Mirror) { adjMoveSpeed *= 2; }
  if(adjMoveSpeed > 0 && (*curTime - lastMoveTime) >= FPS_TO_TIME(adjMoveSpeed)) {
    lastMoveTime = *curTime;
    moveThisCycle = true;
  }

  for(uint16_t i = 0; i < numLEDs; i++) { leds_b[i] = 0; } // Clear LEDs

  if(displayMode == 0) { displayMode = GetDisplayMode(); }

  if(transitionState != TransitionState::None && transitionState != TransitionState::Messy) { // Quick check
    uint32_t transTime = MAX_TRANS_TIME;// MAX_TRANS_TIME / 0xFFFF * baseParams.transLength;
    if(*curTime >= lastModeTransition + transTime) { // Min time enforced
      while(!isFirstCycleOfNewMode) { // Keep rerolling until mode is found
        uint8_t nextStackMode;
        
        // Check mode-specific requirements
        bool isValidMode = true;
        do {
          isValidMode = true;
          nextStackMode = random8(1, uint8_t(StackMode::Length)-1);
          if(uint8_t(nextStackMode) >= uint8_t(stackMode)) { nextStackMode++; } // Skip over current mode
          if(nextStackMode == uint8_t(StackMode::StutterStepColors)) {
            // Check to make sure colors are evenly spaced
            for(int i = 1; i < numStacks; i++) {
              if(stacks[i].color == stacks[i-1].color) { isValidMode = false; }
            }
          }
          else if(wrapItUp && !transitionableMode[nextStackMode]) { isValidMode = false; }
          else if(minStacksRequired[nextStackMode] > numLEDs / dimPeriod) { isValidMode = false; }
        } while(!isValidMode);

        // Check for valid transition state
        if(allowedModes[int(transitionState)][nextStackMode]) {
          moveClockwise = random8(2) == 0;
          stackMode = StackMode(nextStackMode);
          lastModeTransition = *curTime;
          isFirstCycleOfNewMode = true;

          // Set displayMode for new modes
          displayMode = GetDisplayMode();
        }
      }
    }
  }

if(isFirstCycleOfNewMode) { DUMP(int(stackMode)) }
  // Draw current stackMode
  if(stackMode == StackMode::Shutters)                  { transitionState = TransitionState(Shutters()); }
  else if(stackMode == StackMode::Stack3)               { transitionState = TransitionState(StackSections(3, 0)); }
  else if(stackMode == StackMode::Stack4)               { transitionState = TransitionState(StackSections(4, 0)); }
  else if(stackMode == StackMode::Stack5)               { transitionState = TransitionState(StackSections(5, 0)); }
  else if(stackMode == StackMode::Stack2Mirror)         { transitionState = TransitionState(StackSections_Mirror(2, 87)); }
  else if(stackMode == StackMode::Stack4Mirror)         { transitionState = TransitionState(StackSections_Mirror(4, 87)); }
  else if(stackMode == StackMode::StutterStepMinSmooth) { transitionState = TransitionState(StutterStepBands(displayMode)); }
  else if(stackMode == StackMode::StutterStepMaxSmooth) { transitionState = TransitionState(StutterStepBands(displayMode)); }
  else if(stackMode == StackMode::StutterStepColors)    { transitionState = TransitionState(StutterStepBands()); }
  else { THROW_DUMP(F("Unrecognized stackMode!"), int(stackMode)) }

  isFirstCycleOfNewMode = false;
}


////////////// Private helper functions //////////////
void Stackers::DrawStack(struct_stack& s) {
  uint8_t len = s.length;
  uint16_t pix = s.pixel;
  for(int j = 0; j < len; j++) {
    leds_b[pix] = PIXEL_BRIGHTNESS;
    leds[pix] = pm->palette[s.color];
    if(++pix == numLEDs) { pix = 0; }
  }
}
void Stackers::DrawAllStacks() {
  for(int i = 0; i < numStacks; i++) {
    DrawStack(stacks[i]);
  }
}
void Stackers::MoveStack(struct_stack& s, bool clockwise) {
  if(clockwise) {
    s.pixel--;
    if(s.pixel == 0xFFFF) { s.pixel = numLEDs-1; }
  }
  else {
    s.pixel++;
    if(s.pixel == numLEDs) { s.pixel = 0; }
  }
}
void Stackers::MoveAllStacks(bool clockwise) {
  for(int i = 0; i < numStacks; i++) {
    MoveStack(stacks[i], clockwise);
  }
}

void Stackers::PrepForInsert(uint8_t numSections) {
  int n = numStacks / numSections;

  for(int i = numSections-1; i > 0; i--) {
    for(int j = n; j > 0; j--) {
      int idx = i*n + j - 1;
      stacks[idx+i] = stacks[idx];
    }
  }
}
void Stackers::PrepForInsert_Mirror(uint8_t numSections) {
  if(numSections == 4) {
    for(int i = numStacks-1; i >= numStacks/2; i--) {
      stacks[i+numSections-1] = stacks[i];
    }

    for(int i = numStacks/2; i > 0; i--) {
      stacks[i] = stacks[i-1];
    }
  }
  else if(numSections == 2) {
    for(int i = numStacks; i > 0; i--) {
      stacks[i] = stacks[i-1];
    }
  }
}
uint8_t Stackers::WipeClean(uint8_t numSections, uint16_t offset, uint16_t progress) {
  uint16_t LEDsPerGroup = numLEDs / numSections;
  
  // Calculate these values once and remember until the next animation (This can probably be included in the following if block)
  static int16_t adjOffsets[MAX_SECTIONS] = { 0 };
  static int16_t maxAdjustment = 0;
  if(isFirstCycleOfNewMode) {
    maxAdjustment = 0;
    for(uint16_t i = 0; i < numSections; i++) {
      adjOffsets[i] = offset;
    }
  }

  // See if enough empty pixels for each wipe to start on one
  bool enoughEmptyPixels = false;
  uint8_t numEmptyPixels = 0;
  for(uint16_t i = 0; i < numLEDs; i++) {
    if(leds_b[i] == 0) {
      numEmptyPixels++;
      if(numEmptyPixels >= numSections) {
        enoughEmptyPixels = true;
        break;
      }
    }
  }

  // Aim for clean transitions that don't begin in the middle of stacks
  if(transitionState == TransitionState::Full && numSections > 2 && stacks[0].length!=LEDsPerGroup && enoughEmptyPixels) {
    int16_t prevOffset;
    int16_t adjOffset = offset;
    while(true) {
      prevOffset = adjOffset;
      uint16_t i = 0;
      for(i = 0; i <= numSections; i++) {
        if(leds_b[(i*LEDsPerGroup + adjOffset + numLEDs) % numLEDs] != 0) {
          adjOffset--;
          break;
        }
      }

      if(prevOffset == adjOffset) {
        // Lined up right. Break from loop
        for(uint8_t i = 0; i < numSections; i++) {
          adjOffsets[i] = adjOffset;
        }
        break;
      }

      if(offset - adjOffset >= LEDsPerGroup) {
        // Not possible to perfectly align a clean wipe. Set individual offsets
        for(uint8_t i = 0; i < numSections; i++) {
          adjOffsets[i] = offset;
          while(leds_b[(i*LEDsPerGroup + adjOffsets[i] + numLEDs) % numLEDs] != 0) {
            adjOffsets[i]--;
          }
          if(offset - adjOffsets[i] > maxAdjustment) { maxAdjustment = offset - adjOffsets[i]; }
        }

        // Start on the first lit pixel
        for(uint8_t i = 0; i < numSections; i++) {
          adjOffsets[i]++;
        }
        break;
      }
    }
  }
  
  // Debugging output
  if(isFirstCycleOfNewMode) {
    String temp = "";
    for(int i = 0; i < numSections; i++) {
      temp = temp + adjOffsets[i] + ", ";
    }
    DUMP(temp)
    DUMP(numSections)
    DUMP(progress)
  }

  for(uint16_t i = 0; i <= progress; i++) {
    for(uint8_t j = 0; j < numSections; j++) {
      leds_b[(i + j*LEDsPerGroup + adjOffsets[j] + numLEDs) % numLEDs] = 0;
    }
  }

  if(progress == LEDsPerGroup + maxAdjustment) {
    maxAdjustment = 0;
    for(uint16_t i = 0; i < numSections; i++) {
      adjOffsets[i] = offset;
    }
    return uint8_t(TransitionState::Empty);
  }
  return uint8_t(TransitionState::None);
}


////////// Private animations //////////////
uint8_t Stackers::StackSectionsUp(uint8_t numSections, uint16_t offset, uint16_t& progress, uint8_t &curStep) {
  uint16_t LEDsPerGroup = numLEDs / numSections;

  // Check for stack completion
  if(progress == LEDsPerGroup - (numStacks/numSections) * dimPeriod) {
    // Completed run; Create a stack if there is room
    if(progress >= maxStackLength) {
      PrepForInsert(numSections);
      int n = numStacks/numSections;
      for(int i = 0; i < numSections; i++) {
        int idx = (i+1)*(n+1) - 1;
        stacks[idx].pixel = (offset + progress-maxStackLength + i*LEDsPerGroup) % numLEDs;
        stacks[idx].length = maxStackLength;
        stacks[idx].color = (numStacks/numSections) % numColors;
      }

      numStacks += numSections;
      progress = 0;
    }
  }

  // Draw stacks
  DrawAllStacks();

  // Draw partial progress
  uint8_t numPixels = progress < maxStackLength ? progress : maxStackLength;
  for(int i = 0; i < numPixels; i++) {
    for(int j = 0; j < numSections; j++) {
      uint16_t idx = (offset + progress + j*LEDsPerGroup - i - 1) % numLEDs;
      leds[idx] = pm->palette[(numStacks/numSections) % numColors];
      leds_b[idx] = PIXEL_BRIGHTNESS;
    }
  }

  // Check if filled up all available LEDs
  uint16_t usedLEDs = numStacks/numSections * dimPeriod + progress;
  if(usedLEDs >= LEDsPerGroup) {
    curStep = 1;
    if(progress == 0) { return uint8_t(usedLEDs == LEDsPerGroup ? TransitionState::Full : TransitionState::Messy); }
    progress = 0;
  }

  return uint8_t(TransitionState::None);
}
uint8_t Stackers::StackSectionsDown(uint8_t numSections, uint16_t offset, uint16_t& progress, uint8_t &curStep) {
  static bool doPartials = true;

  if(isFirstCycleOfNewMode) { doPartials = false; }

  DrawAllStacks();

  uint16_t LEDsPerGroup = numLEDs / numSections;

  if(doPartials) {
    uint16_t sparePixels = LEDsPerGroup % dimPeriod;
    if(dimPeriod * (numStacks/numSections) > LEDsPerGroup) { // If stacks have been created but dimPeriod not filled yet
      if(sparePixels > stackLength + progress) { progress = sparePixels - stackLength; } // Skip over the opening blanks
      //sparePixels = 0; // For cases where there's enough room for the stack but not the spacing
    }
    else {
      // Draw partial pixels
      for(int i = 0; i < sparePixels && i < maxStackLength; i++) {
        for(int j = 0; j < numSections; j++) {
          int idx = (offset + sparePixels - i - 1 + j*LEDsPerGroup) % numLEDs;
          leds[idx] = pm->palette[(numStacks/numSections) % numColors];
          leds_b[idx] = PIXEL_BRIGHTNESS;
        }
      }
    }
  }

  // Start the Wipe at the start of the stack; don't waste time wiping the first set of blank pixels
  //int min = LEDsPerGroup - dimPeriod * numStacks/numSections;
  //if(min < 0) { min = sparePixels - stackLength; DUMP(min) }
  //if(progress < min) { progress = min; }

  uint8_t retVal = WipeClean(numSections, offset, progress);
  if(TransitionState(retVal) == TransitionState::Empty) {
    progress = 0;
    numStacks = 0;
    curStep = 0;
    doPartials = true;
  }

  return retVal;
}
uint8_t Stackers::StackSections(uint8_t numSections, uint16_t offset) {
  static uint16_t progress = 0;
  static uint8_t curStep = 0;

  if(isFirstCycleOfNewMode) {
    if(stackLength == 0) { numStacks = 0; }
    if(numStacks == 0) { curStep = 0; }
    
    if(numStacks*dimPeriod == numLEDs) {
      curStep = 1; // Transitioned in from full stacks
      stackLength = maxStackLength;
    }
    else {
      // Screen is empty
      curStep = 0;
      ScaleParams();
    }

    progress = 0;
  }
  else if(moveThisCycle) {
    progress++;
  }

  uint8_t retVal = uint8_t(TransitionState::None);
  if(curStep==0) { retVal = StackSectionsUp(numSections, offset, progress, curStep); }
  else if(curStep==1) { retVal = StackSectionsDown(numSections, offset, progress, curStep); }

  isFirstCycleOfNewMode = false;
  return retVal;
}

uint8_t Stackers::StackSectionsUp_Mirror(uint8_t numSections, uint16_t offset, uint16_t& progress, uint8_t &curStep) {
  uint16_t LEDsPerGroup = numLEDs / numSections;

  // Check for stack completion
  if(progress == LEDsPerGroup - (numStacks/numSections) * dimPeriod - (dimPeriod-maxStackLength)/2) {
    // Completed run; Create a stack if there is room
    if(progress >= maxStackLength) {
      PrepForInsert_Mirror(numSections);

      int n = numStacks/numSections;

      if(numSections == 4) {
        for(int i = 0; i < numSections; i++) {
          int idx = i == 0 ? 0 : (i == numSections-1) ? 4*(n+1)-1 : 2*n+i;
          stacks[idx].length = maxStackLength;
          stacks[idx].color = n % numColors;
          if(i % 2 == 0) { // Run evens forward
            stacks[idx].pixel = (offset + progress-maxStackLength + i*LEDsPerGroup-1) % numLEDs;
          }
          else { // Run odds as a mirror of idx-n-1
            //int iLast = i-1;
            //int lastIDX = iLast == 0 ? 0 : (iLast == numSections-1) ? 4*(n+1)-1 : 2*n+iLast;
            //stacks[idx].pixel = (numLEDs + numLEDs - stacks[lastIDX].pixel - maxStackLength) % numLEDs;
            stacks[idx].pixel = (offset + (i+1)*LEDsPerGroup - progress) % numLEDs;
          }
        }
      }
      else if(numSections == 2) {
        stacks[0].length = maxStackLength;
        stacks[0].color = n % numColors;
        stacks[0].pixel = (offset + progress-maxStackLength + numLEDs) % numLEDs;

        int idx = numStacks+1;
        stacks[idx].length = maxStackLength;
        stacks[idx].color = n % numColors;
        stacks[idx].pixel = (stacks[0].pixel + (numStacks+1)*dimPeriod) % numLEDs;
      }

      numStacks += numSections;
      progress = 0;
    }
  }

  // Draw stacks
  DrawAllStacks();

  if(2*progress >= maxStackLength && (numStacks == numLEDs/dimPeriod - numSections/2)) {
    // Create the last stack from the 2 halves
    if(numSections == 4) {
      int insertI = numStacks/2;
      int i = numStacks;
      for(; i >= insertI; i--) {
        stacks[i] = stacks[i-1];
      }

      stacks[i+1].pixel = (offset + numLEDs * 3/2 - progress) % numLEDs;
      stacks[i+1].length = maxStackLength;
      stacks[i+1].color = (numStacks/numSections) % numColors;
      DrawStack(stacks[i+1]);
      numStacks++;

      stacks[numStacks].pixel = (offset + numLEDs - progress) % numLEDs;
      stacks[numStacks].length = maxStackLength;
      stacks[numStacks].color = (numStacks/numSections) % numColors;
      DrawStack(stacks[numStacks]);
      numStacks++;
    }
    else if(numSections == 2) {
      PrepForInsert_Mirror(numSections);
      stacks[0].pixel = (offset + numLEDs - progress) % numLEDs;
      stacks[0].color = (numStacks/numSections) % numColors;
      stacks[0].length = maxStackLength;
      DrawStack(stacks[0]); // Todo: Why drawstack before drawAllSTacks?
      numStacks++;
    }

    curStep = 1;
    progress = 0;
    DrawAllStacks();
    return uint8_t(TransitionState::Full);
  }
  else {
    // Draw partial progress
    uint8_t numPixels = progress < maxStackLength ? progress : maxStackLength;

    for(int i = 0; i < numPixels; i++) {
      for(int j = 0; j < numSections; j++) {
        if(j % 2 == 0) {
          uint16_t idx = (offset + numLEDs + progress + j*LEDsPerGroup - i - 1) % numLEDs;
          leds[idx] = pm->palette[(numStacks/numSections) % numColors];
          leds_b[idx] = PIXEL_BRIGHTNESS;
        }
        else { // Mirror j-1
          uint16_t idx = (offset + numLEDs - progress + (j-1)*LEDsPerGroup + i + 1) % numLEDs;
          leds[idx] = pm->palette[(numStacks/numSections) % numColors];
          leds_b[idx] = PIXEL_BRIGHTNESS;
        }
      }
    }

    // Check if filled up all available LEDs
    if(numStacks/numSections  * dimPeriod + progress >= LEDsPerGroup) {
      curStep = 1;
      if(numStacks*dimPeriod == numLEDs) { return uint8_t(TransitionState::Full); }
      progress = 0;
    }
  }

  return uint8_t(TransitionState::None);
}
uint8_t Stackers::StackSectionsDown_Mirror(uint8_t numSections, uint16_t offset, uint16_t& progress, uint8_t &curStep) {
  static bool doPartials = true;
  if(isFirstCycleOfNewMode) { doPartials = false; }
  
  DrawAllStacks();

  uint16_t LEDsPerGroup = numLEDs / numSections;
  
  if(doPartials) {
    uint16_t sparePixels = (numLEDs - numStacks*dimPeriod) / numSections;
    if(dimPeriod * (numStacks/numSections) > LEDsPerGroup) { sparePixels = 0; } // For cases where there's enough room for the stack but not the spacing 

    // Draw partial pixels
    for(int i = 0; i < sparePixels && i < maxStackLength; i++) {
      for(uint8_t j = 0; j < numSections; j++) {
        if(j % 2 == 0) {
          uint16_t idx = (offset + numLEDs - i - 1) % numLEDs;
          leds[idx] = pm->palette[(numStacks/numSections) % numColors];
          leds_b[idx] = PIXEL_BRIGHTNESS;
        }
        else {
          uint16_t idx = (offset + numLEDs - i) % numLEDs;
          leds[idx] = pm->palette[(numStacks/numSections) % numColors];
          leds_b[idx] = PIXEL_BRIGHTNESS;
        }
      }
    }
  }

  for(uint16_t i = 0; i <= progress; i++) {
    for(uint8_t j = 0; j < numSections; j++) {
      if(j % 2 == 0) { leds_b[ (offset + i + j  *  LEDsPerGroup) % numLEDs ] = 0; } 
      else { leds_b[(offset + numLEDs - i + (j-1)*LEDsPerGroup) % numLEDs] = 0; }
    }
  }

  // Check for end condition
  if(progress == LEDsPerGroup) {
    progress = 0;
    numStacks = 0;
    curStep = 0;
    doPartials = true;
    
    return uint8_t(TransitionState::Empty);
  }

  return uint8_t(TransitionState::None);
}
uint8_t Stackers::StackSections_Mirror(uint8_t numSections, uint16_t offset) {
  static uint16_t progress = 0;
  static uint8_t curStep = 0;
  
  if(isFirstCycleOfNewMode) {
    if(stackLength == 0) { numStacks = 0; }
    if(numStacks == 0) { curStep = 0; }

    if(numStacks*dimPeriod == numLEDs) {
      curStep = 1;
      stackLength = maxStackLength;
    }
    else {
      // Stack is empty
      curStep = 0;
      ScaleParams();
    }

    isFirstCycleOfNewMode = false;
    progress = 0;
  }
  else if(moveThisCycle) {
    progress++;
  }

  if(curStep==0) { return StackSectionsUp_Mirror(numSections, offset, progress, curStep); }
  else if(curStep==1) { return StackSectionsDown_Mirror(numSections, offset, progress, curStep); }
  
  return uint8_t(TransitionState::None);
}

uint8_t Stackers::Shutters() {
  const uint8_t minStackLength = 2; // Locally; diff than the global MIN_STACK_LENGTH

  // Wipe off, wipe on - Done by changing size and moving location until size is 0, then opening up again from new position
  static bool fadeIn = true;

  if(numStacks == 0) {
    CreateStacks(uint8_t(StackMode::Shutters));
  }

  if(stackLength == maxStackLength) { fadeIn = false; }
  else if(stackLength <= minStackLength) { 
    fadeIn = true;
    if(wrapItUp && maxStackLength < MIN_STACK_LENGTH_FOR_EXIT) { maxStackLength = MIN_STACK_LENGTH_FOR_EXIT; }
    else { ScaleParams(true); }
  }

  if(fadeIn) {
    if(moveThisCycle) {
      stackLength++;
      for(int i = 0; i < numStacks; i++) { stacks[i].length = stackLength; }
      if(moveClockwise) { MoveAllStacks(true); }
    }

    DrawAllStacks();
    return uint8_t(stackLength < MIN_STACK_LENGTH ? TransitionState::None : TransitionState::Full);
  }
  else {
    if(moveThisCycle) {
      stackLength--;
      for(int i = 0; i < numStacks; i++) { stacks[i].length = stackLength; }
      if(!moveClockwise) { MoveAllStacks(false); }
    }

    DrawAllStacks();
    return uint8_t(stackLength < MIN_STACK_LENGTH ? TransitionState::None : TransitionState::Full);
  }
}

uint8_t Stackers::StutterStepBands(int numGroups) {
  // Subset of the stacks move forward until they hit another (or overlap), then the other half moves
  static int moveMod = 0;

  if(stackLength == 0) { stackLength = maxStackLength; }
    
  if(isFirstCycleOfNewMode) {
    moveMod = 0;
  
    for(int i = 0; i < numStacks; i++) {
      if(stacks[i].length == 0) {
        stacks[i].length = stackLength;
        stacks[i].pixel = dimPeriod * i;
      }
    }
  }

  const bool fullyMoveAllStacks = false;
  if(moveThisCycle) {
    if(numGroups == -1) {
      // No param given; make changes based on stackMode
      if(stackMode == StackMode::StutterStepColors) {
        bool moveCompleted = fullyMoveAllStacks; // Initialize differently for diff modes; booleans mean diff things
        for(int i = 0; i < numStacks; i++) {
          if(stacks[i].color == moveMod) {
            if(!fullyMoveAllStacks) {
// TODO: Check for collision before move? What if they spawn touching each other
              MoveStack(stacks[i], moveClockwise);
              // Look for collision with any other stack
              for(int j = 0; j < numStacks; j++) {
                if(moveClockwise) { if((stacks[j].pixel + stacks[j].length) % numLEDs == stacks[i].pixel) { moveCompleted = true; break; } }
                else              { if((stacks[i].pixel + stacks[i].length) % numLEDs == stacks[j].pixel) { moveCompleted = true; break; } }
              }
            }
            else {
              // moveCompleted starts as true and toggles off if anything moves
              bool stopped = false;
              for(int j = 0; j < numStacks; j++) {
                if(moveClockwise) { if((stacks[j].pixel + stacks[j].length) % numLEDs == stacks[i].pixel) { stopped = true; break; } }
                else              { if((stacks[i].pixel + stacks[i].length) % numLEDs == stacks[j].pixel) { stopped = true; break; } }
              }
              if(!stopped) { MoveStack(stacks[i], moveClockwise); moveCompleted = false; }
            }
          }
        }

        if(moveCompleted) {
          if(moveClockwise) { moveMod = (numColors + moveMod-1) % numColors; }
          else              { moveMod =  (moveMod+1) % numColors; }
        }
      }
    }
    else {
      // Standard use - based on numGroups instead of numColors
      bool moveCompleted = fullyMoveAllStacks;
      for(int i = moveMod; i < numStacks; i+=numGroups) {
        if(!fullyMoveAllStacks) {
          MoveStack(stacks[i], moveClockwise);
          for(int j = 0; j < numStacks; j++) {
            if(moveClockwise) { if((stacks[j].pixel + stacks[j].length) % numLEDs == stacks[i].pixel) { moveCompleted = true; break; } }
            else              { if((stacks[i].pixel + stacks[i].length) % numLEDs == stacks[j].pixel) { moveCompleted = true; break; } }
          }
        }
        else {
          // moveCompleted starts as true and toggles off if anything moves
          bool stopped = false;
          for(int j = 0; j < numStacks; j++) {
            if(moveClockwise) { if((stacks[j].pixel + stacks[j].length) % numLEDs == stacks[i].pixel) { stopped = true; break; } }
            else              { if((stacks[i].pixel + stacks[i].length) % numLEDs == stacks[j].pixel) { stopped = true; break; } }
          }
          if(!stopped) { MoveStack(stacks[i], moveClockwise); moveCompleted = false; }
        }
      }

      if(moveCompleted) {
        if(moveClockwise) { moveMod = (numGroups + moveMod - 1) % numGroups; }
        else { moveMod = (moveMod+1) % numGroups; }
        return StutterStepBands(numGroups);
      }
    }
  }

  DrawAllStacks();

  // return Messy when half way through movement, else Full (when evenly spaced)
  if(moveClockwise) { for(int i = 0; i < numStacks-1; i++) { if((stacks[i].pixel + dimPeriod) % numLEDs != stacks[i+1].pixel)           { return uint8_t(TransitionState::Messy); } } }
  else {              for(int i = 1; i < numStacks; i++)   { if((numLEDs + stacks[i].pixel - dimPeriod) % numLEDs != stacks[i-1].pixel) { return uint8_t(TransitionState::Messy); } } }
  return uint8_t(TransitionState::Full);
}
