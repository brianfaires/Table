#pragma once

#include "GammaManager.h"
#include "PaletteManager.h"
#include "Definitions.h"
#include "Util.h"
#include "PatternScrolling\PatternController.h"

#ifdef TIMING_ANALYSIS
    #define NUM_TIMING_POINTS 20
    extern uint32_t timingValues[NUM_TIMING_POINTS];
    extern uint32_t curDebugTime;
    extern uint32_t lastDebugTime;
    extern uint8_t curTiminingAnalysisPoint;
    #define TIMING_ANALYSIS_BEGIN_LOOP  curDebugTime = SYSTEM_TIME; curTiminingAnalysisPoint=0; for(uint8_t i=0;i<NUM_TIMING_POINTS;i++) { timingValues[i]=0; }
    #define TIMING_ANALYSIS_POINT       lastDebugTime = curDebugTime; curDebugTime = SYSTEM_TIME; timingValues[curTiminingAnalysisPoint++] = curDebugTime-lastDebugTime;
    #define TIMING_ANALYSIS_END_LOOP    for(uint8_t i=0; i<NUM_TIMING_POINTS; i++) { if(timingValues[i]!=0) { PRINT(timingValues[i] + "\t"); } else  { PRINTLN((SYSTEM_TIME - timing.now) + "\t"); break; } }
    #define DEBUG_TIMING(msg)           DEBUG(msg)
  #else
    #define TIMING_ANALYSIS_BEGIN_LOOP
    #define TIMING_ANALYSIS_POINT
    #define TIMING_ANALYSIS_END_LOOP
    #define DEBUG_TIMING(msg)
#endif

#ifdef CHECK_FOR_CLIPPING
  extern uint32_t lastClippedTime;
#endif

// Organizational structs for timers, configuration, and runtime parameters
extern struct_timers timing;
extern struct_config layerConfig;
extern struct_base_show_params baseParams;
extern struct_top_show_params topParams;

// Color and brightness outputs
extern CRGBArray<NUM_LEDS> leds;
extern uint8_t leds_b[NUM_LEDS];
extern uint8_t leds_5bit_brightness[NUM_LEDS];

extern CRGBArray<NUM_LEDS> leds_top;
extern uint8_t leds_top_b[NUM_LEDS];
extern uint8_t globalBrightness;

// Objects
extern PaletteManager pm;
extern GammaManager Gamma;
extern PatternController pc;


/////// All global functions ///////
// Table.cpp
void SkipTime(uint32_t amount);
void SkipTimeForTimers(uint32_t amount);

// IO.cpp
void UpdateIO();
void SkipTimeForIO(uint32_t amount);
bool ProcessSerialInput();
void PrintParams();
void PrintBaseTopAndPMParams();
void PrintBaseParams();
void PrintTopParams();
void PrintPaletteManagerParams();
void PrintLayerParams();
void PrintPatternControllerParams();

// ParameterWalking.cpp
void UpdateAnimationParameters(uint32_t curTime);
void PulseBaseParams(uint32_t curTime);
void RandomizeBaseParams();
void WalkTopParams();
void WalkPaletteManagerParams();
void WalkLayerParams();

// Layers.cpp
void DrawBaseLayer();
void InitBaseLayer();
void CleanupBaseLayer(BaseAnimation lastAnimation);
void TransitionBaseAnimation(uint32_t curTime);
void NextBaseAnimation(uint32_t curTime);
void DrawTopLayer();
void InitTopLayer();
void CleanupTopLayer(TopAnimation lastAnimation);
void TransitionTopAnimation(uint32_t curTime);
void NextTopAnimation(uint32_t curTime);
void OverlayLayers();

// BaseAnimations.cpp
void Fire();
void CreateFirePalette(CRGBPalette16 *firePalette);
void ScrollingSaturation(uint8_t nWhite, uint8_t nTrans, uint8_t nPure);
void DiscoFire();
void Orbs();
void ScrollingGlimmerBands();
void CenterSpawn();
void Stacks(uint32_t curTime);
uint8_t InitStackers(uint16_t rotationIndex, uint32_t curTime);
bool DrawStacker(struct_stacker* s, uint32_t curTime);
bool ClearStackers(uint8_t clearMode, uint32_t curTime);
void Collision();
void PulseInPlace();
void ColorExplosion(uint32_t curTime);
void MovingStrobe();
void Shutters();
void StutterStepBands();
void ColorCycle();
void HotGlow();

// TopAnimations.cpp
void Glitter();
void Rain();
void Twinkle();
void FourComets(uint32_t curTime);
void DrawComet(struct_comet* comet, uint8_t cometLength, uint16_t moveIndex);
