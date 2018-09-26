#define MIN_BRIGHTNESS 32
#define GET_FADE_STEP_SIZE(x) (255.0f - MIN_BRIGHTNESS) / (x+1)
#define NUM_BRIGHTNESS_PATTERNS 16
#define NUM_COLOR_PATTERNS 2

class PatternGenerator {
  public:
    uint8_t numColors, colorThickness, brightLength, transLength, spacing;
    
    PatternGenerator();
    uint8_t GetBrightnessPeriod();
    uint16_t GetColorPeriod(uint8_t colorPatternIndex);
    void WriteBrightnessPattern(uint8_t brightnessPatternIndex, uint8_t* outputArray);
    void WriteColorPattern(uint8_t colorPatternIndex, PRGB* outputArray);

  private:
    void WriteColorPattern_Gradient(PRGB* outputPattern);
    void WriteColorPattern_Blocks(PRGB* outputPattern);
    
    void WriteDimPattern_Comet(uint8_t* outputPattern);
    void WriteDimPattern_BackwardComet(uint8_t* outputPattern);
    void WriteDimPattern_TwoSided(uint8_t* outputPattern);
    void WriteDimPattern_Barbell(uint8_t* outputPattern);
    void WriteDimPattern_StepBarbell(uint8_t* outputPattern);
    void WriteDimPattern_BrokenBarbell(uint8_t* outputPattern);
    void WriteDimPattern_BrokenStepBarbell(uint8_t* outputPattern);
    void WriteDimPattern_InverseStepBarbell(uint8_t* outputPattern);
    void WriteDimPattern_Towers(uint8_t* outputPattern);
    void WriteDimPattern_StepsUp(uint8_t* outputPattern);
    void WriteDimPattern_StepsDown(uint8_t* outputPattern);
    void WriteDimPattern_SlopedHighTowers(uint8_t* outputPattern);
    void WriteDimPattern_SlopedLowTowers(uint8_t* outputPattern);
    void WriteDimPattern_SlideHigh(uint8_t* outputPattern);
    void WriteDimPattern_SlideLow(uint8_t* outputPattern);
    void WriteDimPattern_NoDim(uint8_t* outputPattern);
};

PatternGenerator::PatternGenerator() {
  numColors = 0;
  colorThickness = 0;
  brightLength = 0;
  transLength = 0;
  spacing = 0;
}

