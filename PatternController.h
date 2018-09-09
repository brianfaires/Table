#include "GammaManager.h"
#include "PatternGenerator.h"
#include "PatternRepeater.h"

class PatternController {
  public:
    PatternController();
    void Init(struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, uint32_t curTime);
    void Update(struct_base_show_params& params, CRGB* target, uint16_t numLEDs, uint32_t curTime);
    void SkipTime(uint32_t amount);

  private:
    PatternRepeater pr;
    PatternGenerator pg;

    void ScaleParams(struct_base_show_params& params);
    void SetDisplayMode(uint8_t colorPattern, uint8_t brightnessPattern);
    void WalkSpeeds();
    bool WalkColorParams();
    bool WalkBrightnessParams();
    
    uint8_t lastBrightnessPattern[NUM_LEDS], targetBrightnessPattern[NUM_LEDS];
    PRGB lastColorPattern[NUM_LEDS], targetColorPattern[NUM_LEDS];
    uint32_t lastBrightnessChange, lastColorChange;
    uint8_t numColors, colorThickness, brightLength, spacing, transLength;
    int8_t colorSpeed, dimSpeed;
    uint8_t brightnessPatternIndex, colorPatternIndex;
    uint16_t colorPeriod, brightnessPeriod;

};

