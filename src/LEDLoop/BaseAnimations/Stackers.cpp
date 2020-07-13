#include "LEDLoop/LEDLoop.h"
#include "Definitions.h"



const uint32_t MAX_TRANS_TIME = 30*ONE_SEC;
#define MAX_STACKS 40
#define NUM_STACK_MODES 3
enum class StackMode : uint8_t { None, Shutters, StutterStep, Stack4 };

struct_stack stacks[MAX_STACKS];
uint8_t numStacks = 0;
uint8_t stackLength = 0;
uint8_t maxStackLength = 0;
uint16_t dimPeriod;
uint32_t lastModeTransition;
bool moveClockwise = true;
bool readyForTransition = false;
StackMode stackMode = StackMode::None;

void LEDLoop::Stacks() {
  if(readyForTransition) {
    // Animation is ready, so check if timer is up
    uint32_t transTime = 20*ONE_SEC;// MAX_TRANS_TIME / 0xFFFF * baseParams.transLength;
    if(timing.now - lastModeTransition >= transTime) {
      stackMode = StackMode((uint8_t(stackMode) % NUM_STACK_MODES) + 1); // Increment stackMode and skip over 'None'
      lastModeTransition = timing.now;
    }
  }

  // Draw current stackMode
  if(stackMode == StackMode::Shutters) { readyForTransition = Shutters(); }
  else if(stackMode == StackMode::Stack4) { readyForTransition = Stack4(); }
  else if(stackMode == StackMode::StutterStep) { readyForTransition = StutterStepBands(); }

  DrawAllStacks();
}

uint8_t LEDLoop::InitStacks() {
  // Sets up a full collection of stacks with max length
  dimPeriod = allowedDimPeriods[scaleParam(baseParams.dimPeriod, 0, NUM_ALLOWED_DIM_PERIODS-1)];
  numStacks = numLEDs / dimPeriod;
  maxStackLength = scale16(dimPeriod, baseParams.brightLength);
  uint8_t numColors = scaleParam(baseParams.numColors, 1, PALETTE_SIZE-1);

DUMP(dimPeriod)
DUMP(numStacks)
DUMP(maxStackLength)

  // Initialize shutters animation
  stackMode = StackMode::Shutters;
  lastModeTransition = timing.now;
  moveClockwise = random8(1);
  stackLength = 0;
  for(uint16_t i = 0; i < numStacks; i++) {
    stacks[i].pixel = i * dimPeriod;
    stacks[i].color = pm->palette[i % numColors];
  }  

  return numStacks;
}

void LEDLoop::DrawAllStacks() {
  for(int i = 0; i < numLEDs; i++) { leds_b[i] = 0; }
  
  for(int i = 0; i < numStacks; i++) {
    uint8_t len = stacks[i].length;
    uint16_t pix = stacks[i].pixel;
    for(int j = 0; j < len; j++) {
      leds_b[pix] = 128;
      leds[pix] = stacks[i].color;
      if(++pix == numLEDs) { pix = 0; }
    }
  }
}
void LEDLoop::MoveStack(struct_stack& s, bool clockwise) {
  if(clockwise) {
    s.pixel--;
    if(s.pixel == 0xFFFF) { s.pixel = numLEDs-1; }
  }
  else {
    s.pixel++;
    if(s.pixel == numLEDs) { s.pixel = 0; }
  }
}
void LEDLoop::MoveAllStacks(bool clockwise) {
  for(int i = 0; i < numStacks; i++) {
    MoveStack(stacks[i], clockwise);
  }
}

// Assumes maxStacks are already defined
bool LEDLoop::Shutters() {
  // Wipe off, wipe on - Done by changing size and moving location until size is 0, then opening up again from new position
  static bool fadeIn = true;

  if(stackLength == 0) { fadeIn = true; }
  else if(stackLength == maxStackLength) { fadeIn = false; }

  if(fadeIn) {
    stackLength++;
    
    for(int i = 0; i < numStacks; i++) { stacks[i].length = stackLength; }
    if(moveClockwise) { MoveAllStacks(true); }
    return stackLength == maxStackLength; // Ready for transition at max length
  }
  else {
    stackLength--;

    for(int i = 0; i < numStacks; i++) { stacks[i].length = stackLength; }
    if(!moveClockwise) { MoveAllStacks(false); }
    return false; // Don't allow this transition point for now... return stackLength == 0; // Ready for transition on empty screen
  }
}

bool LEDLoop::StutterStepBands() {
  // Half the stacks move forward until they hit another (or overlap), then the other half moves
  static bool moveOdd = false;
  
  if(     (stacks[0].pixel + stacks[0].length) % numLEDs == stacks[1].pixel) { moveOdd = true; }
  else if((stacks[1].pixel + stacks[1].length) % numLEDs == stacks[2].pixel) { moveOdd = false; }

  for(uint8_t i = moveOdd ? 1 : 0; i < numStacks; i+=2) { MoveStack(stacks[i], moveClockwise); }

  return (stacks[0].pixel + dimPeriod) % numLEDs == stacks[1].pixel; // half way through movement; i.e. evenly spaced
}

bool LEDLoop::Stack4() {
  return true;
}


/////////////////// Stackers ///////////////////////
#define NUM_STACKERS 4
struct_stacker stackers[NUM_STACKERS];
void LEDLoop::Stackers() {  
  static uint16_t rotationIndex = numLEDs - 18;
  static uint8_t clearMode = InitStackers(rotationIndex);
  static uint32_t lastRotation = timing.now;
  
  uint8_t moveSpeed = scaleParam((uint8_t)2*abs(baseParams.dimSpeed), uint8_t(20), uint8_t(REFRESH_RATE));
  int8_t rotationSpeed = scaleParam(baseParams.colorSpeed, int8_t(-REFRESH_RATE), int8_t(REFRESH_RATE));

  // Clear display
  for(uint16_t i = 0; i < numLEDs; i++) { leds[i] = CRGB::Black; leds_b[i] = 180; }

  // Rotate stackers
  if(rotationSpeed != 0) {
    while(timing.now - lastRotation >= FPS_TO_TIME(abs(rotationSpeed))) {
      lastRotation += FPS_TO_TIME(abs(rotationSpeed));
      for(uint8_t i = 0; i < NUM_STACKERS; i++) {
        if(rotationSpeed < 0) {
          stackers[i].startPixel--;
          if(stackers[i].startPixel == 0xFFFF) { stackers[i].startPixel = numLEDs-1; }
          stackers[i].endPixel--;
          if(stackers[i].endPixel == 0xFFFF) { stackers[i].endPixel = numLEDs-1; }
        }
        else {
          stackers[i].startPixel++;
          if(stackers[i].startPixel == numLEDs) { stackers[i].startPixel = 0; }
          stackers[i].endPixel++;
          if(stackers[i].endPixel == numLEDs) { stackers[i].endPixel = 0; }
        }
      }
    }
  }
  
  // Draw stackers
  bool done = true;
  for(uint8_t i = 0; i < NUM_STACKERS; i++) {
    stackers[i].moveSpeed = stackers[i].moveForward ? moveSpeed + rotationSpeed : abs(rotationSpeed - moveSpeed); // Todo: is this working?
    done &= DrawStacker(&stackers[i]);
  }

  if(done) {
    // Clear stackers
    if(ClearStackers(clearMode)) {
      // Reset animation and load new params
      clearMode = InitStackers(rotationIndex);
    }
  }
}

uint8_t LEDLoop::InitStackers(uint16_t rotationIndex) {
  uint8_t numColors = scaleParam(baseParams.numColors, 1, 4);
  uint8_t stackSize = 0;//scaleParam(baseParams.brightLength, 5, 15);
  uint8_t spacing = 0;//scaleParam(baseParams.transLength, 0, 4);
  spacing += spacing % 2; // Keep spacing even
    
  stackers[0].startPixel = (0 + rotationIndex + numLEDs) % numLEDs;
  stackers[0].endPixel = (numLEDs/4 - 1 - spacing/2 + rotationIndex + numLEDs) % numLEDs;
  
  if(NUM_STACKERS > 1) {
    stackers[1].startPixel = (numLEDs-1 + rotationIndex + numLEDs) % numLEDs;
    stackers[1].endPixel = (numLEDs*3/4 + spacing/2 + rotationIndex + numLEDs) % numLEDs;

    if(NUM_STACKERS > 2) {
      stackers[2].startPixel = (numLEDs/2 + rotationIndex + numLEDs) % numLEDs;
      stackers[2].endPixel = (numLEDs*3/4 - 1 - spacing/2 + rotationIndex + numLEDs) % numLEDs;

      if(NUM_STACKERS > 3) {
        stackers[3].startPixel = (numLEDs/2 - 1 + rotationIndex + numLEDs) % numLEDs;
        stackers[3].endPixel = (numLEDs/4 + spacing/2 + rotationIndex + numLEDs) % numLEDs;
      }
    }
  }
    
  for(uint8_t i = 0; i < NUM_STACKERS; i++) {
    stackers[i].numStacks = 0;
    stackers[i].numColors = numColors;
    stackers[i].stackSize = stackSize;
    stackers[i].spacing = spacing;
    stackers[i].moveIndex = stackers[i].startPixel;
    stackers[i].moveForward = i % 2 == 0;
    stackers[i].lastMove = timing.now;
  }

  // return clearMode
  return scaleParam(baseParams.displayMode, 0, 3);
}

bool LEDLoop::DrawStacker(struct_stacker* s) {
  uint16_t curPixel = s->endPixel;
  for(uint8_t i = 0; i < s->numStacks; i++) {
    // Draw stack
    for(uint8_t j = 0; j < s->stackSize; j++) {
      leds[curPixel] = pm->palette[i % s->numColors];
      if(curPixel == s->startPixel) { break; }
      if(s->moveForward) {
        curPixel--;
        if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
      }
      else {
        curPixel++;
        if(curPixel == numLEDs) { curPixel = 0; }
      }
    }

    if(curPixel != s->startPixel) {
      // Draw spacing
      for(uint8_t j = 0; j < s->spacing; j++) {
        leds[curPixel] = CRGB::Black; //todo: superfluous line since we clear and redraw every cycle
        if(curPixel == s->startPixel) { break; }
        if(s->moveForward) {
          curPixel--;
          if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
        }
        else {
          curPixel++;
          if(curPixel == numLEDs) { curPixel = 0; }
        }
      }
    }
  }

  if(curPixel == s->startPixel) { return true; }

  if(s->moveIndex == curPixel) {
    // End of movement for this stack; rotation moved into end position
    s->numStacks++;
    s->moveIndex = s->startPixel;
  }
  
  // Move
  while(timing.now - s->lastMove >= FPS_TO_TIME(s->moveSpeed)) {
    s->lastMove += FPS_TO_TIME(s->moveSpeed);
    if(s->moveForward) {
      s->moveIndex++;
      if(s->moveIndex == numLEDs) { s->moveIndex = 0; }
    }
    else {
      s->moveIndex--;
      if(s->moveIndex == 0xFFFF) { s->moveIndex = numLEDs-1; }
    }
  }
  
  // Draw moving stack
  for(uint8_t i = 0; i < s->stackSize; i++) {
    uint16_t idx = s->moveForward ? (s->moveIndex - i + numLEDs) % numLEDs : (s->moveIndex + i) % numLEDs;
    leds[idx] = pm->palette[s->numStacks % s->numColors];
    if(idx == s->startPixel) { break; }
  }

  if(s->moveIndex == curPixel) {
    // End of movement for this stack
    s->numStacks++;
    s->moveIndex = s->startPixel;
  }

  return false;
}

bool LEDLoop::ClearStackers(uint8_t clearMode) {
  static uint16_t clearMoveIndex = 0;
  
  if(clearMode == 0) { return true; }
  else if(clearMode == 1) {
    // Clear from endPos
    for(uint8_t i = 0; i < NUM_STACKERS; i++) {
      uint16_t curPixel = stackers[i].endPixel;
      if(stackers[i].moveForward) {
        for(uint8_t j = 0; j < clearMoveIndex; j++) {
          leds[curPixel--] = CRGB::Black;
          if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
        }
      }
      else {
        for(uint8_t j = 0; j < clearMoveIndex; j++) {
          leds[curPixel++] = CRGB::Black;
          if(curPixel == numLEDs) { curPixel = 0; }
        }
      }
    }

    while(timing.now - stackers[0].lastMove >= FPS_TO_TIME(stackers[0].moveSpeed)) {
      for(uint8_t i = 0; i < NUM_STACKERS; i++) { stackers[i].lastMove += FPS_TO_TIME(stackers[i].moveSpeed); }
      clearMoveIndex++;
      if(clearMoveIndex >= numLEDs / NUM_STACKERS) { clearMoveIndex = 0; return true; }
    }
  }
  else if(clearMode == 2) {
    // Clear from startPos
    for(uint8_t i = 0; i < NUM_STACKERS; i++) {
      uint16_t curPixel = stackers[i].startPixel;
      if(stackers[i].moveForward) {
        for(uint8_t j = 0; j < clearMoveIndex; j++) {
          leds[curPixel++] = CRGB::Black;
          if(curPixel == numLEDs) { curPixel = 0; }
        }
      }
      else {
        for(uint8_t j = 0; j < clearMoveIndex; j++) {
          leds[curPixel--] = CRGB::Black;
          if(curPixel == 0) { curPixel = numLEDs-1; }
        }
      }
    }

    while(timing.now - stackers[0].lastMove >= FPS_TO_TIME(stackers[0].moveSpeed)) {
      for(uint8_t i = 0; i < NUM_STACKERS; i++) { stackers[i].lastMove += FPS_TO_TIME(stackers[i].moveSpeed); }
      clearMoveIndex++;
      if(clearMoveIndex >= numLEDs / NUM_STACKERS) { clearMoveIndex = 0; return true; }
    }
  }
  else if(clearMode == 3) {
    // Overwrite cleared stacks and draw moving black stack
    static uint8_t numClearedStacks = 0;
    
    for(uint8_t i = 0; i < NUM_STACKERS; i++) {
      uint16_t curPixel = stackers[i].endPixel;
      uint16_t limit = numClearedStacks * (stackers[i].stackSize + stackers[i].spacing);
      
      for(uint8_t j = 0; j < limit; j++) {
        if(stackers[i].moveForward) {
          leds[curPixel--] = CRGB::Black;
          if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
        }
        else {
          leds[curPixel++] = CRGB::Black;
          if(curPixel == numLEDs) { curPixel = 0; }
        }
      }
  
      for(uint8_t j = 0; j < stackers[i].stackSize; j++) {
        uint16_t idx = stackers[i].moveForward ? (stackers[i].startPixel + clearMoveIndex - j + numLEDs) % numLEDs : (stackers[i].startPixel - clearMoveIndex + j + numLEDs) % numLEDs;
        leds[idx] = CRGB::Black;
        if(idx == stackers[i].startPixel) { break; }
      }
  
      // Move and check for end of clear area
      while((timing.now - stackers[i].lastMove) >= FPS_TO_TIME(stackers[i].moveSpeed)) {
        stackers[i].lastMove += FPS_TO_TIME(stackers[i].moveSpeed);
        if(i == NUM_STACKERS-1) { clearMoveIndex++; }
        uint16_t limit = numLEDs / NUM_STACKERS - stackers[i].spacing/2 - numClearedStacks*(stackers[i].stackSize + stackers[i].spacing);
        if(clearMoveIndex == limit) {
          if(i == NUM_STACKERS-1) {
            numClearedStacks++;
            clearMoveIndex = 0;
            if(numClearedStacks == stackers[i].numStacks) { numClearedStacks = 0; return true; }
          }
        }
      }
    }
  }

  return false;
}

