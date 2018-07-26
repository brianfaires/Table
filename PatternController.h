#include "PatternGenerator.h"
#include "PatternRepeater.h"

#define NUM_BRIGHTNESS_PATTERNS 1
#define NUM_COLOR_PATTERNS 2

class PatternController {
  public:
    PatternController();
    void Init(uint32_t curTime);
    void Update(struct_base_show_params* params, uint32_t curTime);
    void SkipTime(uint32_t amount);

  private:
    PatternRepeater pr;
    PatternGenerator pg;

    void ScaleParams(struct_base_show_params* params);
    void WalkSpeeds();
    bool WalkColorParams();
    bool WalkBrightnessParams();
    
    uint8_t lastBrightnessPattern[NUM_LEDS], targetBrightnessPattern[NUM_LEDS];
    PRGB lastColorPattern[NUM_LEDS], targetColorPattern[NUM_LEDS];
    uint32_t lastBrightnessChange, lastColorChange;
    uint8_t numColors, colorThickness, brightLength, spacing, transLength;
    int8_t colorSpeed, brightnessSpeed;
    uint8_t curBrightnessPattern, curColorPattern;
    uint16_t colorPeriod, brightnessPeriod;

};

