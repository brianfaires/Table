/* TODO:
 *  Future ideas
 *  **Use hardware SPI pins
 *    Blend out/in animations at same time
 *    Overlay blendAmount: base on Luma(), or brightness of top layer only
 *    PaletteManager random walk palettes
 *    Use fastmath function scale8() in scaleParam() functions
 *  
 *  Button IO:
 *    - Medium-long press into tap doesn't do double click
 *    - Double-click long hold throws away first click
 *    - Sets values to 0 to signal off; this will error out 1us every 71 hours
 *    
 *  Patterns:
 *    - changeType not remembered when oscillating params through PatternController
 *    - Only WORM doesn't flicker when decoupled from movement
 *    - Bright spots on ends look weird cause they draw attention and are frozen
 *    Things that change/twinkle as they scroll
 *    Patterns that set the color of their pixels
 *    Animations that fade in/out of subsets (like 3comets into 1 comet)
 *    Blending between patterns with perdiods that are factors
 *    
 *  To do:
 *    Test 71-minute time rollover
 *    Base animations - based on time
 *    Top animations - based on time
 *    Make standard snake work with blackness; overlayed with dimming pattern too maybe
 *    Serial output: buffer large outputs to avoid one big delay
 *    
 *  To do:
 *    - PatternScroller and PatternController library(s)
 *    - Tune Palettes
 *    - Get rid of int8_t's for baseParams
 *      
 */

#include <PaletteManager.h>
#include <FastLED.h>
#include "A_Globals.h"

void setup() {
  #ifdef DEBUG_TIMING
    uint32_t startupTime = SYSTEM_TIME;
  #endif

  delay(STARTUP_DELAY_MS); // sanity delay

  //--------------------Initialize hardware--------------------
  #ifdef DEBUG_SERIAL
    Serial.begin(SERIAL_DATA_RATE_HZ);
    Serial.println("Serial comm intialized.");
  #endif

  pinMode(BTN1_PIN, INPUT_PULLUP);
  FastLED.addLeds<APA102, LED_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, NUM_LEDS, 0, leds_b, colorCorrections, &globalBrightness, gammaDim, gammaDim_5bit);
  //FastLED.addLeds<SK9822, LED_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, NUM_LEDS, 0);
  //FastLED.setBrightness(BRIGHTNESS);
  leds = CRGB::Black;
  leds_top = CRGB::Black;

  FastLED.show();
  #ifdef DEBUG_SERIAL
    Serial.println("LEDs defined and cleared.");
  #endif
  
  //--------------------Initialize software--------------------
  pm.Init(INIT_PM_WALK_LENGTH, INIT_PM_PAUSE_LENGTH, timing.now);
  #ifdef DEBUG_SERIAL
    Serial.println("PaletteManager init complete.");
  #endif

  Gamma.Init(gammaR, gammaG, gammaB, reverseGammaR, reverseGammaG, reverseGammaB, &globalBrightness);
  #ifdef DEBUG_SERIAL
    Serial.println("Gamma init complete.");
  #endif

  InitBaseLayer();
  InitTopLayer();
  #ifdef DEBUG_SERIAL
    Serial.println("Layer init complete.");
  #endif

  #ifdef DEBUG_SERIAL
    Serial.println("setup() complete.");
    #ifdef DEBUG_TIMING
      Serial.print("setup() time: ");
      Serial.println((uint32_t)(SYSTEM_TIME - startupTime));
    #endif
  
  #endif

  #ifdef MANUAL_PARAMS
    Serial.setTimeout(100);
    PrintParams();
  #endif

  SkipTime(SYSTEM_TIME);
}

void loop() {
  #ifdef TEST_COLOR_CORRECTION
    for(uint16_t i = 0; i < NUM_LEDS; i++) { leds_b[i] = 255; }
    Gamma.RunTests(leds, leds_b, 384, 4, 32);
    return;
  #endif

  
  timing.now = SYSTEM_TIME;

  UpdateIO();

  if(timing.now - timing.lastDraw >= FPS_TO_TIME(REFRESH_RATE)) {
    UpdateAnimationParameters(timing.now);
    pm.Update(timing.now);
    
    #ifdef TEST_PALETTES
      leds = CRGB::Black;
      for(uint16_t i = 0; i < 120; i++) { leds_b[i] = globalBrightness; }
      uint8_t pixelsPerPalette = 120 / PALETTE_SIZE;
      for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
        //leds(pixelsPerPalette*i + 1, pixelsPerPalette*(i+1) - 2) = pm.palette[i]; // With spaces
        leds(pixelsPerPalette*i, pixelsPerPalette*(i+1) - 1) = pm.palette[i];
      }
    #else
      TransitionBaseAnimation(timing.now);
      TransitionTopAnimation(timing.now);
      DrawBaseLayer();
      DrawTopLayer();
      OverlayLayers();
    #endif

    FastLED.show();
    timing.lastDraw += FPS_TO_TIME(REFRESH_RATE);
    //if(timing.now > timing.lastDraw + FPS_TO_TIME(REFRESH_RATE)) { THROW("ERROR: Drawing clipped by " + String(timing.now - timing.lastDraw) + "us") }
  }

  #ifdef DEBUG_TIMING
    Serial.println("Total loop time = " + String(SYSTEM_TIME - timing.now) + "us");
  #endif
}

void SkipTime(uint32_t amount) {
  SkipTimeForTimers(amount);
  SkipTimeForIO(amount);
  pm.SkipTime(amount);
  pc.SkipTime(amount);
}

void SkipTimeForTimers(uint32_t amount) {
  timing.now += amount;
  timing.lastDraw += amount;
  timing.lastBaseParamWalk += amount;
  timing.lastTopParamWalk += amount;
  timing.lastBaseTransition += amount;
  timing.lastTopTransition += amount;
}

