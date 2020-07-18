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
  baseParams.animation = BaseAnimation(1 + ((uint8_t)baseParams.animation % NUM_BASE_ANIMATIONS));
  CleanupBaseLayer(lastBaseAnimation);
  InitBaseLayer();
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
