#include "LEDLoop/LEDLoop.h"
#include "Table.h"

void LEDLoop::UpdateAnimationParameters() {
  #ifdef PULSE_BASE_PARAMS
    PulseBaseParams();
  #else
    if(timing.now - timing.lastBaseParamWalk >= layerConfig.baseParamWalkTime) {
      RandomizeBaseParams();
      timing.lastBaseParamWalk = timing.now;
    }
  #endif
  
  if(timing.now - timing.lastTopParamWalk >= layerConfig.topParamWalkTime) {
    WalkTopParams();
    timing.lastTopParamWalk = timing.now;
  }

  #ifdef MANUAL_PARAMS
    if(ProcessSerialInput(IS_INTERIOR_LOOP)) { 
      PrintParams();
      SkipTime(SYSTEM_TIME - timing.now);
    }
  #endif
}

void LEDLoop::PulseBaseParams() {
  const uint32_t pulseLength = 0.75*ONE_SEC;//MIN / 128;
  
  static uint32_t curCycle = 0;
  static bool brightLow = false;
  static bool transLow = false;
  //static bool brightMove = false;
  //static bool transMove = true;
  
  if(timing.now / pulseLength != curCycle) {
    curCycle = timing.now / pulseLength;

    /////// Changes to make each period ///////
    //if(brightMove) { brightLow = !brightLow; }
    //if(transMove) { transLow = !transLow; }
    transLow = !transLow;
    brightLow = !brightLow;
    
    //brightMove = false;
    //transMove = !brightMove;
    //if(!brightMove) { transMove = true; }
    //else if(brightLow != transLow) { transMove = random8(2); }
  }


  float curPulseLength = float(timing.now % pulseLength) / pulseLength;
  //baseParams.transLength = transLow ? 255*curPulseLength : 255*(1-curPulseLength);
  baseParams.brightLength = brightLow ? 255*curPulseLength : 255*(1-curPulseLength);
  //baseParams.brightLength = 255 - baseParams.transLength;
  
/*
  //baseParams.transLength = transLow ? 0 : 255;
  //baseParams.brightLength = transLow ? 255 : 0;

  uint8_t curPulseLength = (timing.now % pulseLength) * uint64_t(255) / pulseLength;
  baseParams.brightLength = transLow ? curPulseLength : 255-curPulseLength;
  if(baseParams.brightLength < 255) { baseParams.brightLength++; } // To sync up the changes in brightLength and transLength after they're scaled from 0:dimPeriod/3, (at least at default value) todo: does this work for all dimPeriods?
  baseParams.transLength = transLow ? 255-curPulseLength : curPulseLength;
  
  
  //if(brightMove) { baseParams.brightLength = brightLow ? curPulseLength : 255 - curPulseLength; }
  //if(transMove)  { baseParams.transLength  = transLow  ? curPulseLength : 255 - curPulseLength; }
*/
}

void LEDLoop::RandomizeBaseParams() {
  uint8_t paramToChange = random8(4);
  switch(paramToChange) {
    case 0:
      baseParams.dimSpeed = 127 - random8();
      break;
    case 1:
      baseParams.colorSpeed = 127 - random8();
      break;
    case 2:
      baseParams.numColors = random8();
      break;
    case 3:
      baseParams.displayMode = random8();
      break;
    default:
      break;
  }

  PrintBaseParams(IS_INTERIOR_LOOP);
}

void LEDLoop::WalkTopParams() {
  PrintTopParams(IS_INTERIOR_LOOP);
}

void LEDLoop::WalkPaletteManagerParams() {
  PrintPaletteManagerParams(IS_INTERIOR_LOOP);
}

void LEDLoop::WalkLayerParams() {
  PrintLayerParams(IS_INTERIOR_LOOP);
}
