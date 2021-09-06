/* Functions and objects specific to the Infinity table, with visibility on objects that may be changed via IO.
   This handles IO and the definition of objects that exist on the table. This and the config files should be the only code that is not project-agnostic.
*/
#pragma once
#include "obj/LEDLoop.h"

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

