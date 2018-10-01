#include "GammaManager.h"
#include "PatternGenerator.h"
#include "PatternRepeater.h"

#define NUM_PATTERN_PARAMS 4

class PatternController {
  public:
    PatternController();
    void Init(struct_base_show_params& params, PaletteManager* pm, GammaManager* gm, uint32_t curTime);
    void Update(struct_base_show_params& params, CRGB* target, uint8_t* target_b, uint16_t numLEDs, uint32_t curTime);
    void SkipTime(uint32_t amount);

    uint32_t GetColorPauseLength();
    void SetColorPauseLength(uint32_t value);
    uint32_t GetColorBlendLength();
    void SetColorBlendLength(uint32_t value);
    uint32_t GetDimPauseLength();
    void SetDimPauseLength(uint32_t value);
    uint32_t GetDimBlendLength();
    void SetDimBlendLength(uint32_t value);

  private:
    PatternRepeater pr;
    PatternGenerator pg;

    void ScaleParams(struct_base_show_params& params, uint32_t curTime);
    void SetDisplayMode(uint8_t colorPattern, uint8_t brightnessPattern, uint32_t curTime);
    void WalkSpeeds();
    bool WalkColorParams();
    bool WalkDimParams();
    void BlendColorPattern(uint32_t curTime);
    void BlendDimPattern(uint32_t curTime);

    uint32_t dimPatternBlendLength, colorPatternBlendLength;
    uint32_t dimPatternPauseLength, colorPatternPauseLength;
    
    uint8_t numColors, colorThickness, brightLength, spacing, transLength;
    int8_t colorSpeed, dimSpeed;
    
    uint8_t oldDimPattern[NUM_LEDS], curDimPattern[NUM_LEDS], targetDimPattern[NUM_LEDS];
    PRGB oldColorPattern[NUM_LEDS], curColorPattern[NUM_LEDS], targetColorPattern[NUM_LEDS];

    uint32_t lastDimParamChange, lastColorParamChange;
    uint32_t lastDimPatternChange, lastColorPatternChange;
    
    uint8_t targetDimPatternIndex, targetColorPatternIndex;
    uint8_t oldDimPatternIndex, oldColorPatternIndex;
    uint16_t colorPeriod, brightnessPeriod;

    uint8_t randomDimPatternIndex, randomColorPatternIndex;

};

