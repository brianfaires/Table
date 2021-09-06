/*
    Represents a solid loop of LEDs. Includes:
        1. Storage of LED info
        2. Links to globals (PaletteManager, global brightness)
        3. Local GammaManager, param structs, and everything needed for drawing animations
*/
#pragma once

#include "lib/GammaManager.h"
#include "lib/PaletteManager.h"
#include "Definitions.h"
#include "Util.h"
#include "obj/PatternController/PatternController.h"
#include "obj/Stackers.h"

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
        Stackers stackers;


        // Layers.cpp
        void DrawBaseLayer();
        void InitBaseLayer();
        void CleanupBaseLayer(BaseAnimation lastAnimation);
        void TransitionBaseAnimation();
        void NextBaseAnimation();
        BaseAnimation PeekNextBaseAnimation();
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
