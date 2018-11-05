// Base animations
const uint8_t FIRE_COOLING = 7;
const uint8_t FIRE_SPARKING = 120;
const uint8_t FIRE_MIN_SPARK_SIZE = 160;

void Fire() {
  static bool gReverseDirection = false;
  
  static CRGBPalette16 gPal = HeatColors_p;
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  //   static CRGBPalette16 gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
  CreateFirePalette(&gPal);

  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, FIRE_COOLING));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < FIRE_SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(FIRE_MIN_SPARK_SIZE, 255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void CreateFirePalette(CRGBPalette16 *firePalette) {
  CRGB steps[3];
  hsv2rgb_rainbow(pm.palette[0], steps[0]);
  hsv2rgb_rainbow(pm.palette[2], steps[1]);
  hsv2rgb_rainbow(pm.palette[4], steps[2]);

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


// Under development

void ScrollingSaturation(uint8_t nWhite, uint8_t nTrans, uint8_t nPure) {
  const uint8_t maxBlendAmount = 160;
  //const uint8_t whiteBrightness = 210;

  uint8_t period = nWhite + nPure + 2*nTrans;
  uint16_t extendedLEDCount = ((NUM_LEDS-1)/period+1)*period;
  //CRGB myWhite = CRGB(whiteBrightness, whiteBrightness, whiteBrightness);
  uint16_t i = 0;
  while(i < extendedLEDCount) {
    // progressively blend the LEDs toward white
    for(uint16_t j = 0; j < period - nPure; j++) {
      int16_t offset = 0;//baseCount*THROTTLE_MID_POINT/baseSpeed2;
      uint16_t idx = (i + offset) % extendedLEDCount;
      if(i++ >= extendedLEDCount) { return; }
      if(idx >= NUM_LEDS) continue;


      if(j >= 2*nTrans + nWhite) {
        // skip over nPure LEDs to leave them at full saturation
        i += nPure;
      }
      else {
        uint8_t whiteBrightness = (leds[idx].r + leds[idx].g + leds[idx].b) / 3;
        CRGB myWhite = CRGB(whiteBrightness, whiteBrightness, whiteBrightness);
        
        if(j < nTrans)
          nblend(&leds[idx], &myWhite, 1, maxBlendAmount * j / nTrans);
        else if(j < nTrans + nWhite)
          nblend(&leds[idx], &myWhite, 1, maxBlendAmount);
        else //if(j < 2*nTrans + nWhite)
          nblend(&leds[idx], &myWhite, 1, maxBlendAmount * (2*nTrans + nWhite - j) / nTrans);
      }
    }
  }
}


void DiscoFire() {
  
}


void ColorExplosion() {
  
}

void Orbs() {
  const uint8_t centerPoint = 51;
  const uint8_t minRadius = 3;
  //const uint8_t maxRadius = 51;
  const uint8_t fadeRate = 64;
    
  uint8_t curRadius;
  CRGB colorToUse;
  
  int16_t radiusOffset = 0;//round((maxRadius-minRadius) * (sin8(baseCount % 255) - 128.0) / 128.0);
  if(radiusOffset < 0) {
    hsv2rgb_rainbow(pm.palette[2], colorToUse);
    curRadius = minRadius - radiusOffset;
  }
  else {
    hsv2rgb_rainbow(pm.palette[3], colorToUse);
    curRadius = minRadius + radiusOffset;
  }
  
  // Draw
  leds[centerPoint] = colorToUse;

  for(uint16_t i = 1; i <= curRadius; i++) {
    if(centerPoint >= i)
      leds[centerPoint - i] = colorToUse;
    if(centerPoint + i < NUM_LEDS)
      leds[centerPoint + i] = colorToUse;
  }

  for(CRGB & pixel : leds) pixel.fadeToBlackBy(fadeRate);
}

void ScrollingGlimmerBands() {
  const uint8_t glimmerPortion = 4;
  const uint8_t glimmerFloor = 180;
  CRGB colorPattern[baseParams.dimPeriod];
  uint8_t spacing = baseParams.dimPeriod - baseParams.numColors * baseParams.brightLength;

  uint16_t curPixel = 0;
  for(uint8_t i = 0; i < baseParams.numColors; i++) {
    for(uint8_t j = 0; j < baseParams.brightLength; j++) {
      hsv2rgb_rainbow(pm.palette[2 + i], colorPattern[curPixel]);
      if(random8(glimmerPortion) == 0) {
        colorPattern[curPixel] |= glimmerFloor;
      }
      curPixel++;
    }
    for(uint8_t j = 0; j < spacing; j++) {
      colorPattern[curPixel++] = CRGB::Black;
    }
  }
}

void CenterSpawn() {
  // Propogate out
  for(uint16_t i = 0; i < NUM_LEDS/2; i++) {
    leds[i] = leds[i+1];
    leds[NUM_LEDS-1 - i] = leds[NUM_LEDS-2 - i];
  }

  uint8_t firstCol = 0;//(baseCount / 128) % 6;
  uint8_t secondCol = (firstCol + 1) % 6;
  uint8_t blendAmount = 0;//2*(baseCount % 128);

  
  leds[NUM_LEDS/2 - 1] = HSV2RGB(nblend(pm.palette[firstCol], pm.palette[secondCol], blendAmount), leds_b[NUM_LEDS/2 - 1]);
  leds[NUM_LEDS/2] = leds[NUM_LEDS/2 - 1];
  leds_b[NUM_LEDS/2] = leds_b[NUM_LEDS/2 - 1];
}

