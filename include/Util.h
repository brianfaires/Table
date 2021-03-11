#pragma once
#include "FastLED.h"
#include "LEDLoop/GammaManager.h"

#define FPS_TO_TIME(x) uint32_t(ONE_SEC / x)

inline uint8_t scaleParam(uint8_t param, uint8_t minVal, uint8_t maxVal) {
  return minVal + (maxVal - minVal + 1) * param / 0x100;
}
inline int8_t scaleParamSigned(uint8_t param, int8_t minVal, int8_t maxVal) {
  return minVal + (maxVal - minVal + 1) * param / 0x100;
}
inline uint16_t scaleParam16(uint16_t param, uint16_t minVal, uint16_t maxVal) {
  return minVal + (maxVal - minVal + 1) * param / 0x10000;
}
inline int16_t scaleParamSigned16(uint16_t param, int16_t minVal, int16_t maxVal) {
  return minVal + (maxVal - minVal + 1) * param / 0x10000;
}

inline CRGB HSV2RGB(CHSV& hsv, uint8_t& led_b) {
  uint8_t tempV = hsv.v;
  hsv.v = 255;
  CRGB retVal;
  hsv2rgb_rainbow(hsv, retVal);
  led_b = tempV;
  return retVal;
}

inline void getCartesianFromHSV(CHSV color, int16_t& x, int16_t& y) {
  uint16_t theta = 256 * color.h;
  x = cos16(theta) * color.s / 255;
  y = sin16(theta) * color.s / 255;

  //PRINTLN("getCart" + color.ToString() + "= " + theta + ", " + x + ", " + y);
}
inline CHSV getHSVFromCartesian(int16_t x, int16_t y) {
  CHSV hsv;
  hsv.h = (uint8_t) (atan2(y/32767.0, x/32767.0) * 128 / PI); // theta
  hsv.s = (uint8_t) (sqrt(x * x + y * y) / 128); // length of hypotenuse
  hsv.v = 255;
  return hsv;
  //PRINTLN("getHSV(" + x + ", " + y + ") = " + hsv);
}

inline void fill_gradient_HSV_linear(CHSV* leds, uint16_t numPixels, CHSV a, CHSV b) {
  int16_t x1, y1, x2, y2;
  getCartesianFromHSV(a, x1, y1);
  getCartesianFromHSV(b, x2, y2);

  int16_t x, y;

  for(uint16_t i = 0; i < numPixels; i++) {
    uint8_t blendAmount = 255 * i / (numPixels-1);
    x = x1 + (x2-x1) * blendAmount/255;
    y = y1 + (y2-y1) * blendAmount/255;
    leds[i] = getHSVFromCartesian(x, y);
  }
}
inline void fill_gradient_HSV_linear(CRGB* leds, uint8_t* leds_b, uint16_t numPixels, CHSV a, CHSV b) {
  int16_t x1, y1, x2, y2;
  getCartesianFromHSV(a, x1, y1);
  getCartesianFromHSV(b, x2, y2);

  int16_t x, y;

  CHSV temp;
  for(uint16_t i = 0; i < numPixels; i++) {
    uint8_t blendAmount = 255 * i / (numPixels-1);
    x = x1 + (x2-x1) * blendAmount/255;
    y = y1 + (y2-y1) * blendAmount/255;
    temp = getHSVFromCartesian(x, y);
    leds[i] = HSV2RGB(temp, leds_b[i]);
  }  
}
inline void blendHSV(CHSV& a, CHSV b, uint8_t blendAmount) {
  int16_t x1, y1, x2, y2;
  getCartesianFromHSV(a, x1, y1);
  getCartesianFromHSV(b, x2, y2);

  int16_t x = x1 + (x2-x1) * blendAmount/255;
  int16_t y = y1 + (y2-y1) * blendAmount/255;
  uint8_t v = a.v;
  a = getHSVFromCartesian(x, y);
  a.v = v;
}

// Test function to compare gamma corrected RGB blending with linear HSV blending
/*
void CompareGradients(CHSV a, CHSV b, uint8_t numLEDs, CRGB* leds, uint8_t* leds_b) {
  fill_gradient_HSV_linear(&leds[0], leds_b, numLEDs, a, b);
  CRGB rgbA, rgbB;
  hsv2rgb_rainbow(a, rgbA);
  hsv2rgb_rainbow(b, rgbB);
  Gamma.Inverse(rgbA);
  Gamma.Inverse(rgbB);
  fill_gradient_RGB(&leds[384-numLEDs], numLEDs, rgbB, rgbA);
  for(int i = 384-numLEDs; i <= 384; i++) {
    Gamma.Correct(leds[i]); 
    leds_b[i] = 255;
  }
}
*/



// Timing debug tools
#ifdef TIMING_ANALYSIS
    #define NUM_TIMING_POINTS 20
    #define TIMING_STARTUP uint32_t startupTime = SYSTEM_TIME;
    #define TIMING_ANALYSIS_BEGIN_LOOP  curDebugTime = SYSTEM_TIME; curTiminingAnalysisPoint=0; for(uint8_t i=0;i<NUM_TIMING_POINTS;i++) { timingValues[i]=0; }
    #define TIMING_ANALYSIS_POINT       lastDebugTime = curDebugTime; curDebugTime = SYSTEM_TIME; timingValues[curTiminingAnalysisPoint++] = curDebugTime-lastDebugTime;
    #define TIMING_ANALYSIS_END_LOOP    for(uint8_t i=0; i<NUM_TIMING_POINTS; i++) { if(timingValues[i]!=0) { PRINT(timingValues[i] + "\t"); } else  { PRINTLN((SYSTEM_TIME - timing.now) + "\t"); break; } }
    #define DEBUG_TIMING(msg)           DEBUG(msg)
    uint32_t timingValues[NUM_TIMING_POINTS] = {0};
    uint32_t curDebugTime = 0;
    uint32_t lastDebugTime = 0;
    uint8_t curTiminingAnalysisPoint = 0;
#else
    #define TIMING_STARTUP
    #define TIMING_ANALYSIS_BEGIN_LOOP
    #define TIMING_ANALYSIS_POINT
    #define TIMING_ANALYSIS_END_LOOP
    #define DEBUG_TIMING(msg)
#endif

#ifdef CHECK_FOR_CLIPPING
    extern uint32_t lastClippedTime;
#endif