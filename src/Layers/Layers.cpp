#include "Globals.h"

// Counters for transitions
uint8_t baseTransitionProgress;
uint8_t topTransitionProgress;


void DrawBaseLayer() {
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
      ColorExplosion(timing.now);
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
      Stacks(timing.now);
      break;

    default:
      leds = CRGB::Black;
      timing.lastDraw = timing.now;
      break;
  }

  // Handle fading between animations
  leds.fadeToBlackBy(baseTransitionProgress);
}
void InitBaseLayer() {
  DEBUG_ANIMATION("Init base layer: " + baseParams.animation);
  
  switch(baseParams.animation) {
    case BaseAnimation::Scroller:
      pc.Init(NUM_LEDS, &(timing.now), baseParams, &pm, &Gamma, allowedDimPeriods, allowedColorPeriods);
      break;

    case BaseAnimation::Stacks:
      //InitStackers(NUM_LEDS - 18);
      break;
      
    default:
      break;
  }
}
void CleanupBaseLayer(BaseAnimation lastAnimation) {
  DEBUG_ANIMATION("Cleanup base layer: " + lastAnimation);
  
  switch(lastAnimation) {
    default:
      break;
  }
}
void TransitionBaseAnimation(uint32_t curTime) {
  // Currently, not possible to draw 2 animations at once.  One fades out, then one fades in.
  static bool alreadySwitched = false;
  
  if(curTime - timing.lastBaseTransition >= layerConfig.basePauseLength) {
    uint32_t transitionTime = curTime - timing.lastBaseTransition - layerConfig.basePauseLength;
    if(transitionTime < layerConfig.baseTransOutLength) {
      baseTransitionProgress = 256 * transitionTime / layerConfig.baseTransOutLength;
    }
    else if(transitionTime < layerConfig.baseTransOutLength + layerConfig.baseTransInLength) {
      baseTransitionProgress = 255 * (layerConfig.baseTransOutLength + layerConfig.baseTransInLength - transitionTime) / layerConfig.baseTransInLength;
      if(!alreadySwitched) {
        NextBaseAnimation(curTime);
        alreadySwitched = true;
      }
    }
    else {
      alreadySwitched = false;
      baseTransitionProgress = 0;
      timing.lastBaseTransition = curTime;
    }
  }
}
void NextBaseAnimation(uint32_t curTime) {
  BaseAnimation lastBaseAnimation = baseParams.animation;
  baseParams.animation = BaseAnimation(1 + ((uint8_t)baseParams.animation % NUM_BASE_ANIMATIONS));
  CleanupBaseLayer(lastBaseAnimation);
  InitBaseLayer();
}

void DrawTopLayer() {
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
      FourComets(timing.now);
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
void InitTopLayer() {
  DEBUG_ANIMATION("Init top layer: " + topParams.animation);
  
  switch(topParams.animation) {
    default:
      break;
  }
}
void CleanupTopLayer(TopAnimation lastAnimation) {
  DEBUG_ANIMATION("Cleanup top layer: " + lastAnimation);
  
  switch(lastAnimation) {
    default:
      break;
  }
}
void TransitionTopAnimation(uint32_t curTime) {
  // Currently, not possible to draw 2 animations at once.  One fades out, then one fades in.
  static bool alreadySwitched = false;
  
  if(curTime - timing.lastTopTransition >= layerConfig.topPauseLength) {
    uint32_t transitionTime = curTime - timing.lastTopTransition - layerConfig.topPauseLength;
    if(transitionTime < layerConfig.topTransOutLength) {
      topTransitionProgress = 256 * transitionTime / layerConfig.topTransOutLength;
    }
    else if(transitionTime < layerConfig.topTransOutLength + layerConfig.topTransInLength) {
      topTransitionProgress = 255 * (layerConfig.topTransOutLength + layerConfig.topTransInLength - transitionTime) / layerConfig.topTransInLength;
      if(!alreadySwitched) {
        NextTopAnimation(curTime);
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
void NextTopAnimation(uint32_t curTime) {
  if(NUM_TOP_ANIMATIONS > 0) {
    TopAnimation lastTopAnimation = topParams.animation;
    topParams.animation = TopAnimation(1 + ((uint8_t)topParams.animation % NUM_TOP_ANIMATIONS));
    CleanupTopLayer(lastTopAnimation);
    InitTopLayer();
  }
}

void OverlayLayers() {
  uint16_t topBrightness;
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    topBrightness = leds_top_b[i];
    if(topBrightness > 0) {
      uint8_t blendAmount = 255 * topBrightness / (topBrightness + leds_b[i]);
      nblend(leds_top[i], leds[i], blendAmount);
    }
  }
}
