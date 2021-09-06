/* A component of LEDLoop; defines the logic for managing multiple animations on different layers, and the transitions between animations on a single layer.
*/
#include "obj/LEDLoop.h"

// Counters for transitions
uint8_t baseTransitionProgress;
uint8_t topTransitionProgress;


void LEDLoop::DrawBaseLayer() {
  switch(baseParams.animation) {
    case BaseAnimation::Scroller:
      pc.Update(baseParams, leds, leds_b);
      break;

    case BaseAnimation::Fire:
      Fire();
      break;

    case BaseAnimation::DiscoFire:
      DiscoFire();
      break;

    case BaseAnimation::ColorExplosion:
      ColorExplosion();
      break;

    case BaseAnimation::Orbs:
      Orbs();
      break;

    case BaseAnimation::GlimmerBands:
      ScrollingGlimmerBands();
      break;

    case BaseAnimation::CenterSpawn:
      CenterSpawn();
      break;

    case BaseAnimation::Stacks:
      stackers.Stacks();
      break;

    default:
      leds = CRGB::Black;
      timing.lastDraw = timing.now;
      break;
  }

  // Handle fading between animations
  leds.fadeToBlackBy(baseTransitionProgress);
}
void LEDLoop::InitBaseLayer() {
  DEBUG_ANIMATION("Init base layer: " + baseParams.animation);
  
  switch(baseParams.animation) {
    case BaseAnimation::Scroller:
      pc.Init(numLEDs, &(timing.now), baseParams, pm, &Gamma, allowedDimPeriods, allowedColorPeriods, NUM_ALLOWED_DIM_PERIODS, NUM_ALLOWED_COLOR_PERIODS);
      break;

    case BaseAnimation::Stacks:
      stackers.CreateStacks();
      break;
      
    default:
      break;
  }
}
void LEDLoop::CleanupBaseLayer(BaseAnimation lastAnimation) {
  DEBUG_ANIMATION("Cleanup base layer: " + lastAnimation);
  
  switch(lastAnimation) {
    default:
      break;
  }
}
void LEDLoop::TransitionBaseAnimation() {
  static int activeTransition = 0; // Used to stay in the transition code block, even if starting conditions are no longer met
  static int transitionPhase = 0;  // Tracks progress within a transition
  static uint32_t transStartTime = 0; // Useful for timed actions within a phase

  // First, identify transition type.  Default is currently a slow fade out and in.
  BaseAnimation nextBase = PeekNextBaseAnimation();
  if(activeTransition == 1 || (activeTransition == 0 && baseParams.animation == BaseAnimation::Stacks && nextBase == BaseAnimation::Scroller)) {
    #pragma region Stacks->Scroller    
    if(timing.now - timing.lastBaseTransition < layerConfig.basePauseLength) { return; }
    if(stackers.transitionState != Stackers::TransitionState::Full) { stackers.wrapItUp = true; return; }
    if(stackers.stackLength < MIN_STACK_LENGTH_FOR_EXIT) { return; } // Below min size for dimPatterns
    uint16_t dimPeriod = numLEDs / stackers.numStacks; //allowedDimPeriods[scaleParam(baseParams.dimPeriod, 0, NUM_ALLOWED_DIM_PERIODS-1)];
    if(stackers.stackLength > dimPeriod-3) { return; } // Above max size for dimPatterns (and a little extra to avoid overflowing scaled params)

    if(transitionPhase == 0) {
      DEBUG_TRANSITIONS("Stacks->Scroller: Begin Phase 0")
      activeTransition = 1;
      stackers.wrapItUp = false;
      // Assume a consistent dimPattern coming from stackers;  Initialize dimPattern to match period, length, and offset
      uint16_t maxExtraPixels = dimPeriod - MIN_SCROLLER_LIT_PLUS_ONE;
      baseParams.brightLength = (stackers.stackLength - MIN_SCROLLER_LIT) * 0x10000 / maxExtraPixels; // Scaled based on the number of extra pixels needed
      baseParams.transLength = baseParams.brightLength;
      baseParams.colorPeriod = 255;
      uint8_t scaledDown = scaleParam16(baseParams.brightLength, 0, (dimPeriod-MIN_SCROLLER_LIT_PLUS_ONE)/3);
      uint8_t moarPix = stackers.stackLength - (MIN_SCROLLER_LIT + 3*scaledDown);
      if(moarPix == 1) { baseParams.brightLength += (3 * 0x10000 / maxExtraPixels); }
      else if(moarPix == 2) { baseParams.transLength += (3 * 0x10000 / maxExtraPixels); }

      // Find the stack/dimPeriod that contains pixel 0; ie its location is 0, or the highest value
      uint16_t maxStackStart = stackers.stacks[0].pixel;
      uint8_t maxStack = 0;
      for(int i = 0; i < stackers.numStacks; i++) {
        if(stackers.stacks[i].pixel == 0) {
          maxStack = i;
          maxStackStart = 0;
          break;
        }
        if(stackers.stacks[i].pixel > maxStackStart) {
          maxStack = i;
          maxStackStart = stackers.stacks[i].pixel;
        }
      }

      // Initialize colorPattern to match the existing colors
      uint8_t colorIndexes[stackers.numStacks];
      for(int i = 0; i < stackers.numStacks; i++) {
        uint8_t colorIndex = stackers.stacks[(maxStack + i) % stackers.numStacks].color;
        colorIndexes[i] = colorIndex;
      }
      
      pc.setManualBlocks(colorIndexes, stackers.numStacks, dimPeriod);
      pc.setSyncScrollingSpeeds(true);

      // Set displayMode to snake and manualBlocks
      baseParams.displayMode = pc.GenerateDisplayModeValue(DimPatternName::Snake, ColorPatternName::ManualBlocks);

      // Init speed to match
      baseParams.dimSpeed = stackers.GetScaledDimSpeed();
      if(stackers.moveClockwise) { baseParams.dimSpeed *= -1; }
          
      baseParams.animation = PeekNextBaseAnimation();
      InitBaseLayer();
      
      pc.setDimIndexOffset((dimPeriod + numLEDs - stackers.stacks[0].pixel) % dimPeriod); // Not covered in Init()
      pc.setColorIndexOffset((dimPeriod + numLEDs - stackers.stacks[0].pixel) % dimPeriod); // Not covered in Init()

      pc.setManualBlocks(colorIndexes, stackers.numStacks, dimPeriod); // Called twice to override what was done in Init() - Maybe not necessary?

      transitionPhase = 1;
    }
    else if(transitionPhase == 1) {
      // Blend into desired color pattern
      if(transStartTime == 0) {
        DEBUG_TRANSITIONS("Stacks->Scroller: End Phase 0/Begin Phase 1")
        baseParams.displayMode = pc.GenerateDisplayModeValue(DimPatternName::Random, ColorPatternName::Gradient); // Todo: what to default to?
        pc.BeginColorBlend();
        //pc.BeginDimBlend();
        transStartTime = timing.now;
      }
      else if(timing.now - transStartTime >= pc.getColorBlendLength()) {
        DEBUG_TRANSITIONS("Stacks->Scroller: Swapping to Scroller")
        timing.lastBaseTransition = timing.now;
        transitionPhase = 0;
        activeTransition = 0;
        transStartTime = 0;
        pc.setSyncScrollingSpeeds(false);
      }
    }
    #pragma endregion
  }
  else if(activeTransition == 2 || (activeTransition == 0 && baseParams.animation == BaseAnimation::Scroller && nextBase == BaseAnimation::Stacks)) {
    #pragma region Scroller->Stacks

    // Bring speed within allowable range
    if(timing.now - timing.lastBaseTransition < layerConfig.basePauseLength) { return; } // Wait for pauseLength before starting transition
    if(abs(baseParams.dimSpeed) > stackers.MAX_MOVE_SPEED) {
      if(baseParams.dimSpeed < 0) { baseParams.dimSpeed = -1 * stackers.MAX_MOVE_SPEED; }
      else { baseParams.dimSpeed = stackers.MAX_MOVE_SPEED; }
    }
    
    if(abs(pc.ps->getDimSpeed()) > stackers.MAX_MOVE_SPEED) { return; }
    
    // Bring colorSpeed to match dimSpeed, then enable syncScrollingSpeeds
    // Also need to make sure that dimPatternOffset == colorPatternOffset before sync'ing speed
    uint16_t dimPeriod = allowedDimPeriods[scaleParam(baseParams.dimPeriod, 0, NUM_ALLOWED_DIM_PERIODS-1)];
    int16_t diff = pc.getDimIndexOffset() - (pc.getColorIndexOffset() % dimPeriod);
    if(diff != 0) { return; }

    //if(baseParams.colorSpeed < baseParams.dimSpeed) { baseParams.colorSpeed++; DUMP(baseParams.colorSpeed) }
    //else if(baseParams.colorSpeed > baseParams.dimSpeed) { baseParams.colorSpeed--; DUMP(baseParams.colorSpeed) }
    //if(baseParams.colorSpeed == baseParams.dimSpeed) { pc.syncScrollingSpeeds = true; DUMP(baseParams.colorSpeed) }

    if(transitionPhase == 0) {
      activeTransition = 2;
      if(transStartTime == 0) {
        DEBUG_TRANSITIONS("Scroller->Stacks: Begin Phase 0")
        // Switch to snake and fixed colors
        baseParams.displayMode = pc.GenerateDisplayModeValue(DimPatternName::Snake, ColorPatternName::ManualBlocks);
        pc.setSyncScrollingSpeeds(true);
        //pc.BeginDimBlend(); // Too jumpy with low blend speed
        pc.BeginColorBlend();
        transStartTime = timing.now;
        
        // Define manual block colors
        uint8_t numPeriods = numLEDs / dimPeriod;
        uint8_t colIndexes[numPeriods];
        uint8_t numColors = scaleParam(baseParams.numColors, 2, PALETTE_SIZE-1);
        for(int i = 0; i < numPeriods; i++) {
          colIndexes[i] = i % numColors;
        }
        pc.setManualBlocks(colIndexes, numPeriods, dimPeriod);
      }
      else {
        uint32_t timeElapsed = timing.now - transStartTime;
        if((timeElapsed >= pc.getDimPauseLength() + pc.getDimBlendLength()) && (timeElapsed >= pc.getColorBlendLength())) {
          DEBUG_TRANSITIONS("Scroller->Stacks: End Phase 0")
          transitionPhase = 1;
          transStartTime = 0;
        }
      }
    }
    else if(transitionPhase == 1) {
      if(transStartTime == 0) { DEBUG_TRANSITIONS("Scroller->Stacks: Swapping to Stacks") }
      // Create stacks to match current pattern, then hard swap
      stackers.stackLength = MIN_SCROLLER_LIT + 2*pc.ps->transLength + pc.ps->brightLength;
      stackers.numStacks = numLEDs / dimPeriod;
      stackers.moveClockwise = baseParams.dimSpeed < 0;
      uint8_t targetSpeed = abs(baseParams.dimSpeed);
      DUMP(targetSpeed)
      if(targetSpeed > Stackers::MAX_MOVE_SPEED) { targetSpeed = Stackers::MAX_MOVE_SPEED; }
      if(baseParams.dimSpeed >= 0) { while(stackers.GetScaledDimSpeed() < targetSpeed) { baseParams.dimSpeed++; } }
      if(baseParams.dimSpeed <= 0) { while(stackers.GetScaledDimSpeed() > targetSpeed) { baseParams.dimSpeed--; } }

      uint8_t* colIndexes = pc.getManualBlocks();
      DUMP(stackers.numStacks)
      for(int i = 0; i < stackers.numStacks; i++)
      {
        stackers.stacks[i].color = colIndexes[i];
        stackers.stacks[i].pixel = (dimPeriod*i + numLEDs - pc.getColorIndexOffset()) % numLEDs;
        stackers.stacks[i].length = stackers.stackLength;
      }
      
      baseParams.animation = BaseAnimation::Stacks;
      activeTransition = 0;
      transitionPhase = 0;
      timing.lastBaseTransition = timing.now;
TRACE()
    }
    #pragma endregion
  }
  else {
    // ------------ Default - Fade transition --------------------
    // Currently, not possible to draw 2 animations at once.  One fades out, then one fades in.
    static bool alreadySwitched = false;
    
    if(timing.now - timing.lastBaseTransition >= layerConfig.basePauseLength) {
      uint32_t transitionTime = timing.now - timing.lastBaseTransition - layerConfig.basePauseLength;
      if(transitionTime < layerConfig.baseTransOutLength) {
        baseTransitionProgress = 256 * transitionTime / layerConfig.baseTransOutLength;
      }
      else if(transitionTime < layerConfig.baseTransOutLength + layerConfig.baseTransInLength) {
        baseTransitionProgress = 256 * (layerConfig.baseTransOutLength + layerConfig.baseTransInLength - transitionTime) / layerConfig.baseTransInLength;
        if(!alreadySwitched) {
          NextBaseAnimation();
          alreadySwitched = true;
        }
      }
      else {
        alreadySwitched = false;
        baseTransitionProgress = 0;
        timing.lastBaseTransition = timing.now;
      }
    }
  }
}
void LEDLoop::NextBaseAnimation() {
  BaseAnimation lastBaseAnimation = baseParams.animation;
  baseParams.animation = PeekNextBaseAnimation();
  CleanupBaseLayer(lastBaseAnimation);
  InitBaseLayer();
}
BaseAnimation LEDLoop::PeekNextBaseAnimation() {
  return BaseAnimation(1 + ((uint8_t)baseParams.animation % NUM_BASE_ANIMATIONS));
}

void LEDLoop::DrawTopLayer() {
  switch(topParams.animation) {
    case TopAnimation::Glitter:
      Glitter();
      break;
      
    case TopAnimation::Rain:
      Rain();
      break;
    
    case TopAnimation::Twinkle:
      Twinkle();
      break;
      
    case TopAnimation::Bounce:
      //Bounce();
      break;
      
    case TopAnimation::Comets:
      FourComets();
      break;
      
    case TopAnimation::Collision:
      Collision();
      break;
      
    default:
      leds_top = CRGB::Black;
      break;
  }

  leds_top.fadeToBlackBy(topTransitionProgress);
}
void LEDLoop::InitTopLayer() {
  DEBUG_ANIMATION("Init top layer: " + topParams.animation);
  
  switch(topParams.animation) {
    default:
      break;
  }
}
void LEDLoop::CleanupTopLayer(TopAnimation lastAnimation) {
  DEBUG_ANIMATION("Cleanup top layer: " + lastAnimation);
  
  switch(lastAnimation) {
    default:
      break;
  }
}
void LEDLoop::TransitionTopAnimation() {
  // Currently, not possible to draw 2 animations at once.  One fades out, then one fades in.
  static bool alreadySwitched = false;
  
  if(timing.now - timing.lastTopTransition >= layerConfig.topPauseLength) {
    uint32_t transitionTime = timing.now - timing.lastTopTransition - layerConfig.topPauseLength;
    if(transitionTime < layerConfig.topTransOutLength) {
      topTransitionProgress = 256 * transitionTime / layerConfig.topTransOutLength;
    }
    else if(transitionTime < layerConfig.topTransOutLength + layerConfig.topTransInLength) {
      topTransitionProgress = 255 * (layerConfig.topTransOutLength + layerConfig.topTransInLength - transitionTime) / layerConfig.topTransInLength;
      if(!alreadySwitched) {
        NextTopAnimation();
        alreadySwitched = true;
      }
    }
    else {
      alreadySwitched = false;
      topTransitionProgress = 0;
      timing.lastTopTransition += timing.now;
    }
  }
}
void LEDLoop::NextTopAnimation() {
  if(NUM_TOP_ANIMATIONS > 0) {
    TopAnimation lastTopAnimation = topParams.animation;
    topParams.animation = TopAnimation(1 + ((uint8_t)topParams.animation % NUM_TOP_ANIMATIONS));
    CleanupTopLayer(lastTopAnimation);
    InitTopLayer();
  }
}

void LEDLoop::OverlayLayers() {
  uint16_t topBrightness;
  for(uint16_t i = 0; i < numLEDs; i++) {
    topBrightness = leds_top_b[i];
    if(topBrightness > 0) {
      uint8_t blendAmount = 255 * topBrightness / (topBrightness + leds_b[i]);
      nblend(leds_top[i], leds[i], blendAmount);
    }
  }
}
