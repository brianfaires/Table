#include "Globals.h"

// Counters for transitions
uint8_t baseTransitionProgress;
uint8_t topTransitionProgress;


void DrawBaseLayer() {
  switch(baseParams.animation) {
    case SCROLLER:
      pc.Update(baseParams, leds, leds_b);
      break;

    case FIRE:
      Fire();
      break;

    case DISCO_FIRE:
      DiscoFire();
      break;

    case COLOR_EXPLOSION:
      ColorExplosion(timing.now);
      break;

    case ORBS:
      Orbs();
      break;

    case GLIMMER_BANDS:
      ScrollingGlimmerBands();
      break;

    case CENTER_SPAWN:
      CenterSpawn();
      break;

    case STACKS:
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
  DEBUG_ANIMATION("Init base layer: " + String(baseParams.animation));
  
  switch(baseParams.animation) {
    case SCROLLER:
      pc.Init(NUM_LEDS, &(timing.now), baseParams, &pm, &Gamma, allowedDimPeriods, allowedColorPeriods);
      break;

    case STACKS:
      //InitStackers(NUM_LEDS - 18);
      break;
      
    default:
      break;
  }
}
void CleanupBaseLayer(uint8_t lastAnimation) {
  DEBUG_ANIMATION("Cleanup base layer: " + String(lastAnimation));
  
  switch(lastAnimation) {
    default:
      break;
  }
}
void TransitionBaseAnimation(uint32_t curTime) {
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
  uint8_t lastBaseAnimation = baseParams.animation;
  baseParams.animation = 1 + (baseParams.animation % NUM_BASE_ANIMATIONS);
  CleanupBaseLayer(lastBaseAnimation);
  InitBaseLayer();
}

void DrawTopLayer() {
  switch(topParams.animation) {
    case GLITTER:
      Glitter();
      break;
      
    case RAIN:
      Rain();
      break;
    
    case TWINKLE:
      Twinkle();
      break;
      
    case BOUNCE:
      //Bounce();
      break;
      
    case COMETS:
      FourComets(timing.now);
      break;
      
    case COLLISION:
      Collision();
      break;
      
    default:
      leds_top = CRGB::Black;
      break;
  }

  leds_top.fadeToBlackBy(topTransitionProgress);
}
void InitTopLayer() {
  DEBUG_ANIMATION("Init top layer: " + String(topParams.animation));
  
  switch(topParams.animation) {
    default:
      break;
  }
}
void CleanupTopLayer(uint8_t lastAnimation) {
  DEBUG_ANIMATION("Cleanup top layer: " + String(lastAnimation));
  
  switch(lastAnimation) {
    default:
      break;
  }
}
void TransitionTopAnimation(uint32_t curTime) {
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
    uint8_t lastTopAnimation = topParams.animation;
    topParams.animation = 1 + (topParams.animation % NUM_TOP_ANIMATIONS);
    CleanupTopLayer(lastTopAnimation);
    InitTopLayer();
  }
}

void OverlayLayers() {
  for(uint16_t i = 0; i < NUM_LEDS; i++) {
    uint16_t topBrightness = leds_top[i].r + leds_top[i].g + leds_top[i].b;
    if(topBrightness > 0) {
      uint16_t baseBrightness = leds[i].r + leds[i].g + leds[i].b;
      uint8_t blendAmount = 255 * topBrightness / (topBrightness + baseBrightness);
      nblend(leds[i], leds_top[i], blendAmount);
    }
  }
}
