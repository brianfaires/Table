#ifndef PATTERN_REPEATER_H
#define PATTERN_REPEATER_H

#define PATTERN_PARAM_CHANGE_DISTANCE     NUM_LEDS
#define BRIGHTNESS_PARAM_CHANGE_DISTANCE  NUM_LEDS

class PatternRepeater : public PatternHandler {
  public:
    PatternRepeater();
    virtual void SetCRGBs(CRGB* target, uint8_t numLEDs, PaletteManager& pm);
    virtual void SetColorPattern(PRGB* newPattern, uint8_t newColorPatternLength);
    virtual void SetBrightnessPattern(uint8_t* newPattern);
    virtual void Init(uint32_t curTime);
    virtual void Update(uint32_t& newTime);
    virtual bool IsReadyForPatternChange();
    virtual bool IsReadyForBrightnessChange();
    virtual void SkipTime(uint32_t amount);
    virtual void BrightnessParametersChanged();
    virtual void PatternParametersChanged();
    virtual uint8_t GetPeriod();
    
  private:
    virtual void ScrollPattern(bool scrollForward);
    virtual void ScrollBrightness(bool scrollForward);
    virtual bool BrightnessPatternIsInitialized();
    
    PRGB colorPattern[NUM_LEDS];
    uint8_t colorPatternLength;
    uint8_t colorIndexFirst;
    uint8_t colorIndexLast;
    uint32_t lastColorMove;
    int16_t colorParamWaitCounter;
    
    uint8_t brightnessPattern[NUM_LEDS];
    uint8_t brightnessIndexFirst;
    uint8_t brightnessIndexLast;
    uint32_t lastBrightnessMove;
    int16_t brightnessParamWaitCounter;
    
};

#endif
