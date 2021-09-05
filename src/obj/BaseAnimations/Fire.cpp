#include "obj/LEDLoop.h"
#include "Definitions.h"

const uint8_t FIRE_COOLING = 7;
const uint16_t FIRE_SPARKING = 4000;
const uint8_t FIRE_MIN_SPARK_SIZE = 160;

void LEDLoop::FireGlitter() {
  static CRGBPalette16 gPal = HeatColors_p;
  static byte heat[MAX_LEDS];
  static byte prevHeat[MAX_LEDS];

  CreateFirePalette(&gPal);
  
  // Step 1.  Cool down every cell a little
  for(int i = 0; i < numLEDs; i++) {
    heat[i] = qsub8( heat[i],  random8(0, FIRE_COOLING));
  }
  
  // Step 2.  Heat from each cell diffuses a little
  heat[0] = (prevHeat[numLEDs-1] + prevHeat[0] + prevHeat[0] + prevHeat[1] ) / 4;
  heat[numLEDs-1] = (prevHeat[numLEDs-2] + prevHeat[numLEDs-1] + prevHeat[numLEDs-1] + prevHeat[0] ) / 4;
  for(int i = 2; i < numLEDs; i++) {
    heat[i-1] = (prevHeat[i] + prevHeat[i-1] + prevHeat[i-1] + prevHeat[i-2] ) / 4;
  }
  
  // Step 3.  Randomly ignite new 'sparks' of heat
  for(int i = 0; i < numLEDs; i++) {
    if(random16() < FIRE_SPARKING) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(FIRE_MIN_SPARK_SIZE, 255) );
    }
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < numLEDs; j++) {
    // Scale the heat value from 0-255 down to 0-240 for best results with color palettes.
    byte colorindex = scale8( heat[j], 240);
    CRGB color = ColorFromPalette( gPal, colorindex);
    
    leds[j] = color;
  }
}

void LEDLoop::Fire() {
  static bool gReverseDirection = false;
  
  static CRGBPalette16 gPal = HeatColors_p;
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
  CreateFirePalette(&gPal);

  static byte heat[MAX_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < numLEDs; i++) {
      heat[i] = qsub8( heat[i],  random8(0, FIRE_COOLING));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= numLEDs - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < FIRE_SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(FIRE_MIN_SPARK_SIZE, 255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < numLEDs; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (numLEDs-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}


void LEDLoop::CreateFirePalette(CRGBPalette16 *firePalette) {
  CRGB steps[3];
  hsv2rgb_rainbow(pm->palette[0], steps[0]);
  hsv2rgb_rainbow(pm->palette[2], steps[1]);
  hsv2rgb_rainbow(pm->palette[4], steps[2]);

  steps[0].maximizeBrightness();
  steps[1].maximizeBrightness();

  steps[0] %= 64; // Set to 1/4 of max brightness
  steps[1] %= 64;// Set to 1/4 of max brightness

  while(steps[0].r > 25 && steps[0].g > 25 && steps[0].b > 25) { steps[0]--; } // Manually increase saturation
  //while(steps[2].r < 255 && steps[2].g < 255 && steps[3].b < 255) { steps[2]++; }
  steps[2] |= CRGB(40, 40, 40); // Raise each channel to a minimum of 40
  steps[2].maximizeBrightness();
  steps[2] %= 64;// Set to 1/4 of max brightness

  CRGB start, target;

  // Step 1: Black to dim, saturated color0
  target = steps[0];
  for(uint8_t i = 0; i < 90; i++) {
    start = CRGB::Black;
    nblend(start, target, 255 * i / 90);
    firePalette->entries[i] = start;
  }

  // Step 2: color0 to color1, up value and drop saturation
  target = steps[1];
  for(uint8_t i = 90; i < 160; i++) {
    start = steps[0];
    nblend(start, target, 255 * (i-90) / 70);
    firePalette->entries[i] = start;
  }

  // Step 3: color1 to color2, up value and drop saturation
  target = steps[2];
  for(uint8_t i = 160; i < 240; i++) {
    start = steps[1];
    nblend(start, target, 255 * (i-160) / 80);
    firePalette->entries[i] = start;
  }

  // Step 4: blend into (almost) fully bright white
  target = CRGB(255, 240, 230);
  nblend(target, steps[2], 100); // ~60% white
  for(uint16_t i = 240; i < 256; i++) {
    start = steps[2];
    nblend(start, target, 255 * (i-240) / 16);
    firePalette->entries[i] = start;
  }
}

void LEDLoop::DiscoFire() {
  
}
