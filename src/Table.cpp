#include "Globals.h"

void setup() {
  #if DEBUG_TIMING
    uint32_t startupTime = SYSTEM_TIME;
  #endif

  delay(STARTUP_DELAY_MS); // sanity delay

  //--------------------Initialize hardware--------------------
  Serial.begin(SERIAL_DATA_RATE_HZ);
  DEBUG("Serial comm intialized.");

  pinMode(BTN1_PIN, INPUT_PULLUP);
  FastLED.addLeds<APA102, LED_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, NUM_LEDS, 0, leds_5bit_brightness);
  //FastLED.addLeds<SK9822, LED_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, NUM_LEDS, 0);
  //FastLED.setBrightness(BRIGHTNESS);
  leds = CRGB::Black;
  leds_top = CRGB::Black;

  FastLED.show();
  DEBUG("LEDs defined and cleared.");
  
  //--------------------Initialize software--------------------
  pm.Init(&(timing.now), INIT_PM_WALK_LENGTH, INIT_PM_PAUSE_LENGTH, INIT_PALETTE);
  DEBUG("PaletteManager init complete.");

  Gamma.Init(&globalBrightness);
  DEBUG("Gamma init complete.");

  InitBaseLayer();
  DEBUG("Base layer init complete.");
  InitTopLayer();
  DEBUG("Top layer init complete.");
  DEBUG("setup() complete.");

  DEBUG_TIMING("setup() time: " + String((uint32_t)(SYSTEM_TIME - startupTime)));

  #ifdef MANUAL_PARAMS
    Serial.setTimeout(100);
    PrintParams();
  #endif

  SkipTime(SYSTEM_TIME);
}

void loop() {
  #ifdef TEST_COLOR_CORRECTION
    for(uint16_t i = 0; i < NUM_LEDS; i++) { leds_b[i] = 255; }
    Gamma.RunTests(leds, leds_b, NUM_LEDS, 4, 32);
    return;
  #endif

  timing.now = SYSTEM_TIME;

  UpdateIO();

  if(timing.now - timing.lastDraw >= FPS_TO_TIME(REFRESH_RATE)) {
    UpdateAnimationParameters(timing.now);
    pm.Update();
    
    #ifdef TEST_PALETTES
      const uint16_t length = 120;
      const uint16_t offset1 = 228;
      const uint16_t offset2 = 390;
      
      const uint16_t pixelsPerPalette = length / PALETTE_SIZE;
      leds = CRGB::Black;
      for(uint16_t i = 0; i < NUM_LEDS; i++) { leds_b[i] = globalBrightness; }
      for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
        // With spaces
        leds(offset1 + pixelsPerPalette*i + 1, offset1 + pixelsPerPalette*(i+1) - 2) = pm.palette[i];
        leds(offset2 + pixelsPerPalette*i + 1, offset2 + pixelsPerPalette*(i+1) - 2) = pm.palette[PALETTE_SIZE-1-i];
        // Without spaces
        //leds(offset1 + pixelsPerPalette*i, offset1 + pixelsPerPalette*(i+1) - 1) = pm.palette[i];
        //leds(offset2 + pixelsPerPalette*i, offset2 + pixelsPerPalette*(i+1) - 1) = pm.palette[PALETTE_SIZE-1-i];
      }
    #else
      TransitionBaseAnimation(timing.now);
      TransitionTopAnimation(timing.now);
      DrawBaseLayer();
      DrawTopLayer();
      OverlayLayers();
    #endif

    Gamma.PrepPixelsForFastLED(leds, leds_b, leds_5bit_brightness, NUM_LEDS);
    FastLED.show();

    timing.lastDraw += FPS_TO_TIME(REFRESH_RATE);
    #ifdef DEBUG_CLIPPING
      if((timing.now > timing.lastDraw + FPS_TO_TIME(REFRESH_RATE)) && (timing.now-lastClippedTime > ONE_SEC)) {
        DEBUG_CLIPPING("ERROR: Drawing clipped by " + (timing.now - timing.lastDraw) + "us")
        lastClippedTime = timing.now;
      }
    #endif
  }

  DEBUG_TIMING("Total loop time = " + String(SYSTEM_TIME - timing.now) + "us");
}

void SkipTime(uint32_t amount) {
  SkipTimeForTimers(amount);
  SkipTimeForIO(amount);
  pm.SkipTime(amount);
  pc.SkipTime(amount);
  #ifdef DEBUG_CLIPPING
    lastClippedTime += amount;
  #endif
}

void SkipTimeForTimers(uint32_t amount) {
  timing.now += amount;
  timing.lastDraw += amount;
  timing.lastBaseParamWalk += amount;
  timing.lastTopParamWalk += amount;
  timing.lastBaseTransition += amount;
  timing.lastTopTransition += amount;
}
