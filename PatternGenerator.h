#define MIN_BRIGHTNESS 1
#define GET_FADE_STEP_SIZE(x) (255.0f - MIN_BRIGHTNESS) / (x+1)
#define NUM_DIM_PATTERNS 16
#define NUM_COLOR_PATTERNS 2

class PatternGenerator {
  public:
    uint8_t numColors, colorThickness, brightLength, transLength, spacing;
    
    PatternGenerator();
    uint8_t GetDimPeriod();
    uint16_t GetColorPeriod(uint8_t targetColorPatternIndex);
    void WriteDimPattern(uint8_t targetDimPatternIndex, uint8_t* outputArray);
    void WriteColorPattern(uint8_t targetColorPatternIndex, PRGB* outputArray);

  private:
    void WriteColorPattern_Gradient(PRGB* outputPattern);
    void WriteColorPattern_Blocks(PRGB* outputPattern);
    
    void WriteDimPattern_Comet(uint8_t* outputPattern);
    void WriteDimPattern_ReverseComet(uint8_t* outputPattern);
    void WriteDimPattern_TwoSided(uint8_t* outputPattern);
    void WriteDimPattern_Barbell(uint8_t* outputPattern);
    void WriteDimPattern_Snake(uint8_t* outputPattern);
    void WriteDimPattern_Snake2(uint8_t* outputPattern);
    void WriteDimPattern_Snake3(uint8_t* outputPattern);
    void WriteDimPattern_ThreeEvenComets(uint8_t* outputPattern);
    void WriteDimPattern_ThreeEvenReverseComets(uint8_t* outputPattern);
    void WriteDimPattern_Towers(uint8_t* outputPattern);
    void WriteDimPattern_Bowties(uint8_t* outputPattern);
    void WriteDimPattern_ReverseBowties(uint8_t* outputPattern);
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

