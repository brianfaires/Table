#ifndef PATTERN_SCROLLER_H
#define PATTERN_SCROLLER_H

#include "PatternRepeater.h"

class PatternScroller {
  public:
    PatternScroller();
    virtual void SetCRGBs(CRGB* target, uint16_t numLEDs, PaletteManager& pm);
    virtual void SetColorPattern(PRGB* newPattern, uint16_t newColorPeriod);
    virtual void SetDimPattern(uint8_t* newPattern, uint16_t newDimPeriod, uint32_t curTime, bool isParamChange);
    virtual void Init(uint32_t curTime);
    virtual void Update(uint32_t& newTime);
    virtual bool IsReadyForColorPatternChange(uint32_t curTime);
    virtual bool IsReadyForBrightnessChange(uint32_t curTime);
    virtual void SkipTime(uint32_t amount);
    virtual void BrightnessParametersChanged();
    virtual void ColorPatternParametersChanged();
    
    int8_t colorSpeed;
    int8_t dimSpeed;
    uint8_t myBrightness;

  private:
    virtual void ScrollColorPattern(bool scrollForward);
    virtual void ScrollDimPattern(bool scrollForward);
    
    PRGB colors[NUM_LEDS];
    PRGB colorPattern[NUM_LEDS];
    uint16_t colorPeriod;
    uint16_t colorIndexFirst;
    uint16_t colorIndexLast;
    uint32_t lastColorMove;
    int16_t colorParamWaitCounter;
    
    uint8_t brightnesses[NUM_LEDS];
    uint8_t brightnessPattern[NUM_LEDS];
    uint16_t brightnessPeriod;
    uint16_t brightnessIndexFirst;
    uint16_t brightnessIndexLast;
    uint32_t lastDimMove;
    int16_t brightnessParamWaitCounter;
    
};

#endif
