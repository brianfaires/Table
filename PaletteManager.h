#ifndef PALETTE_MANAGER_H
#define PALETTE_MANAGER_H

#define NUM_PM_PARAMS 2
#define PALETTE_SIZE 6
#define NUM_PALETTES 10

static CHSV test_palette[PALETTE_SIZE]={CHSV(  0,  0,255), CHSV(  0,  0,255), CHSV(  0,  0,255), CHSV(  0,  0,255), CHSV(  0,  0,255), CHSV(  0,  0,255) };

static CHSV ocean[PALETTE_SIZE] =     { CHSV(160,255,255), CHSV(135,200,255), CHSV(145,180,255), CHSV(155,155,255), CHSV(140,255,255), CHSV(145,110,255) };
static CHSV fire[PALETTE_SIZE] =      { CHSV(  8,255,255), CHSV( 16,245,255), CHSV( 25,255,255), CHSV( 48,255,255), CHSV( 34,150,255), CHSV(160,120,255) };
static CHSV icyBright[PALETTE_SIZE] = { CHSV(120,135,255), CHSV(210,160,255), CHSV( 64,190,255), CHSV(135,180,255), CHSV(210,150,255), CHSV(  0,  0,255) };
static CHSV watermelon[PALETTE_SIZE] ={ CHSV(220,255,255), CHSV( 79,255,255), CHSV(240,255,255), CHSV( 90,180,255), CHSV(192,255,255), CHSV(190,100,255) };
static CHSV fruitLoop[PALETTE_SIZE] = { CHSV(224,255,255), CHSV(175,255,255), CHSV( 80,235,255), CHSV(140,255,255), CHSV(190,255,255), CHSV( 64,170,255) };
static CHSV love[PALETTE_SIZE] =      { CHSV(250,255,255), CHSV(224,255,255), CHSV(197,255,255), CHSV(224,200,255), CHSV(250,175,255), CHSV(160, 65,255) };
static CHSV rave[PALETTE_SIZE] =      { CHSV(160,255,255), CHSV(208,255,255), CHSV(  0,255,255), CHSV( 53,255,255), CHSV( 96,255,255), CHSV(128,100,255) };
static CHSV pastel[PALETTE_SIZE] =    { CHSV(132,200,255), CHSV( 32,210,255), CHSV(200,165,255), CHSV(  5,180,255), CHSV(220,190,255), CHSV( 64,150,255) };
static CHSV strobe[PALETTE_SIZE] =    { CHSV(160,100,255), CHSV(  5,100,255), CHSV(  0,255,255), CHSV( 32,185,255), CHSV(160,145,255), CHSV(  0,  0,255) };
static CHSV lava[PALETTE_SIZE] =      { CHSV(  3,255,255), CHSV( 28,255,255), CHSV( 40,255,255), CHSV( 18,255,255), CHSV(255,255,255), CHSV(128,150,255) };
static CHSV forest[PALETTE_SIZE] =    { CHSV(  0,255,255), CHSV( 32,255,255), CHSV( 64,255,255), CHSV( 48,200,255), CHSV( 70,160,255), CHSV( 50,100,255) };
static CHSV rainbow[PALETTE_SIZE] =   { CHSV(  0,255,255), CHSV( 32,255,255), CHSV( 64,255,255), CHSV( 48,200,255), CHSV( 70,160,255), CHSV( 50,100,255) };

/*
static CRGB ocean[PALETTE_SIZE] =     { CRGB(  0,  0,255), CRGB(  0,130,210), CRGB( 25,200,160), CRGB( 30, 45,230), CRGB( 55,  7,255), CRGB(200,200, 60) };
static CRGB fire[PALETTE_SIZE] =      { CRGB(255,  6,  0), CRGB(255, 52,  0), CRGB(240,130,  0), CRGB(220,197,  0), CRGB( 32, 26,255), CRGB(255,170, 85) };
static CRGB icyBright[PALETTE_SIZE] = { CRGB( 35,255,180), CRGB(255,138,185), CRGB(255,255, 70), CRGB( 55,135,255), CRGB(190, 95,255), CRGB(255,255,255) };
static CRGB watermelon[PALETTE_SIZE] ={ CRGB(255,  0,165), CRGB(160, 26,255), CRGB(170,255,  8), CRGB(255, 20, 70), CRGB(100,255,  0), CRGB(200,150,255) };
static CRGB fruitLoop[PALETTE_SIZE] = { CRGB(255,105,  0), CRGB( 40,  0,255), CRGB( 80,255, 14), CRGB(  0, 80,255), CRGB(255,  0,120), CRGB(255,255, 60) };
static CRGB love[PALETTE_SIZE] =      { CRGB(255,  3,  7), CRGB(255,  0,200), CRGB(135, 15,255), CRGB(130,  0,  0), CRGB( 50, 24,255), CRGB(100,100,255) };
static CRGB rave[PALETTE_SIZE] =      { CRGB(  0,  0,255), CRGB(255,  0,  0), CRGB(255,255,  0), CRGB(255,  0,255), CRGB(  0,255,  0), CRGB(100,255,255) };
static CRGB pastel[PALETTE_SIZE] =    { CRGB( 30,205,255), CRGB(255,125, 20), CRGB(120,255,130), CRGB(210, 38, 32), CRGB(110, 40,255), CRGB(255,255, 80) };
static CRGB strobe[PALETTE_SIZE] =    { CRGB(185,172,255), CRGB( 50, 40, 30), CRGB(255,  0,  0), CRGB(200,200, 35), CRGB(  0,  0,255), CRGB(255,255,255) };
*/
//static CRGB ocean[PALETTE_SIZE] = { CRGB(0,0,255), CRGB(10,200,255), CRGB(90,50,255), CRGB(60,140,255), CRGB(90,150,255), CRGB(200,200,255) };
//static CRGB lava[PALETTE_SIZE] = { CRGB(255,0,0), CRGB(255,100,0), CRGB(255,255,0), CRGB(255,150,50), CRGB(220,255,160), CRGB(255,200,100) };

static CHSV allPalettes[NUM_PALETTES][PALETTE_SIZE] = { ocean, fire, icyBright, watermelon, fruitLoop, love, rave, pastel, strobe, lava/*, forest, rainbow*/ };

class PaletteManager {
  public:
    CHSV palette[PALETTE_SIZE];

    PaletteManager(); 
    void Init(uint32_t intialWalkLength, uint32_t intialPauseLength, uint32_t curTime);
    void Update(uint32_t curTime);
    bool PalettesAreEqual();
    void NextPalette(uint32_t curTime);
    void SkipTime(uint32_t amount);
    
    uint8_t GetTarget();
    void SetTarget(uint8_t newTarget, uint32_t curTime);
    
    uint32_t GetWalkLength();
    void SetWalkLength(uint32_t newWalkLength, uint32_t curTime);
    
    uint32_t GetPauseLength();
    void SetPauseLength(uint32_t newPauseLength, uint32_t curTime);

  private:
    uint8_t target;
    uint32_t walkLength;
    uint32_t pauseLength;
    CHSV oldPalette[PALETTE_SIZE];
    CHSV targetPalette[PALETTE_SIZE];
    uint32_t lastSwitchTime;
};
#endif

