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


