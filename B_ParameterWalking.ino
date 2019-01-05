void UpdateAnimationParameters(uint32_t curTime) {
  if(timing.now - timing.lastBaseParamWalk >= layerConfig.baseParamWalkTime) {
    RandomizeBaseParams();
    timing.lastBaseParamWalk = timing.now;
  }
  
  if(timing.now - timing.lastTopParamWalk >= layerConfig.topParamWalkTime) {
    WalkTopParams();
    timing.lastTopParamWalk = timing.now;
  }

  //PulseBaseParams(curTime);
  
  #ifdef MANUAL_PARAMS
    if(ProcessSerialInput()) { 
      PrintParams();
      SkipTime(SYSTEM_TIME - curTime);
    }
  #endif
}

void PulseBaseParams(uint32_t curTime) {
  const uint32_t pulseLength = 2.3*ONE_SEC;//MIN / 128;
  
  static uint32_t curCycle = 0;
  static bool brightLow = false;
  static bool transLow = false;
  static bool brightMove = false;
  static bool transMove = true;
  
  if(curTime / pulseLength != curCycle) {
    curCycle = curTime / pulseLength;
    //if(brightMove) { brightLow = !brightLow; }
    //if(transMove) { transLow = !transLow; }
    transLow = !transLow;
    //brightMove = false;
    //transMove = !brightMove;
    //if(!brightMove) { transMove = true; }
    //else if(brightLow != transLow) { transMove = random8(2); }
  }

  //baseParams.transLength = transLow ? 0 : 255;
  //baseParams.brightLength = transLow ? 255 : 0;

  uint8_t curPulseLength = (curTime % pulseLength) * uint64_t(255) / pulseLength;
  baseParams.brightLength = transLow ? curPulseLength : 255-curPulseLength;
  if(baseParams.brightLength < 255) { baseParams.brightLength++; } // To sync up the changes in brightLength and transLength after they're scaled from 0:dimPeriod/3, (at least at default value) debug: does this work for all dimPeriods?
  baseParams.transLength = transLow ? 255-curPulseLength : curPulseLength;
  
  
  //if(brightMove) { baseParams.brightLength = brightLow ? curPulseLength : 255 - curPulseLength; }
  //if(transMove)  { baseParams.transLength  = transLow  ? curPulseLength : 255 - curPulseLength; }
}

void RandomizeBaseParams() {
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

  PrintBaseParams();
}

void WalkTopParams() {
  PrintTopParams();
}

void WalkPaletteManagerParams() {
  PrintPaletteManagerParams();
}

void WalkLayerParams() {
  PrintLayerParams();
}

