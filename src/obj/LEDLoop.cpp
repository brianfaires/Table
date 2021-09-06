#include "obj/LEDLoop.h"

void LEDLoop::SkipTime(uint32_t amount) {
  timing.now += amount;
  timing.lastDraw += amount;
  timing.lastBaseParamWalk += amount;
  timing.lastTopParamWalk += amount;
  timing.lastBaseTransition += amount;
  timing.lastTopTransition += amount;
  pc.SkipTime(amount);
  stackers.SkipTime(amount);
  #if DEBUG_CLIPPING
    lastClippedTime += amount;
  #endif
}

void LEDLoop::Setup(uint8_t* pGlobalBrightness, PaletteManager* pPaletteManager)
{
  if(numLEDs == NUM_LEDS_UPPER)
  {
    // 2*2*3*5*6*7
    NUM_ALLOWED_DIM_PERIODS = 10;
    NUM_ALLOWED_COLOR_PERIODS = 12;
    allowedDimPeriods  =  {     21, 28, 30, 35, 42, 60, 70, 84, 105, 210 };
    allowedColorPeriods = { 20, 21, 28, 30, 35, 42, 60, 70, 84, 105, 210, 420 };
  }
  else if(numLEDs == NUM_LEDS_INTERIOR)
  {
    // 2*2*2*3*17
    NUM_ALLOWED_DIM_PERIODS = 7;
    NUM_ALLOWED_COLOR_PERIODS = 8;
    allowedDimPeriods   = { 17, 24, 34, 51, 68, 102, 204 };
    allowedColorPeriods = { 17, 24, 34, 51, 68, 102, 204, 408 };
  }
  else if(numLEDs == MAX_LEDS) {
    // 2*2*2*5*13
    
    NUM_ALLOWED_DIM_PERIODS = 7;
    NUM_ALLOWED_COLOR_PERIODS = 9;
    allowedDimPeriods   = { 20, 26, 40, 52, 65, 104, 130 };
    allowedColorPeriods = { 20, 26, 40, 52, 65, 104, 130, 260, 520 };
  }

  
  //--------------------Initialize LEDs--------------------
  leds = CRGB::Black;
  leds_top = CRGB::Black;

  //--------------------Initialize software--------------------
  pm = pPaletteManager;
  pm->Init(&(timing.now), INIT_PM_WALK_LENGTH, INIT_PM_PAUSE_LENGTH, INIT_PALETTE, INIT_FADE_IN_FROM_BLACK);
  DEBUG("PaletteManager init complete.");

  Gamma.Init(leds, leds_b, leds_5bit_brightness, numLEDs, pGlobalBrightness);
  DEBUG("Gamma init complete.");

  stackers.Init(numLEDs, pm, leds, leds_b, &baseParams, allowedDimPeriods, NUM_ALLOWED_DIM_PERIODS, &(timing.now));

  InitBaseLayer();
  DEBUG("Base layer init complete.");

  InitTopLayer();
  DEBUG("Top layer init complete.");
}

bool LEDLoop::Loop(uint32_t curTime)
{
  timing.now = curTime;

  #ifdef RUN_GAMMA_TESTS
    Gamma.RunTests(4, 32);
    return;
  #endif

  if(timing.now - timing.lastDraw < FPS_TO_TIME(REFRESH_RATE)) { return false; }
  
  #ifdef TEST_PALETTES
    const uint16_t length = 120;
    const uint16_t offset1 = 228;
    const uint16_t offset2 = 390;
    
    const uint16_t pixelsPerPalette = length / PALETTE_SIZE;
    leds = CRGB::Black;
    for(uint16_t i = 0; i < numLEDs; i++) { leds_b[i] = globalBrightness; }
    for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
      // With spaces
      leds(offset1 + pixelsPerPalette*i + 1, offset1 + pixelsPerPalette*(i+1) - 2) = pm->palette[i];
      leds(offset2 + pixelsPerPalette*i + 1, offset2 + pixelsPerPalette*(i+1) - 2) = pm->palette[PALETTE_SIZE-1-i];
      // Without spaces
      //leds(offset1 + pixelsPerPalette*i, offset1 + pixelsPerPalette*(i+1) - 1) = pm->palette[i];
      //leds(offset2 + pixelsPerPalette*i, offset2 + pixelsPerPalette*(i+1) - 1) = pm->palette[PALETTE_SIZE-1-i];
    }
  #else
    TransitionBaseAnimation(); TIMING_ANALYSIS_POINT
    TransitionTopAnimation(); TIMING_ANALYSIS_POINT
    DrawBaseLayer(); TIMING_ANALYSIS_POINT
    DrawTopLayer(); TIMING_ANALYSIS_POINT
    OverlayLayers(); TIMING_ANALYSIS_POINT
  #endif

  Gamma.PrepPixelsForFastLED(); TIMING_ANALYSIS_POINT

  timing.lastDraw += FPS_TO_TIME(REFRESH_RATE);
  #ifdef CHECK_FOR_CLIPPING
    if((timing.now > timing.lastDraw + FPS_TO_TIME(REFRESH_RATE)) && (timing.now-lastClippedTime > ONE_SEC)) {
      DEBUG("ERROR: Drawing clipped by " + (timing.now - timing.lastDraw) + "us")
      lastClippedTime = timing.now;
    }
  #endif

  return true;
}
