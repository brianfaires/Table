#pragma once

#include "GammaManager.h"
#include "PaletteManager.h"
#include "Definitions.h"
#include "Util.h"
#include "PatternScrolling\PatternController.h"

class LEDLoop {
    public:
        template<uint8_t DATA_PIN, uint8_t CLOCK_PIN, uint16_t NUM_LEDS> void Setup(uint8_t* pGlobalBrightness, PaletteManager* pPaletteManager)
        {
            numLEDs = NUM_LEDS;
            FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(LED_DATA_RATE_MHZ)>(leds, numLEDs, 0, leds_5bit_brightness);
            Setup(pGlobalBrightness, pPaletteManager);
        }
        private: void Setup(uint8_t* pGlobalBrightness, PaletteManager* pPaletteManager);
        public: bool Loop(uint32_t curTime);

    //private:
        void SkipTime(uint32_t amount);

        // Organizational structs for timers, configuration, and runtime parameters
        struct_timers timing;
        struct_config layerConfig;
        struct_base_show_params baseParams;
        struct_top_show_params topParams;

        // Color and brightness outputs
        uint16_t numLEDs;
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
        PaletteManager* pm;
        GammaManager Gamma;
        PatternController pc;


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
        void FireGlitter();
        void Fire();
        void CreateFirePalette(CRGBPalette16 *firePalette);
        void DiscoFire();
        
        
        void Stacks();
        uint8_t InitStacks(uint8_t mode = 0);
        void DrawStack(struct_stack& s);
        void DrawAllStacks();
        void MoveStack(struct_stack& s, bool clockwise);
        void MoveAllStacks(bool clockwise);
        bool Stack4();
        bool Stack4_Mirror();
        bool Shutters();
        bool StutterStepBands();
        
        
        void Stackers();
        uint8_t InitStackers(uint16_t rotationIndex);
        bool DrawStacker(struct_stacker* s);
        bool ClearStackers(uint8_t clearMode);
        
        
        void ScrollingSaturation(uint8_t nWhite, uint8_t nTrans, uint8_t nPure);
        void Orbs();
        void ScrollingGlimmerBands();
        void CenterSpawn();
        
        
        void Collision();
        void PulseInPlace();
        void ColorExplosion();
        void MovingStrobe();
        void ColorCycle();
        void HotGlow();

        // TopAnimations.cpp
        void Glitter();
        void Rain();
        void Twinkle();
        void FourComets();
        void DrawComet(struct_comet* comet, uint8_t cometLength, uint16_t moveIndex);

};
