void Glitter() {
  static uint32_t lastGlitter = 0;

  int8_t refreshRate = scaleParam(topParams.speed, 9, 13);
  uint16_t numLitLEDs = scaleParam(topParams.portion, 3, 16);
  
  if(timing.now - lastGlitter > FPS_TO_TIME(refreshRate)) {
    leds_top = CRGB::Black;
    lastGlitter = timing.now;
    for(uint16_t i = 0; i < numLitLEDs; i++) {
      uint16_t randIdx = random16(NUM_LEDS);
      leds_top[randIdx] = HSV2RGB(pm.palette[5], leds_top_b[randIdx]);
    }
  }
}

void Rain() {
  static uint32_t lastMove = 0;
  
  uint8_t portion = scaleParam(topParams.portion, 8, 15);
  int8_t refreshRate = scaleParam(topParams.speed, -50, 50);
  if(refreshRate <= 0 && refreshRate > -7) { refreshRate = -7; }
  else if(refreshRate >= 0 && refreshRate < 7) { refreshRate = 7; }

  
  if(timing.now - lastMove > FPS_TO_TIME(abs(refreshRate))) {
    lastMove = timing.now;
    
    if(refreshRate > 0) {
      for(uint16_t i = NUM_LEDS-1; i > 0; i--) {
        leds_top[i] = leds_top[i-1];
        leds_top_b[i] = leds_top_b[i-1];
      }
      if(random8(portion) == 0) {
        leds_top[0] = HSV2RGB(pm.palette[5], leds_top_b[0]);
      }
      else {
        leds_top[0] = CRGB::Black;
      }
    }
    else {
      for(uint16_t i = 0; i < NUM_LEDS-1; i++) {
        leds_top[i] = leds_top[i+1];
        leds_top_b[i] = leds_top_b[i+1];
      }
      if(random8(portion) == 0) {
        leds_top[NUM_LEDS-1] = HSV2RGB(pm.palette[5], leds_top_b[NUM_LEDS-1]);
      }
      else {
        leds_top[NUM_LEDS-1] = CRGB::Black;
      }
    }
  }
}

void ScrollingTwinkle() {
  
}

/*
//--------------------- Bounce ---------------------
#define SERIAL_DEBUG_BOUNCY
typedef struct {
  uint16_t loc;
  int8_t vel;
  CRGB color;
} Bouncy;

void Bounce() {
  const uint8_t MAX_BOUNCY_SPAWN_HEIGHT = 100;
  const uint8_t MIN_BOUNCY_SPAWN_HEIGHT = 40;
  const uint8_t MAX_BOUNCIES = 4;

  const uint8_t spawnProb = 2;
  const uint8_t fadeRate = 196;
  const uint8_t bouncyHeight = 4;
  const uint8_t bounceVelLoss = 5;

  bool foundOne = false;
  for(uint8_t i = 0; i < MAX_BOUNCIES; i++) {
    if(objects[i]) {
      foundOne = true;
      Bouncy* b = (Bouncy*)objects[i];
      if(b->vel > 0) {
        b->loc += b->vel / 10 + 1;
      }
      else if(b->vel < 0) {
        b->loc += b->vel / 10 - 1;
      }
      b->vel--;
      if(b->loc == 0 || b->loc > MAX_BOUNCY_SPAWN_HEIGHT) { // at 0 or overflowed
        b->loc = 0;
        b->vel = b->vel *-1;
        if(b->vel < 2*bounceVelLoss) {
          free(objects[i]);
          objects[i] = NULL;
        }
        else {
          b->vel -= bounceVelLoss;
        }
      }
    }
  }

  if(!foundOne || random8() < spawnProb) SpawnNewBouncy();

  for(CRGB & pixel : leds) { pixel.fadeToBlackBy(fadeRate); }

  // Draw Bouncies
  for(uint8_t i = 0; i < MAX_BOUNCIES; i++) {
    if(objects[i]) {
      Bouncy* b = (Bouncy*)objects[i];
      for(CRGB & pixel : leds(b->loc, b->loc + bouncyHeight - 1)) { pixel += b->color; }
    }
  }
}

void SpawnNewBouncy() {
  uint8_t i;
  for(i = 0; i < MAX_BOUNCIES; i++) {
    if(!objects[i]) {
      objects[i] = malloc(sizeof(Bouncy));
      ((Bouncy*)objects[i])->loc = random8(MIN_BOUNCY_SPAWN_HEIGHT, MAX_BOUNCY_SPAWN_HEIGHT);
      ((Bouncy*)objects[i])->vel = 0;
      ((Bouncy*)objects[i])->color = pm.palette.entries[8 + 2*i]; // This requires MAX_BOUNCIES to be <= 4
      #ifdef SERIAL_DEBUG_BOUNCY
      Serial.print("Spawning Bouncy #");
      Serial.println(i);
      Serial.print("\tloc=");
      Serial.print(((Bouncy*)objects[i])->loc);
      Serial.print("\tvel=");
      Serial.println(((Bouncy*)objects[i])->vel);
      #endif
    }
  }
}
*/

void Comets() {
  
}


