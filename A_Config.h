#ifndef CONFIG_H
#define CONFIG_H

// Runtime config
#define DEBUG_SERIAL
//#define TEST_PALETTES
//#define USE_TEST_PALETTE
//#define TEST_COLOR_CORRECTION

#ifdef DEBUG_SERIAL
  #define MANUAL_PARAMS
  #define DEBUG_ERRORS
  #define EXPLICIT_PARAMETERS
  //#define DEBUG_BUTTONS
  //#define DEBUG_TIMING
#endif


// Software settings
#define PATTERN_PARAM_CHANGE_DISTANCE    0// NUM_LEDS
#define BRIGHTNESS_PARAM_CHANGE_DISTANCE 0// NUM_LEDS


// Refresh rates and timers
#define REFRESH_RATE 99 // This starts flickering dim colors at 100FPS, no idea why - may be APA102 manufacturer (See FastLED FAQ)
#define ONE_SEC_US 1000000L
#define ONE_MIN_US 60 * ONE_SEC_US
#define ONE_SEC_MS 1000
#define ONE_MIN_MS 60 * ONE_SEC_MS
#define US_BETWEEN_DRAWS (ONE_SEC_US / REFRESH_RATE)


// Initial animation values
#define INIT_BASE_ANIMATION 1
#define INIT_BASE_BRIGHTNESS_SPEED 0
#define INIT_BASE_COLOR_SPEED -5
#define INIT_BASE_SPACING 10
#define INIT_BASE_COLOR_THICKNESS_1 82
#define INIT_BASE_BRIGHT_LENGTH 3
#define INIT_BASE_TRANS_THICKNESS 8
#define INIT_BASE_NUM_COLORS 3
#define INIT_BASE_DISPLAY_MODE 0

#define INIT_TOP_ANIMATION 0
#define INIT_TOP_PORTION 128
#define INIT_TOP_SPEED 64

// Initial software values
#define INIT_GLOBAL_BRIGHTNESS 255

#define INIT_BASE_PAUSE_LENGTH  30 * ONE_MIN_US
#define INIT_BASE_TRANS_OUT_LENGTH  3 * ONE_SEC_US
#define INIT_BASE_TRANS_IN_LENGTH  3 * ONE_SEC_US
#define INIT_TOP_PAUSE_LENGTH   30 * ONE_MIN_US
#define INIT_TOP_TRANS_OUT_LENGTH   3 * ONE_SEC_US
#define INIT_TOP_TRANS_IN_LENGTH   3 * ONE_SEC_US

#define INIT_BASE_PARAM_WALK  30 * ONE_MIN_US
#define INIT_TOP_PARAM_WALK   30 * ONE_MIN_US

#define INIT_PM_WALK_LENGTH   3 * ONE_SEC_US
#define INIT_PM_PAUSE_LENGTH  30 * ONE_MIN_US

// Initial Pattern Controller values
#define INIT_PATTERN_CONTROLLER_DIM_PAUSE_LENGTH 1 * ONE_SEC_US
#define INIT_PATTERN_CONTROLLER_DIM_BLEND_LENGTH 1 * ONE_SEC_US
#define INIT_PATTERN_CONTROLLER_COLOR_PAUSE_LENGTH 2 * ONE_SEC_US
#define INIT_PATTERN_CONTROLLER_COLOR_BLEND_LENGTH 1 * ONE_SEC_US
#define INIT_PATTERN_REPEATER_BRIGHTNESS 16 // RGB Scaling
#define INIT_PATTERN_SCROLLER_BRIGHTNESS 255 // RGB Scaling

// Hardware config
#define STARTUP_DELAY_MS 1000
#define LED_DATA_RATE_MHZ 24
#define SERIAL_DATA_RATE_HZ 115200
#define CLOCK_PIN 13
#define LED_PIN 12
#define NUM_LEDS 420
#define BTN1_PIN 14
#define BTN2_PIN 15
#define NUM_BTNS 2



// Animation indexes
#define NONE 0

#define SCROLLER 1
#define FIRE 2
#define DISCO_FIRE 3
#define COLOR_EXPLOSION 5
#define ORBS 6
#define GLIMMER_BANDS 7
#define CENTER_SPAWN 8
#define NUM_BASE_ANIMATIONS 1

#define GLITTER 1
#define RAIN 2
#define TWINKLE 3
#define BOUNCE 4
#define COMETS 5
#define COLLISION 6
#define NUM_TOP_ANIMATIONS 2


// Gamma correction matrices
const uint8_t PROGMEM gammaR[] = { // 1.4
  0,1,1,1,1,1,1,2,2,2,3,3,4,4,4,5,
  5,6,6,7,7,8,8,9,9,10,10,11,12,12,13,13,
  14,15,15,16,16,17,18,18,19,20,20,21,22,22,23,24,
  25,25,26,27,28,28,29,30,31,31,32,33,34,34,35,36,
  37,38,38,39,40,41,42,43,43,44,45,46,47,48,49,49,
  50,51,52,53,54,55,56,57,57,58,59,60,61,62,63,64,
  65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
  81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,
  97,98,99,100,101,103,104,105,106,107,108,109,110,111,112,113,
  115,116,117,118,119,120,121,122,124,125,126,127,128,129,130,132,
  133,134,135,136,137,139,140,141,142,143,145,146,147,148,149,151,
  152,153,154,155,157,158,159,160,161,163,164,165,166,168,169,170,
  171,173,174,175,176,178,179,180,181,183,184,185,187,188,189,190,
  192,193,194,196,197,198,200,201,202,203,205,206,207,209,210,211,
  213,214,215,217,218,219,221,222,223,225,226,227,229,230,232,233,
  234,236,237,238,240,241,242,244,245,247,248,249,251,252,254,255 };

const uint8_t PROGMEM reverseGammaR[] = {
  0,3,8,10,13,15,17,19,21,23,25,27,28,30,32,33,
  35,37,38,40,41,43,44,46,47,48,50,51,52,54,55,56,
  58,59,60,62,63,64,65,67,68,69,70,71,73,74,75,76,
  77,78,80,81,82,83,84,85,86,87,89,90,91,92,93,94,
  95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,
  111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,
  127,128,129,130,131,132,132,133,134,135,136,137,138,139,140,141,
  142,143,143,144,145,146,147,148,149,150,151,151,152,153,154,155,
  156,157,158,158,159,160,161,162,163,164,164,165,166,167,168,169,
  169,170,171,172,173,174,174,175,176,177,178,179,179,180,181,182,
  183,184,184,185,186,187,188,188,189,190,191,192,192,193,194,195,
  196,196,197,198,199,200,200,201,202,203,203,204,205,206,207,207,
  208,209,210,210,211,212,213,213,214,215,216,217,217,218,219,220,
  220,221,222,223,223,224,225,226,226,227,228,229,229,230,231,232,
  232,233,234,235,235,236,237,237,238,239,240,240,241,242,243,243,
  244,245,246,246,247,248,248,249,250,251,251,252,253,253,254,255 };

const uint8_t PROGMEM gammaG[] = { // 1.35
  0,1,1,1,1,1,2,2,2,3,3,4,4,5,5,6,
  6,7,7,8,8,9,9,10,10,11,12,12,13,14,14,15,
  15,16,17,17,18,19,20,20,21,22,22,23,24,25,25,26,
  27,28,28,29,30,31,31,32,33,34,35,35,36,37,38,39,
  39,40,41,42,43,44,45,45,46,47,48,49,50,51,52,52,
  53,54,55,56,57,58,59,60,61,62,63,63,64,65,66,67,
  68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,
  84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,100,
  101,102,103,104,105,106,107,108,109,110,111,112,113,115,116,117,
  118,119,120,121,122,123,125,126,127,128,129,130,131,132,134,135,
  136,137,138,139,141,142,143,144,145,146,148,149,150,151,152,153,
  155,156,157,158,159,161,162,163,164,165,167,168,169,170,171,173,
  174,175,176,178,179,180,181,182,184,185,186,187,189,190,191,192,
  194,195,196,197,199,200,201,203,204,205,206,208,209,210,211,213,
  214,215,217,218,219,221,222,223,224,226,227,228,230,231,232,234,
  235,236,238,239,240,242,243,244,246,247,248,250,251,252,254,255 };

const uint8_t PROGMEM reverseGammaG[] = {
  0,3,7,9,11,13,15,17,19,21,23,25,26,28,29,31,
  33,34,36,37,38,40,41,43,44,45,47,48,49,51,52,53,
  55,56,57,58,60,61,62,63,65,66,67,68,69,70,72,73,
  74,75,76,77,78,80,81,82,83,84,85,86,87,88,89,90,
  92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,
  108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,
  124,125,126,126,127,128,129,130,131,132,133,134,135,136,137,138,
  139,140,140,141,142,143,144,145,146,147,148,149,149,150,151,152,
  153,154,155,156,157,157,158,159,160,161,162,163,163,164,165,166,
  167,168,169,169,170,171,172,173,174,175,175,176,177,178,179,180,
  180,181,182,183,184,185,185,186,187,188,189,190,190,191,192,193,
  194,194,195,196,197,198,199,199,200,201,202,203,203,204,205,206,
  207,207,208,209,210,211,211,212,213,214,214,215,216,217,218,218,
  219,220,221,222,222,223,224,225,225,226,227,228,228,229,230,231,
  232,232,233,234,235,235,236,237,238,238,239,240,241,241,242,243,
  244,244,245,246,247,247,248,249,250,250,251,252,253,253,254,255 };

const uint8_t PROGMEM gammaB[] = { // 1.5
  0,1,1,1,1,1,1,1,1,2,2,2,3,3,3,4,
  4,4,5,5,6,6,6,7,7,8,8,9,9,10,10,11,
  11,12,12,13,14,14,15,15,16,16,17,18,18,19,20,20,
  21,21,22,23,23,24,25,26,26,27,28,28,29,30,31,31,
  32,33,34,34,35,36,37,37,38,39,40,41,41,42,43,44,
  45,46,46,47,48,49,50,51,52,53,53,54,55,56,57,58,
  59,60,61,62,63,64,65,65,66,67,68,69,70,71,72,73,
  74,75,76,77,78,79,80,81,82,83,84,85,86,88,89,90,
  91,92,93,94,95,96,97,98,99,100,102,103,104,105,106,107,
  108,109,110,112,113,114,115,116,117,119,120,121,122,123,124,126,
  127,128,129,130,132,133,134,135,136,138,139,140,141,142,144,145,
  146,147,149,150,151,152,154,155,156,158,159,160,161,163,164,165,
  167,168,169,171,172,173,174,176,177,178,180,181,182,184,185,187,
  188,189,191,192,193,195,196,197,199,200,202,203,204,206,207,209,
  210,211,213,214,216,217,218,220,221,223,224,226,227,228,230,231,
  233,234,236,237,239,240,242,243,245,246,248,249,251,252,254,255 };

const uint8_t PROGMEM reverseGammaB[] = {
  0,4,10,13,16,18,21,23,25,27,29,31,33,35,36,38,
  40,42,43,45,46,48,50,51,53,54,55,57,58,60,61,62,
  64,65,66,68,69,70,72,73,74,75,77,78,79,80,81,83,
  84,85,86,87,88,89,91,92,93,94,95,96,97,98,99,100,
  101,102,104,105,106,107,108,109,110,111,112,113,114,115,116,117,
  118,119,120,121,122,123,124,124,125,126,127,128,129,130,131,132,
  133,134,135,136,137,137,138,139,140,141,142,143,144,145,146,146,
  147,148,149,150,151,152,152,153,154,155,156,157,158,158,159,160,
  161,162,163,163,164,165,166,167,168,168,169,170,171,172,173,173,
  174,175,176,177,177,178,179,180,181,181,182,183,184,184,185,186,
  187,188,188,189,190,191,191,192,193,194,194,195,196,197,198,198,
  199,200,201,201,202,203,204,204,205,206,206,207,208,209,209,210,
  211,212,212,213,214,215,215,216,217,217,218,219,220,220,221,222,
  222,223,224,225,225,226,227,227,228,229,230,230,231,232,232,233,
  234,234,235,236,237,237,238,239,239,240,241,241,242,243,243,244,
  245,245,246,247,247,248,249,249,250,251,251,252,253,253,254,255 };

const uint8_t PROGMEM gammaDim_5bit[] = {
   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 69
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 104
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // 131
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, // 151
   5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, // 166
   6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, // 177
   7, 7, 7, 7, 7, 7, 7, 7, // 185
   8, 8, 8, 8, 8, 8, // 191
   9, 9, 9, 9, 9, // 196
  10,10,10,10,10, // 201
  11,11,11,11, // 205
  12,12,12,12, // 209
  13,13,13,13, // 213
  14,14,14, // 216
  15,15,15, // 219
  16,16,16, // 222
  17,17,17, // 225
  18,18,18, // 228
  19,19,19, // 231
  20,20, // 233
  21,21, // 235
  22,22, // 237
  23,23, // 239
  24,24, // 241
  25,25, // 243
  26,26, // 245
  27,27, // 247
  28,28, // 249
  29,29, // 251
  30,30, // 253
  31,31 // 255
};

const uint8_t PROGMEM gammaDim[] = {
    0, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 28, 29, 30, 32, 33, 35, 37, 39, 40, 42, 44, 46, 49, 51, 53, 56, 58, 61, 64, 67, 70, 73,
   76, 79, 83, 86, 90, 93, 97,101,105,109,113,118,122,127,132,137,143,148,154,160,165,171,176,182,188,193,199,205,211,218,225,232,239,247,255, // 69
  128,130,132,136,139,142,145,148,151,154,157,160,163,166,169,173,177,181,185,189,193,197,201,205,209,213,217,221,225,230,235,240,245,250,255, // 104
  170,172,174,176,179,182,185,188,191,194,197,200,203,206,209,212,215,218,221,225,229,233,237,241,245,250,255, // 131
  192,194,196,199,202,205,208,211,214,217,220,223,226,229,233,237,241,245,250,255, // 151
  212,214,216,218,221,224,227,230,233,236,239,243,247,251,255, // 166
  224,226,228,231,234,237,240,243,247,251,255, // 177
  228,231,235,239,243,247,251,255, // 185
  232,236,240,245,250,255, // 191
  236,240,245,250,255, // 196
  239,243,247,251,255, // 201
  241,245,250,255, // 205
  243,247,251,255, // 209
  244,247,251,255, // 213
  245,250,255, // 216
  246,250,255, // 219
  246,250,255, // 222
  247,251,255, // 225
  247,251,255, // 228
  248,251,255, // 231
  248,255, // 233
  249,255, // 235
  249,255, // 237
  249,255, // 239
  250,255, // 241
  250,255, // 243
  250,255, // 245
  250,255, // 247
  251,255, // 249
  251,255, // 251
  251,255, // 253
  251,255 // 255
};


/*

const uint8_t PROGMEM gammaDim_5bit[] = {
   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 63
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 88
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // 108
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, // 125
   5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, // 140
   6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, // 153
   7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // 164
   8, 8, 8, 8, 8, 8, 8, 8, 8, // 173
   9, 9, 9, 9, 9, 9, 9, 9, 9, // 182
  10,10,10,10,10,10,10, 10, // 190
  11,11,11,11,11,11,11, // 197
  12,12,12,12,12,12, // 203
  13,13,13,13,13,13, // 209
  14,14,14,14,14, // 214
  15,15,15,15, // 218
  16,16,16,16, // 222
  17,17,17, // 225
  18,18,18, // 228
  19,19,19, // 231
  20,20, // 233
  21,21, // 235
  22,22, // 237
  23,23, // 239
  24,24, // 241
  25,25, // 243
  26,26, // 245
  27,27, // 247
  28,28, // 249
  29,29, // 251
  30,30, // 253
  31,31 // 255
};

const uint8_t PROGMEM gammaDim[] = {
    0, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 28, 30, 32, 34, 36, 38, 41, 43, 46, 48, 51, 54, 56, 59, 61, 64, 67, 70, 73,
   76, 80, 84, 88, 92, 96,101,106,111,116,121,127,133,139,145,150,155,160,165,170,175,181,187,193,199,206,213,221,229,237,246,255, // 63
  128,132,136,140,144,149,153,157,162,166,170,175,180,185,190,196,201,206,212,218,224,231,238,246,255, // 88
  170,173,177,181,185,189,192,196,200,204,208,212,216,221,226,231,236,242,248,255, // 108
  192,195,198,201,204,208,212,215,219,223,227,231,235,240,245,250,255, // 125
  212,214,216,218,221,224,227,230,233,236,239,243,247,251,255, // 140
  224,226,228,230,232,234,237,240,242,245,248,251,255, // 153
  228,230,232,234,236,239,242,245,248,251,255, // 164
  232,234,236,238,241,244,247,251,255, // 173
  236,238,240,242,244,246,249,252,255, // 182
  239,241,243,245,247,249,252,255, // 190
  241,243,245,247,249,252,255, // 197
  243,245,247,249,252,255, // 203
  244,246,248,250,252,255, // 209
  245,247,249,252,255, // 214
  246,248,251,255, // 218
  246,248,251,255, // 222
  247,251,255, // 225
  247,251,255, // 228
  248,251,255, // 231
  248,255, // 233
  249,255, // 235
  249,255, // 237
  249,255, // 239
  250,255, // 241
  250,255, // 243
  250,255, // 245
  250,255, // 247
  251,255, // 249
  251,255, // 251
  251,255, // 253
  251,255 // 255
};

 */

/*
const CRGB PROGMEM colorCorrections[] = {
  0xFF90FA, 0xF4A6FF, 0xD4A0FF, 0xC6A4FF,
  0xC4A8FF, 0xBDA8FF, 0xB4A4FF, 0xB2A4FF,
  0xA8B0FF, 0xA4B0FF, 0xB0B8FF, 0xB0C8FF,
  0xB0D0FF, 0xAAC4FF, 0xA4C6FF, 0xA0C0FF,
  0xA0B1FF, 0x90B0FF, 0x90ABFF, 0x90B0FF,
  0x98B8FF, 0x98B8FF, 0x98B8FF, 0xA8C4FF,
  0xA8C4FF, 0xA8C4FF, 0xA8C4FF, 0xA4C8FF,
  0xA4C8FF, 0xA4C8FF, 0xA4C8FF, 0xA4C8FF  
};*/


const CRGB PROGMEM colorCorrections[] = {
  0xFF8CF8, 0xF498FF, 0xD8A0FF, 0xC8A2FF,
  0xC4A4FF, 0xC0A6FF, 0xBCA8FF, 0xB8A9FF,
  0xB4AAFF, 0xB0ABFF, 0xADACFF, 0xAAADFF,
  0xAAAEFF, 0xA9AFFF, 0xA9B0FF, 0xA8B1FF,
  0xA8B2FF, 0xA8B3FF, 0xA8B4FF, 0xA7B5FF,
  0xA7B6FF, 0xA7B7FF, 0xA7B8FF, 0xA6B9FF,
  0xA6BBFF, 0xA6BCFF, 0xA5BEFF, 0xA5BFFF,
  0xA5C0FF, 0xA4C1FF, 0xA4C3FF, 0xA4C4FF  
};


/* Prior to paired-lookup tables
const uint8_t PROGMEM gammaDim[] = { // 3.0 with flattened low end and sharper top end
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,
  2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,
  4,4,4,5,5,5,5,5,6,6,6,6,7,7,7,8,
  8,8,9,9,10,10,11,11,12,12,12,13,13,14,14,15,
  15,16,16,17,17,18,18,19,19,20,20,21,21,22,22,23,
  23,24,25,25,26,26,27,27,28,28,29,29,30,30,31,32,
  32,33,34,34,35,36,37,37,38,39,39,40,41,41,42,43,
  43,44,45,45,46,47,47,48,49,50,50,51,52,53,53,54,
  55,56,56,57,58,59,60,61,61,62,63,64,65,66,67,67,
  68,69,70,71,72,73,73,74,75,76,77,77,78,79,80,81,
  82,82,83,84,85,86,87,88,89,90,92,93,94,95,96,97,
  98,99,101,102,103,104,105,107,108,109,111,112,113,115,116,118,
  120,121,123,125,127,129,131,133,136,138,141,144,146,149,152,155,
  157,160,163,166,169,172,175,178,181,184,187,191,194,197,200,203,
  206,209,213,217,220,223,226,229,232,235,239,242,245,248,251,255 };

const uint8_t PROGMEM gammaDim_5bit[32] = { // Manually created
  0,1,1,1,2,2,3,3,
  4,5,5,6,6,7,8,9,
  9,10,11,12,13,14,15,16,
  18,19,21,22,24,26,28,31
};
*/

#endif

