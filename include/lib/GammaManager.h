/* For PROD use, uses the scaling included in the config to Gamma Correct both color balance and brightness.
   Uses APA102 5-bit brightness control for more colors at low brightnesses.
   Includes a dev mode (RUN_GAMMA_TESTS) to facilitate the creation of config settings on new hardware.
   Blending functions can be used to blend previously-corrected colors
   Intended use is for all logic/blending to be done in linear space, and always go through gamma correction. This has some impact on CHSV choice and usage.
*/
#pragma once
#include "Arduino.h"
#include "FastLED.h"
#include "config/Config.h"

class GammaManager {
  public:
    void Init(CRGB* _leds, uint8_t* _leds_b, uint8_t* _leds_5bit_brightness, uint16_t _numLEDs, uint8_t *_globalBrightness);
    void Correct(CRGB& pixel);
    void Inverse(CRGB& pixel);
	  CRGB Blend(CRGB& a, CRGB& b, fract8 blendAmount);
	  void BlendInPlace(CRGB& a, CRGB& b, fract8 blendAmount);
    void PrepPixelsForFastLED();

  private:
    CRGB* leds;
    uint8_t* leds_b;
    uint8_t* leds_5bit_brightness;
    uint16_t numLEDs;
    uint8_t* globalBrightness;


// Testing functions; disable when unused for smaller library size
#ifdef RUN_GAMMA_TESTS
  private:
	  uint8_t INITIAL_TEST_BRIGHTNESS = 64;
    bool useLookupMatrices = true;
    float fGammaR = 1.40;//1.30;//1.9;//1.60;//1.75;//1.8;//1.65;//1.15;
    float fGammaG = 1.35;//1.75;//1.9;//1.75;//1.90;//1.6;//2.1;//1.65;
    float fGammaB = 1.5;//2.00;//1.8;//1.80;//2.00;//3.1;//3.1;//2.85;
  
    void RunGradientTest(CRGB* leds, uint8_t* leds_b, uint16_t numLEDs, uint16_t gradientLength = 32);
    void RunSimpleTest(CRGB* leds, uint8_t* leds_b, uint16_t numLEDs, uint8_t thickness = 4);
    void RunWhiteTest(CRGB* leds, uint8_t* leds_b, uint16_t numLEDs, uint8_t spacing);
    void RunMidpointTest(CRGB* leds, uint8_t* leds_b, uint16_t numLEDs, uint8_t thickness = 4, bool onePatternOnly = false);
	  void RunDimmingTest(CRGB* leds, uint8_t* leds_b, uint16_t numLEDs, uint16_t gradientLength);
    void WriteGammaMatrices(float gamma, int max_in = 255, int max_out = 255, String matrixNameSuffix = "", bool includeReverse = true);
    bool ProcessSerialInput();
    public: void RunTests(uint16_t thickness = 4, uint16_t gradientLength = 32);
#endif
};
