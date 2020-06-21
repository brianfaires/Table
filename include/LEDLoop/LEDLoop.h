#pragma once

#include "GammaManager.h"
#include "PaletteManager.h"
#include "Definitions.h"
#include "Util.h"
#include "PatternScrolling\PatternController.h"

#define IS_INTERIOR_LOOP (NUM_LEDS == 408)

class LEDLoop {
    public:
        template<uint8_t DATA_PIN, uint8_t CLOCK_PIN, uint16_t numLEDs> void Setup(uint8_t* pGlobalBrightness)
        {
            NUM_LEDS = numLEDs;
            FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, numLEDs, 0, leds_5bit_brightness);
            Setup(pGlobalBrightness);
        }
        private: void Setup(uint8_t* pGlobalBrightness);
        public: bool Loop();

    //private:
        void SkipTimeForTimers(uint32_t amount);
        void SkipTime(uint32_t amount);

        // Organizational structs for timers, configuration, and runtime parameters
        struct_timers timing;
        struct_config layerConfig;
        struct_base_show_params baseParams;
        struct_top_show_params topParams;

        // Color and brightness outputs
        uint16_t NUM_LEDS;
        CRGBArray<MAX_LEDS> leds;
        uint8_t leds_b[MAX_LEDS];
        uint8_t leds_5bit_brightness[MAX_LEDS];
        uint8_t* pGlobalBrightness;

        CRGBArray<MAX_LEDS> leds_top;
        uint8_t leds_top_b[MAX_LEDS];

        // Temp storing these here
        uint8_t NUM_ALLOWED_DIM_PERIODS;
        uint8_t NUM_ALLOWED_COLOR_PERIODS;
        std::vector<uint16_t> allowedDimPeriods;
        std::vector<uint16_t> allowedColorPeriods;


        // Objects
        PaletteManager pm;
        GammaManager Gamma;
        PatternController pc;

        // ParameterWalking.cpp
        void UpdateAnimationParameters();
        void PulseBaseParams();
        void RandomizeBaseParams();
        void WalkTopParams();
        void WalkPaletteManagerParams();
        void WalkLayerParams();

        // Layers.cpp
        void DrawBaseLayer();
        void InitBaseLayer();
        void CleanupBaseLayer(BaseAnimation lastAnimation);
        void TransitionBaseAnimation();
        void NextBaseAnimation();
        void DrawTopLayer();
        void InitTopLayer();
        void CleanupTopLayer(TopAnimation lastAnimation);
        void TransitionTopAnimation();
        void NextTopAnimation();
        void OverlayLayers();

        // BaseAnimations.cpp
        void Fire();
        void CreateFirePalette(CRGBPalette16 *firePalette);
        void ScrollingSaturation(uint8_t nWhite, uint8_t nTrans, uint8_t nPure);
        void DiscoFire();
        void Orbs();
        void ScrollingGlimmerBands();
        void CenterSpawn();
        void Stacks();
        uint8_t InitStackers(uint16_t rotationIndex);
        bool DrawStacker(struct_stacker* s);
        bool ClearStackers(uint8_t clearMode);
        void Collision();
        void PulseInPlace();
        void ColorExplosion();
        void MovingStrobe();
        void Shutters();
        void StutterStepBands();
        void ColorCycle();
        void HotGlow();

        // TopAnimations.cpp
        void Glitter();
        void Rain();
        void Twinkle();
        void FourComets();
        void DrawComet(struct_comet* comet, uint8_t cometLength, uint16_t moveIndex);

        // Timing debug tools
        #ifdef TIMING_ANALYSIS
            #define NUM_TIMING_POINTS 20
            #define TIMING_ANALYSIS_BEGIN_LOOP  curDebugTime = SYSTEM_TIME; curTiminingAnalysisPoint=0; for(uint8_t i=0;i<NUM_TIMING_POINTS;i++) { timingValues[i]=0; }
            #define TIMING_ANALYSIS_POINT       lastDebugTime = curDebugTime; curDebugTime = SYSTEM_TIME; timingValues[curTiminingAnalysisPoint++] = curDebugTime-lastDebugTime;
            #define TIMING_ANALYSIS_END_LOOP    for(uint8_t i=0; i<NUM_TIMING_POINTS; i++) { if(timingValues[i]!=0) { PRINT(timingValues[i] + "\t"); } else  { PRINTLN((SYSTEM_TIME - timing.now) + "\t"); break; } }
            #define DEBUG_TIMING(msg)           DEBUG(msg)
            uint32_t timingValues[NUM_TIMING_POINTS] = {0};
            uint32_t curDebugTime = 0;
            uint32_t lastDebugTime = 0;
            uint8_t curTiminingAnalysisPoint = 0;
        #else
            #define TIMING_ANALYSIS_BEGIN_LOOP
            #define TIMING_ANALYSIS_POINT
            #define TIMING_ANALYSIS_END_LOOP
            #define DEBUG_TIMING(msg)
        #endif

        #ifdef CHECK_FOR_CLIPPING
            uint32_t lastClippedTime = 0;
        #endif

};
