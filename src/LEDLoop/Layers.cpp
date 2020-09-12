#include "LEDLoop/LEDLoop.h"

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
  static int activeTransition = 0;

  // First, identify transition type.  Default is currently a slow fade out and in.
  BaseAnimation nextBase = PeekNextBaseAnimation();
  if(activeTransition == 1 || (baseParams.animation == BaseAnimation::Stacks && nextBase == BaseAnimation::Scroller)) {
    if(timing.now - timing.lastBaseTransition < layerConfig.basePauseLength) { return; }
    if(uint8_t(stackers.transitionState) != 2) { return; }
    if(stackers.stackLength < 9) { return; } // Below min size for dimPatterns
    uint16_t dimPeriod = numLEDs / stackers.numStacks; //allowedDimPeriods[scaleParam(baseParams.dimPeriod, 0, NUM_ALLOWED_DIM_PERIODS-1)];
    if(stackers.stackLength > dimPeriod-3) { return; } // Above max size for dimPatterns (and a little extra to avoid overflowing scaled params)

    static int transitionPhase = 0;

    if(transitionPhase == 0) {
      activeTransition = 1;
      // Assume a consistent dimPattern coming from stackers;  Initialize dimPattern to match period, length, and offset
      uint16_t maxExtraPixels = dimPeriod - 10; // 10 for the min dimPattern and spacing
      baseParams.brightLength = (stackers.stackLength - 9) * 65536L / maxExtraPixels; // Scaled based on the number of extra pixels needed
      baseParams.transLength = baseParams.brightLength;
      baseParams.colorPeriod = 255;
      uint8_t scaledDown = scale16((dimPeriod-10)/3 + 1, baseParams.brightLength);
      uint8_t moarPix = stackers.stackLength - (9 + 3*scaledDown);
      if(moarPix == 1) { baseParams.brightLength += (3 * 65536L / maxExtraPixels); }
      else if(moarPix == 2) { baseParams.transLength += (3 * 65536L / maxExtraPixels); }

      //baseParams.brightLength = 0xFFFF / periodLength * stackers.stackLength;
      //uint8_t denom = (1 + (periodLength - 10 + 2) / 3); // +2 is to force a round up
      //baseParams.brightLength = (65536L * stackers.stackLength / 2) / denom; // Reversed equation for setting brightLength

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

  //DUMP(minStack)
      // Initialize colorPattern to match the existing colors
      uint8_t colorIndexes[stackers.numStacks];
      for(int i = 0; i < stackers.numStacks; i++) {
        uint8_t colorIndex = stackers.stacks[(maxStack + i) % stackers.numStacks].color;
        colorIndexes[i] = colorIndex;
        //DUMP(stackers.stacks[(minStack + i) % stackers.numStacks].pixel)
      }
      
      pc.SetManualBlocks(colorIndexes, stackers.numStacks, dimPeriod);
      pc.syncScrollingSpeeds = true;
      //pc.SetManualControl(true);

      // Set displayMode to snake and manualBlocks
      baseParams.displayMode = pc.GenerateDisplayModeValue(DimPatternName::Snake, ColorPatternName::ManualBlocks);

      // Init speed to match
      baseParams.dimSpeed = stackers.GetScaledDimSpeed();
      if(stackers.moveClockwise) { baseParams.dimSpeed *= -1; }
          
      baseParams.animation = PeekNextBaseAnimation();
      InitBaseLayer();
      
      pc.setDimIndexOffset((dimPeriod + numLEDs - stackers.stacks[0].pixel) % dimPeriod); // Not covered in Init()
      pc.setColorIndexOffset((dimPeriod + numLEDs - stackers.stacks[0].pixel) % dimPeriod); // Not covered in Init()

      pc.SetManualBlocks(colorIndexes, stackers.numStacks, dimPeriod); // Called twice to override what was done in Init()

      transitionPhase = 1;
    }
    else if(transitionPhase == 1) {
      // Blend into desired color pattern
      static uint32_t transStartTime = 0;
      if(transStartTime == 0) {
        baseParams.displayMode = pc.GenerateDisplayModeValue(DimPatternName::Snake, ColorPatternName::Gradient); // Todo: what to default to?
        pc.BeginColorBlend();
        transStartTime = timing.now;
      }
      else if(timing.now - transStartTime >= pc.getColorBlendLength()) {
        transStartTime = 0;
        transitionPhase = 2;
      }
    }
    else if(transitionPhase == 2) {
      // Begin pattern blending
      pc.syncScrollingSpeeds = false;
      baseParams.displayMode = pc.GenerateDisplayModeValue(DimPatternName::Random, ColorPatternName::Gradient); // Todo: what to default to?
      pc.BeginDimBlend();

      timing.lastBaseTransition = timing.now;
      transitionPhase = 0;
      activeTransition = 0;
    }
    
    return;
  }
  else if(baseParams.animation == BaseAnimation::Scroller && nextBase == BaseAnimation::Stacks) {
    // Blend to fixed colors per dimPeriod
    // Switch to blocks
    // Create stacks to match and hard swap

    return;
  }


  // ------------ Default - Fade transition --------------------
  // Currently, not possible to draw 2 animations at once.  One fades out, then one fades in.
  static bool alreadySwitched = false;
  
  if(timing.now - timing.lastBaseTransition >= layerConfig.basePauseLength) {
    uint32_t transitionTime = timing.now - timing.lastBaseTransition - layerConfig.basePauseLength;
    if(transitionTime < layerConfig.baseTransOutLength) {
      baseTransitionProgress = 256 * transitionTime / layerConfig.baseTransOutLength;
    }
    else if(transitionTime < layerConfig.baseTransOutLength + layerConfig.baseTransInLength) {
      baseTransitionProgress = 255 * (layerConfig.baseTransOutLength + layerConfig.baseTransInLength - transitionTime) / layerConfig.baseTransInLength;
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
