#pragma once
#include "GammaManager.h"
#include "PaletteManager.h"
#include "Definitions.h"
#include "Util.h"
#include "PatternScrolling\PatternController.h"

// Timing debug tools
#if DEBUG_TIMING
  uint32_t timingValues[10];
  uint32_t curDebugTime;
  uint32_t lastDebugTime;
#endif

// Organizational structs for timers, configuration, and runtime parameters
struct_timers timing;
struct_config layerConfig;
struct_base_show_params baseParams;
struct_top_show_params topParams;

// Color and brightness outputs
CRGBArray<NUM_LEDS> leds;
uint8_t leds_b[NUM_LEDS];
CRGBArray<NUM_LEDS> leds_top;
uint8_t leds_top_b[NUM_LEDS];
uint8_t globalBrightness = INIT_GLOBAL_BRIGHTNESS;

// Objects
PaletteManager pm;
GammaManager Gamma;
PatternController pc;


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
void CleanupBaseLayer(uint8_t lastAnimation);
void TransitionBaseAnimation(uint32_t curTime);
void NextBaseAnimation(uint32_t curTime);
void DrawTopLayer();
void InitTopLayer();
void CleanupTopLayer(uint8_t lastAnimation);
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
