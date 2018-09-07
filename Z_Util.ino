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

String ToString(CHSV hsv) {
  return "(" + String(hsv.h) + ", " + String(hsv.s) + ", " + String(hsv.v) + ")";
}

String ToString(CRGB rgb) {
  return "(" + String(rgb.r) + ", " + String(rgb.g) + ", " + String(rgb.b) + ")";
}

void fillGradientHSV(CHSV a, CHSV b, uint16_t numPixels, CHSV* leds) {
  int16_t x1, y1, x2, y2;
  getCartesianFromHSV(a, x1, y1);
  getCartesianFromHSV(b, x2, y2);

  int16_t x, y;

  for(uint16_t i = 0; i < numPixels; i++) {
    uint8_t blendAmount = 255 * i / (numPixels-1);
    x = (x1 + x2) * blendAmount / 255;
    y = (y1 + y2) * blendAmount / 255;
    getHSVFromCartesian(leds[i], x, y);
  }
}

void blendHSV(CHSV& a, CHSV b, uint8_t blendAmount) {
  int16_t x1, y1, x2, y2;
  getCartesianFromHSV(a, x1, y1);
  getCartesianFromHSV(b, x2, y2);

  int16_t x = (x1 + x2) * blendAmount / 255;
  int16_t y = (y1 + y2) * blendAmount / 255;
  getHSVFromCartesian(a, x, y);
}

inline void getCartesianFromHSV(CHSV color, int16_t& x, int16_t& y) {
  uint16_t theta = 256 * color.h + 128;
  x = cos16(theta) * color.s / 255;
  y = sin16(theta) * color.s / 255;
}

inline void getHSVFromCartesian(CHSV& hsv, int16_t x, int16_t y) {
  hsv.s = sqrt(x * x + y * y) / 256; // length of hypotenuse
  hsv.h = 256 * atan2(y / 32767.0, x / 32767.0) * PI / 180; // theta
}

