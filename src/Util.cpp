#include "Globals.h"


void fill_gradient_HSV_linear(CHSV* leds, uint16_t numPixels, CHSV a, CHSV b) {
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
void fill_gradient_HSV_linear(CRGB* leds, uint8_t* leds_b, uint16_t numPixels, CHSV a, CHSV b) {
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
void blendHSV(CHSV& a, CHSV b, uint8_t blendAmount) {
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

CRGB HSV2RGB(CHSV& hsv, uint8_t& led_b) {
  uint8_t tempV = hsv.v;
  hsv.v = 255;
  CRGB retVal;
  hsv2rgb_rainbow(hsv, retVal);
  led_b = tempV;
  return retVal;
}
