/* Several single function Top layer animations
*/
#include "obj/LEDLoop.h"

void LEDLoop::Glitter() {
  static uint32_t lastGlitter = 0;

  int8_t refreshRate = scaleParam(2*abs(topParams.speed)-1, 9, 13);
  uint16_t numLitLEDs = scaleParam(topParams.portion, 3, 16);
  
  if(timing.now - lastGlitter > FPS_TO_TIME(refreshRate)) {
    leds_top = CRGB::Black;
    lastGlitter = timing.now;
    for(uint16_t i = 0; i < numLitLEDs; i++) {
      uint16_t randIdx = random16(numLEDs);
      leds_top[randIdx] = HSV2RGB(pm->palette[5], leds_top_b[randIdx]);
    }
  }
}

void LEDLoop::Rain() {
  static uint32_t lastMove = 0;
  
  uint8_t portion = scaleParam(topParams.portion, 8, 15);
  int8_t moveSpeed = scaleParamSigned(2*abs(topParams.speed)-1, 7, 50);
  if(topParams.speed < 0) { moveSpeed *= -1; }
  
  if(timing.now - lastMove > FPS_TO_TIME(abs(moveSpeed))) {
    lastMove = timing.now;
    
    if(moveSpeed > 0) {
      for(uint16_t i = numLEDs-1; i > 0; i--) {
        leds_top[i] = leds_top[i-1];
        leds_top_b[i] = leds_top_b[i-1];
      }
      if(random8(portion) == 0) {
        leds_top[0] = HSV2RGB(pm->palette[5], leds_top_b[0]);
      }
      else {
        leds_top[0] = CRGB::Black;
      }
    }
    else {
      for(uint16_t i = 0; i < numLEDs-1; i++) {
        leds_top[i] = leds_top[i+1];
        leds_top_b[i] = leds_top_b[i+1];
      }
      if(random8(portion) == 0) {
        leds_top[numLEDs-1] = HSV2RGB(pm->palette[5], leds_top_b[numLEDs-1]);
      }
      else {
        leds_top[numLEDs-1] = CRGB::Black;
      }
    }
  }
}

void LEDLoop::Twinkle() {
  uint8_t spawnRate = scaleParam(topParams.portion, 1, 20);
  uint8_t growRate = 2*scaleParam(2*abs(topParams.speed)-1, 1, 8);

  for(uint16_t i = 0; i < numLEDs; i++) {
    if(leds_top_b[i] % 2 == 1) {
      if(leds_top_b[i] < 255 - growRate) { leds_top_b[i] += growRate; }
      else { leds_top_b[i] = 254; }
    }
    else if(leds_top_b[i] > 0) {
      if(leds_top_b[i] <= growRate) { leds_top_b[i] = 0; }
      else { leds_top_b[i] -= growRate; }
    }
  }

  for(uint8_t i = 0; i < spawnRate; i++) {
    uint16_t index;
    do { index = random16(numLEDs); } while(leds_top_b[i] > 0);
    leds_top_b[index] = 1;
    leds_top[index] = pm->palette[PALETTE_SIZE-1];
  }
}

/*
//--------------------- Bounce ---------------------
#define SERIAL_DEBUG_BOUNCY
typedef struct {
  uint16_t loc;
  int8_t vel;
  CRGB color;
} Bouncy;

void LEDLoop::Bounce() {
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

void LEDLoop::SpawnNewBouncy() {
  uint8_t i;
  for(i = 0; i < MAX_BOUNCIES; i++) {
    if(!objects[i]) {
      objects[i] = malloc(sizeof(Bouncy));
      ((Bouncy*)objects[i])->loc = random8(MIN_BOUNCY_SPAWN_HEIGHT, MAX_BOUNCY_SPAWN_HEIGHT);
      ((Bouncy*)objects[i])->vel = 0;
      ((Bouncy*)objects[i])->color = pm->palette.entries[8 + 2*i]; // This requires MAX_BOUNCIES to be <= 4
      #ifdef SERIAL_DEBUG_BOUNCY
      PRINT("Spawning Bouncy #");
      PRINTLN(i);
      PRINT("\tloc=");
      PRINT(((Bouncy*)objects[i])->loc);
      PRINT("\tvel=");
      PRINTLN(((Bouncy*)objects[i])->vel);
      #endif
    }
  }
}
*/

void LEDLoop::FourComets() {
  const uint8_t NUM_COMETS = 4;
  static struct_comet comets[NUM_COMETS];
  static uint16_t moveIndex = 0;
  static uint32_t lastMove = timing.now;
  
  uint8_t cometLength = scaleParam(topParams.portion, 8, 24);
  uint8_t cometSpeed = scaleParam(2*abs(topParams.speed)-1, 8, 30);

  if(comets[0].startPos == comets[1].startPos) {
    // Init
    comets[0].startPos = 0;
    comets[0].moveForward = true;

    comets[1].startPos = 173;
    comets[1].moveForward = false;

    comets[2].startPos = 210;
    comets[2].moveForward = true;

    comets[3].startPos = 333;
    comets[3].moveForward = false;
  }

  for(uint8_t i = 0; i < NUM_COMETS; i++) {
    DrawComet(&comets[i], cometLength, moveIndex);
  }

  if(timing.now - lastMove > FPS_TO_TIME(cometSpeed)) {
    moveIndex++;
    if(moveIndex == 87) {
      moveIndex = 0;
    }
  }
}
void LEDLoop::DrawComet(struct_comet* comet, uint8_t cometLength, uint16_t moveIndex) {
  uint16_t curPixel;
  if(comet->moveForward) { curPixel = (comet->startPos + moveIndex) % numLEDs; }
  else { curPixel = (comet->startPos - moveIndex + numLEDs) % numLEDs; }

  if(comet->moveForward) {
    for(uint8_t i = 0; i < cometLength; i++) {
      leds_top[curPixel] = pm->palette[PALETTE_SIZE-1];
      leds_top[curPixel++] = 255 - 255*i/cometLength;
      if(curPixel == numLEDs) { curPixel = 0; }
      if(curPixel == comet->startPos) { break; }
    }
  }
  else {
    for(uint8_t i = 0; i < cometLength; i++) {
      leds_top[curPixel] = pm->palette[PALETTE_SIZE-1];
      leds_top[curPixel--] = 255 - 255*i/cometLength;
      if(curPixel == 0xFFFF) { curPixel = numLEDs-1; }
      if(curPixel == comet->startPos) { break; }
    }
  }
}
