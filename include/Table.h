#pragma once
#include "LEDLoop/LEDLoop.h"

// Globals
extern uint8_t globalBrightness;
extern LEDLoop interior;
extern LEDLoop upper;

void SkipTime(uint32_t amount);


// IO
void UpdateIO();
void SkipTimeForIO(uint32_t amount);
bool ProcessSerialInput();
void PrintParams();
void PrintBaseTopAndPMParams(LEDLoop* loop);
void PrintBaseParams(LEDLoop* loop);
void PrintTopParams(LEDLoop* loop);
void PrintPaletteManagerParams(LEDLoop* loop);
void PrintLayerParams(LEDLoop* loop);
void PrintPatternControllerParams(LEDLoop* loop);


// Parameter Walking
void UpdateAnimationParameters();
void PulseBaseParams();
void RandomizeBaseParams();
void WalkTopParams();
void WalkPaletteManagerParams();
void WalkLayerParams();

