void DrawBaseLayer() {
  switch(baseParams.animation) {
    case SCROLLER:
      pc.Update(baseParams, leds, leds_b, NUM_LEDS, timing.now);
      break;

    case FIRE:
      Fire();
      break;

    case DISCO_FIRE:
      DiscoFire();
      break;

    case COLOR_EXPLOSION:
      ColorExplosion();
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

    default:
      leds = CRGB::Black;
      timing.lastDraw = timing.now;
      break;
  }

  // Handle fading between animations
  leds.fadeToBlackBy(baseTransitionProgress);
}
void InitBaseLayer() {
  #ifdef DEBUG_ANIMATIONS
    Serial.println("Init base layer: " + String(baseParams.animation));
  #endif
  
  switch(baseParams.animation) {
    case SCROLLER:
      pc.Init(baseParams, &pm, &Gamma, timing.now);
      break;

    default:
      break;
  }
}
void CleanupBaseLayer(uint8_t lastAnimation) {
  #ifdef DEBUG_ANIMATIONS
    Serial.println("Cleanup base layer: " + String(lastAnimation));
  #endif
  
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
      ScrollingTwinkle();
      break;
      
    case BOUNCE:
      //Bounce();
      break;
      
    case COMETS:
      Comets();
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
  #ifdef DEBUG_ANIMATIONS
    Serial.println("Init top layer: " + String(topParams.animation));
  #endif
  
  switch(topParams.animation) {
    default:
      break;
  }
}
void CleanupTopLayer(uint8_t lastAnimation) {
  #ifdef DEBUG_ANIMATIONS
    Serial.println("Cleanup top layer: " + String(lastAnimation));
  #endif
  
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

