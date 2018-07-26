#ifndef PATTERN_REPEATER_H
#define PATTERN_REPEATER_H

class PatternRepeater {
  public:
    PatternRepeater();
    virtual void SetCRGBs(CRGB* target, uint16_t numLEDs, PaletteManager& pm);
    virtual void SetColorPattern(PRGB* newPattern, uint16_t newColorPatternLength);
    virtual void SetBrightnessPattern(uint8_t* newPattern, uint16_t newBrightnessPatternLength);
    virtual void Init(uint32_t curTime);
    virtual void Update(uint32_t& newTime);
    virtual bool IsReadyForColorMove(uint32_t currentTime);
    virtual bool IsReadyForBrightnessMove(uint32_t currentTime);
    virtual void SkipTime(uint32_t amount);

    int8_t colorSpeed;
    int8_t brightnessSpeed;
    uint8_t myBrightness;
    
  private:
    virtual void ScrollColorPattern(bool scrollForward);
    virtual void ScrollBrightnessPattern(bool scrollForward);
    
    PRGB colorPattern[NUM_LEDS];
    uint16_t colorPatternLength;
    uint16_t colorIndexFirst;
    uint32_t lastColorMove;
    
    uint8_t brightnessPattern[NUM_LEDS];
    uint16_t brightnessPatternLength;
    uint16_t brightnessIndexFirst;
    uint32_t lastBrightnessMove;
};

#endif
