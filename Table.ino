/* TODO:
 *  Future ideas
 *  **Use hardware SPI pins
 *    Blend out/in animations at same time
 *    Overlay blendAmount: base on Luma(), or brightness of top layer only
 *    PaletteManager random walk palettes
 *    Use fastmath function scale8() in  scaleParam() functions
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
 *    - Make Serial IO menu 2/3 columns
 *    - Make a SnakeOffset with half spacing; Fade between snake, snake2, and snakeOffset
 *    - Check patterns for fades that should end in full brightness.  That needs a SINGLE_BRIGHT() endcap.
 *    - Bug: param changes while blending
 *    - Bug: changing speed up at tail end of a move moves several times (catching up on time since last move)
 *    - Blending between patterns between perdiods that are factors
 *    - Blending params based on time
 *    - PatternController library
 *    - Tune Palettes
 *      
 *      
 *      
 *  Notes:
 *    - When blending from one pattern to another, if a new target is selected, blend timer resets and moves from current point.  Could instead wait for blending to complete, pause, then blend again.
 *    - When blending between patterns, only the target pattern's params are updated - cannot always know the pattern or combination of patterns that led to the last pattern
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
  FastLED.addLeds<APA102, LED_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, NUM_LEDS, 0, leds_b, colorCorrections, &globalBrightness, gammaDim, gammaDim_5bit);
  //FastLED.addLeds<APA102, LED_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, NUM_LEDS, 0).setCorrection(COLOR_CORRECTION);
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
      Serial.println((uint32_t)(micros() - startupTime));
    #endif
  
  #endif


  #ifdef MANUAL_PARAMS
    Serial.setTimeout(100);
    PrintParams();
  #endif
  
  #ifdef SKIP_TIME_ON_SERIAL_WRITE
    SkipTime(micros());
  #endif
}

void loop() {
  #ifdef TEST_COLOR_CORRECTION
    for(uint16_t i = 0; i < NUM_LEDS; i++) { leds_b[i] = 255; }
    Gamma.RunTests(leds, leds_b, 384, 4, 32);
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
      if(timing.now - timing.lastBaseParamWalk >= layerConfig.baseParamWalkTime) {
        WalkBaseParams();
        timing.lastBaseParamWalk = timing.now;
      }
      if(timing.now - timing.lastTopParamWalk >= layerConfig.topParamWalkTime) {
        WalkTopParams();
        timing.lastTopParamWalk = timing.now;
      }
    #endif 


    pm.Update(timing.now);
    
    #ifdef TEST_PALETTES
      leds = CRGB::Black;
      for(uint16_t i = 0; i < NUM_LEDS; i++) { leds_b[i] = globalBrightness; }
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
    //if(timing.now > timing.lastDraw + US_BETWEEN_DRAWS) { THROW("ERROR: Drawing clipped by " + String(timing.now - timing.lastDraw) + "us") }
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

