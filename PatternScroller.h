#ifndef PATTERN_SCROLLER_H
#define PATTERN_SCROLLER_H

class PatternScroller : public PatternHandler {
  public:
    PatternScroller();
    virtual void SetCRGBs(CRGB* target, uint16_t numLEDs, PaletteManager& pm);
    virtual void SetColorPattern(PRGB* newPattern, uint16_t newColorPatternLength);
    virtual void SetBrightnessPattern(uint8_t* newPattern, uint16_t newBrightnessPatternLength, uint32_t curTime, bool isParamChange);
    virtual void Init(uint32_t curTime);
    virtual void Update(uint32_t& newTime);
    virtual bool IsReadyForColorPatternChange(uint32_t currentTime);
    virtual bool IsReadyForBrightnessChange(uint32_t currentTime);
    virtual void SkipTime(uint32_t amount);
    virtual void BrightnessParametersChanged();
    virtual void ColorPatternParametersChanged();
    
    uint8_t myBrightness;

  private:
    virtual void ScrollColorPattern(bool scrollForward);
    virtual void ScrollBrightnessPattern(bool scrollForward);
    virtual bool BrightnessPatternIsInitialized();
    
    PRGB colors[NUM_LEDS];
    PRGB colorPattern[NUM_LEDS];
    uint16_t colorPatternLength;
    uint16_t colorIndexFirst;
    uint16_t colorIndexLast;
    uint32_t lastColorMove;
    int16_t colorParamWaitCounter;
    
    uint8_t brightnesses[NUM_LEDS];
    uint8_t brightnessPattern[NUM_LEDS];
    uint16_t brightnessPatternLength;
    uint16_t brightnessIndexFirst;
    uint16_t brightnessIndexLast;
    uint32_t lastBrightnessMove;
    int16_t brightnessParamWaitCounter;
    
};

#endif
