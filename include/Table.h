#pragma once
#include "LEDLoop/LEDLoop.h"


// Globals
extern uint8_t globalBrightness;
extern LEDLoop interior;
extern LEDLoop upper;


// IO
void UpdateIO();
void SkipTimeForIO(uint32_t amount);
bool ProcessSerialInput(bool interiorStrip);
void PrintParams();
void PrintBaseTopAndPMParams(bool isInterior);
void PrintBaseParams(bool isInterior);
void PrintTopParams(bool isInterior);
void PrintPaletteManagerParams(bool isInterior);
void PrintLayerParams(bool isInterior);
void PrintPatternControllerParams(bool isInterior);

