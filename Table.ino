/* TODO:
 *  Future ideas
 *    Blend out/in animations at same time
 *    Overlay blendAmount: base on Luma(), or brightness of top layer only
 *    PatternScroller reverseDirection: overwrite random pixels at the end
 *  
 *  Button IO:
 *    Medium-long press into tap doesn't do double click
 *    Double-click long hold throws away first click
 *    Sets values to 0 to signal off; this will error out 1us every 71 hours
 *    
 *  Patterns:
 *    Things that change/twinkle as they scroll
 *    Patterns that set the color of their pixels
 *  
 *  To do:
 *    Test 71-minute time rollover
 *    Base animations - based on time
 *    Top animations - based on time
 *    Make standard snake work with blackness; overlayed with dimming pattern too maybe
 *    Serial output: buffer large outputs to avoid one big delay
 *    
 *  Bugs:
 *    Flickering happens at 100FPS and greater. No idea why.
 *    
 *    
 *  To do:
 *    - Confirm final Gamma stuff
 *    - Test PatternRepeater, get it into a library
 *    - Test/Tune PatternGenerator
 *    - Tune Palettes
 *    - Test PatternScroller, get it into a library
 *      
 */

#include <PaletteManager.h>
#include <FastLED.h>
#include "A_Globals.h"

uint8_t baseTransitionProgress;
uint8_t topTransitionProgress;

extern const uint8_t gamma8[];
extern const uint8_t reverseGamma8[];
void setup() {
  #ifdef DEBUG_TIMING
    uint32_t startupTime = micros();
  #endif

  delay(STARTUP_DELAY_MS); // sanity delay

  //--------------------Initialize hardware--------------------
  #ifdef DEBUG_SERIAL
    Serial.begin(SERIAL_DATA_RATE_HZ);
    Serial.println("Serial comm intialized.");
  #endif

  pinMode(BTN1_PIN, INPUT_PULLUP);
  FastLED.addLeds<APA102, LED_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, NUM_LEDS).setCorrection(COLOR_CORRECTION);
  //FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.setBrightness(BRIGHTNESS);
  leds = CRGB::Black;
  FastLED.show();
  #ifdef DEBUG_SERIAL
    Serial.println("LEDs defined and cleared.");
  #endif
  
  //--------------------Initialize software--------------------
  pm.Init(INIT_PM_WALK_LENGTH, INIT_PM_PAUSE_LENGTH, timing.now);
  InitBaseLayer();
  InitTopLayer();
  
  #ifdef DEBUG_SERIAL
    Serial.println("setup() complete.");
    #ifdef DEBUG_TIMING
      Serial.print("setup() time: ");
      Serial.println((uint32_t)(micros() - startupTime));
    #endif
  
  #endif


  #ifdef MANUAL_PARAMS
    Serial.setTimeout(100);
    PrintParams();
  #endif
  
  SkipTime(micros());
}

void loop() {
  #ifdef SIMPLE_COLOR_CORRECTION_TEST
    RunSimpleColorCorrectionTest();
    return;
  #elif defined(GRADIENT_COLOR_CORRECTION_TEST)
    RunGradientColorCorrectionTest();
    return;
  #endif

  
  timing.now = micros();

  UpdateIO();

  if(timing.now - timing.lastDraw >= US_BETWEEN_DRAWS) {
    // Update animation parameters
    #ifdef MANUAL_PARAMS
      if(ProcessSerialInput()) { 
        PrintParams();
        SkipTime(micros() - timing.now);
      }
      
    #else
      if(timing.now - timing.lastBaseParamWalk >= config.baseParamWalkTime) {
        WalkBaseParams();
        timing.lastBaseParamWalk = timing.now;
      }
      if(timing.now - timing.lastTopParamWalk >= config.topParamWalkTime) {
        WalkTopParams();
        timing.lastTopParamWalk = timing.now;
      }
    #endif 


    pm.Update(timing.now);
    
    #ifdef TEST_PALETTES
      leds = CRGB::Black;
      uint8_t pixelsPerPalette = NUM_LEDS / PALETTE_SIZE;
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
    timing.lastDraw += US_BETWEEN_DRAWS;
    #ifdef DEBUG_ERRORS
      if(timing.now > timing.lastDraw + US_BETWEEN_DRAWS) {
        Serial.println("ERROR: Drawing clipped by " + String(timing.now - timing.lastDraw) + "us");
      }
    #endif
  }

  #ifdef DEBUG_TIMING
    Serial.println("Total loop time = " + String(micros() - timing.now) + "us");
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

