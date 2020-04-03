#pragma once
#include "Globals.h"

#define FPS_TO_TIME(x) (ONE_SEC / x)

inline uint8_t scaleParam(uint8_t param, uint8_t minVal, uint8_t maxVal) {
  return minVal + param * (maxVal - minVal + 1) / 256;
}
inline int8_t scaleParam(int8_t param, int8_t minVal, int8_t maxVal) {
  return minVal + (param * (maxVal - minVal + 1) + 128*(maxVal - minVal + 1)) / 256;
}
inline uint16_t scaleParam16(uint8_t param, uint16_t minVal, uint16_t maxVal) {
  return minVal + param * (maxVal - minVal + 1) / 256;
}
inline int16_t scaleParam16(int8_t param, int16_t minVal, int16_t maxVal) {
  return minVal + (param * (maxVal - minVal + 1) + 128*(maxVal - minVal + 1)) / 256;
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

void fill_gradient_HSV_linear(CHSV* leds, uint16_t numPixels, CHSV a, CHSV b);
void fill_gradient_HSV_linear(CRGB* leds, uint8_t* leds_b, uint16_t numPixels, CHSV a, CHSV b);
void blendHSV(CHSV& a, CHSV b, uint8_t blendAmount);
void CompareGradients(CHSV a, CHSV b, uint8_t numLEDs, CRGB* leds, uint8_t* leds_b);
CRGB HSV2RGB(CHSV& hsv, uint8_t& led_b);
