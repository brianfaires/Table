#include "Table.h"

uint8_t globalBrightness = INIT_GLOBAL_BRIGHTNESS;
PaletteManager pm;
LEDLoop interior;
LEDLoop upper;
uint32_t curTime;

#ifdef CHECK_FOR_CLIPPING
    uint32_t lastClippedTime = 0;
#endif

void setup() {
  TIMING_STARTUP

  // Sanity delay
  delay(STARTUP_DELAY_MS);

  // Initialize Serial
  Serial.begin(SERIAL_DATA_RATE_HZ);
  DEBUG("Serial comm intialized.");

  // Initialize Buttons
  pinMode(BTN1_PIN, INPUT_PULLUP);

  // Initialize LED Loops
  interior.Setup<LED_PIN_INTERIOR, CLOCK_PIN_INTERIOR, NUM_LEDS_INTERIOR>(&globalBrightness, &pm);
  DEBUG("Interior LEDLoop initialized.");

  upper.Setup<LED_PIN_UPPER, CLOCK_PIN_UPPER, NUM_LEDS_UPPER>(&globalBrightness, &pm);
  DEBUG("Upper LEDLoop initialized.");

  FastLED.show();
  DEBUG("LEDs defined and initialized.");

  // End of loop
  SkipTime(SYSTEM_TIME);
  DEBUG("setup() complete.");
  DEBUG_TIMING("setup() time: " + (SYSTEM_TIME - startupTime));

  // Prepare Serial IO 
  #ifdef MANUAL_PARAMS
    Serial.setTimeout(100);
    PrintParams();
  #endif
}


void loop() {
  TIMING_ANALYSIS_BEGIN_LOOP

  bool needToDraw = false;
  curTime = SYSTEM_TIME;

  UpdateAnimationParameters();
  //needToDraw |= interior.Loop(curTime); TIMING_ANALYSIS_POINT
  needToDraw |= upper.Loop(curTime); TIMING_ANALYSIS_POINT

  if(needToDraw) { FastLED.show(); } TIMING_ANALYSIS_POINT

  TIMING_ANALYSIS_END_LOOP
}


void SkipTime(uint32_t amount) {
  SkipTimeForIO(amount);
  pm.SkipTime(amount);
  interior.SkipTime(amount);
  upper.SkipTime(amount);
}


///////////////////// Param Walking ////////////////////////
void UpdateAnimationParameters() {
  #ifdef PULSE_BASE_PARAMS
    PulseBaseParams();
  #else
    /*
    if(curTime - timing.lastBaseParamWalk >= layerConfig.baseParamWalkTime) {
      RandomizeBaseParams();
      timing.lastBaseParamWalk = timing.now;
    }
    */
  #endif
  
  /*
  if(curTime - timing.lastTopParamWalk >= layerConfig.topParamWalkTime) {
    WalkTopParams();
    timing.lastTopParamWalk = timing.now;
  }
  */

  #ifdef MANUAL_PARAMS
    if(ProcessSerialInput()) { 
      PrintParams();
      SkipTime(SYSTEM_TIME - curTime);
    }
  #endif
}

void PulseBaseParams() {
  const uint32_t pulseLength = 0.75*ONE_SEC;//MIN / 128;
  
  static uint32_t curCycle = 0;
  static bool brightLow = false;
  static bool transLow = false;
  //static bool brightMove = false;
  //static bool transMove = true;

  if(SYSTEM_TIME / pulseLength != curCycle) {
    curCycle = SYSTEM_TIME / pulseLength;

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


  //float curPulseLength = float(SYSTEM_TIME % pulseLength) / pulseLength;
  //baseParams.transLength = transLow ? 255*curPulseLength : 255*(1-curPulseLength);
  //baseParams.brightLength = brightLow ? 255*curPulseLength : 255*(1-curPulseLength);
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

void RandomizeBaseParams(LEDLoop* loop) {
  uint8_t paramToChange = random8(4);
  switch(paramToChange) {
    case 0:
      loop->baseParams.dimSpeed = 127 - random8();
      break;
    case 1:
      loop->baseParams.colorSpeed = 127 - random8();
      break;
    case 2:
      loop->baseParams.numColors = random8();
      break;
    case 3:
      loop->baseParams.displayMode = random8();
      break;
    default:
      break;
  }

  PrintBaseParams(loop);
}

void WalkTopParams(LEDLoop* loop) {
  PrintTopParams(loop);
}

void WalkPaletteManagerParams(LEDLoop* loop) {
  PrintPaletteManagerParams(loop);
}

void WalkLayerParams(LEDLoop* loop) {
  PrintLayerParams(loop);
}






/////////////////////// Button IO //////////////////////////////
const uint32_t DEBOUNCE_TIME = 30000;
const uint32_t DOUBLE_CLICK_TIME = 300000;
const uint32_t LONG_PRESS_INCREMENT = 250000;
const uint32_t LONG_PRESS_INIT_TIME = 500000;

uint32_t btn1_lastPressed = 0;
uint32_t btn1_lastClicked = 0;
uint32_t btn1_debounceStart = 0;
uint32_t btn1_pressTime = 0;

void UpdateIO(uint32_t curTime) {
  static bool btn1_pressed = false;

  if(curTime - btn1_debounceStart <= DEBOUNCE_TIME) {
    // Debouncing
    return;
  }

  if(digitalRead(BTN1_PIN) == LOW) {
    // Button 1 pressed
    if(!btn1_pressed) {
      // State change from open to closed
      btn1_debounceStart = curTime;
      btn1_lastPressed = curTime;
      btn1_pressTime = 0;
      btn1_pressed = true;
    }
    else {
      // Pressed but not released
      if(curTime - btn1_lastPressed > LONG_PRESS_INIT_TIME) {
        bool firstTick = btn1_pressTime == 0;
        uint32_t last_btn1_pressTime = btn1_pressTime;
        btn1_pressTime = curTime - btn1_lastPressed;
        if(firstTick || (last_btn1_pressTime - LONG_PRESS_INIT_TIME) / LONG_PRESS_INCREMENT !=
                        (btn1_pressTime - LONG_PRESS_INIT_TIME) / LONG_PRESS_INCREMENT) {
          // Tick long press logic
          btn1_lastClicked = 0; // If double click into long press, throw away the first click
          DEBUG_BTN("Button 1 long press tick");
        }
      }
    }
  }
  else {
    // Button 1 open
    if(btn1_lastClicked != 0 && curTime - btn1_lastClicked > DOUBLE_CLICK_TIME) {
      btn1_lastClicked = 0;
      btn1_pressed = false;
      DEBUG_BTN("Button 1 single click");
      //NextBaseAnimation(curTime);
    }

    if(btn1_pressed) {
      // Button 1 released
      btn1_pressed = false;
      btn1_debounceStart = curTime;
    
      if(curTime - btn1_lastClicked <= DOUBLE_CLICK_TIME) {
        // Double click
        btn1_lastPressed = 0; // Consume click
        btn1_lastClicked = 0;
        DEBUG_BTN("Button 1 double click");
      }
      else {
        // Button 1 clicked and no double click occurred
        if(btn1_pressTime >= LONG_PRESS_INIT_TIME) {
          // End of long press
          btn1_pressTime = 0;
          btn1_lastPressed = 0;
          btn1_lastClicked = 0;
        }
        else {
          btn1_lastClicked = curTime;
        }
      }
    }
  }
}

void SkipTimeForIO(uint32_t amount) {
  if(btn1_lastPressed != 0) { btn1_lastPressed += amount; }
  if(btn1_lastClicked != 0) { btn1_lastClicked += amount; }
  if(btn1_debounceStart != 0) { btn1_debounceStart += amount; }
  if(btn1_pressTime != 0) { btn1_pressTime += amount; }
}




//----------------------------- Serial IO -----------------------------
bool interiorIsActive = true;
#define NUM_PM_PARAMS 2

bool ProcessSerialInput() {
  LEDLoop* loop = interiorIsActive ? &interior : &upper;

  #ifdef MANUAL_PARAMS
    if(Serial.available() <= 0) { return false; }

    String s = Serial.readString();
    s.trim();
    PRINTLN("\nEntered: " + s)

    if(s.startsWith("cs")) {
      interiorIsActive = !interiorIsActive;
      return true;
    }
    else if(s.startsWith("sp")) {
      // Set palette
      s = s.substring(3);
      long colorNum = s.toInt();
      if(colorNum < 0 || colorNum >= PALETTE_SIZE) { return false; }
      if(colorNum < 10) { s = s.substring(2); }
      else { s = s.substring(3); }
      
      long h = s.toInt();
      if(h < 0 || h > 255) { return false; }
      if(h < 10) { s = s.substring(2); }
      else if(h < 100) { s = s.substring(3); }
      else { s = s.substring(4); }

      long sat = s.toInt();
      if(sat < 0 || sat > 255) { return false; }
      if(sat < 10) { s = s.substring(2); }
      else if(sat < 100) { s = s.substring(3); }
      else { s = s.substring(4); }

      long v = s.toInt();
      if(v < 0 || v > 255) { return false; }
      
      loop->pm->palette[colorNum] = CHSV(h,sat,v);
      String output = "";
      for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
        output += String(i) + ": " + loop->pm->palette[i].ToString();
        CRGB temp = loop->pm->palette[i];
        output += "\tRGB: " + temp.ToString();
      }
      PRINT(output)
      return true;
    }
    else if(s.startsWith("tp")) {
      s = s.substring(2);
      long b = s.toInt();
      if(b < 0 || b >= 255) { return false; } // Todo: This does not have visibility on # of palettes
      loop->pm->setTarget(PaletteIndex(b));
      return true;
    }
    else if(s.startsWith("sb")) {
      s = s.substring(2);
      long b = s.toInt();
      if(b < 0 || b > 255) { return false; }
      *(loop->pGlobalBrightness) = b;
      return true;
    }
    else if(s.startsWith("rb")) {
      loop->baseParams.numColors = random8();
      loop->baseParams.colorSpeed = random8() - 128;
      loop->baseParams.dimSpeed = random8() - 128;
      loop->baseParams.brightLength = random16();
      loop->baseParams.transLength = random16();
      loop->baseParams.displayMode = random8();
      loop->baseParams.dimPeriod = random8();
      loop->baseParams.colorPeriod = random8();
      return true;
    }
    else if(s.startsWith("rt")) {
      loop->topParams.portion = random8();
      loop->topParams.speed = random8();
      return true;
    }
    else if(s.startsWith("np")) {
      loop->pm->NextPalette();
      return true;
    }
    else if(s.startsWith("nb")) {
      loop->NextBaseAnimation();
      return true;
    }
    else if(s.startsWith("nt")) {
      loop->NextTopAnimation();
      return true;
    }
    
    long paramNum = s.toInt();
    if(paramNum < 0 || paramNum >= NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS + NUM_PATTERN_PARAMS) { return false; }
    if(paramNum == 0 && s[0] != '0') { THROW("Invalid input") return false; }
    if(paramNum < 10) { s = s.substring(1); }
    else { s = s.substring(2); }
    long value = s.toInt();

    if(paramNum == 0) {
      if(value < 0 || value > NUM_BASE_ANIMATIONS) { THROW("OOB baseAnimation") return false; }
    }
    else if(paramNum == 9) {
      if(value < 0 || value > NUM_TOP_ANIMATIONS) { THROW("OOB topAnimation") return false; }
    }
    else if(paramNum == 1 || paramNum == 2 || paramNum == 11) {
      if(value < -128 || value > 127) { THROW("OOB int8_t") return false; }
    }
    else if((paramNum >= 3 && paramNum <= 8) || (paramNum >= 10 && paramNum <= 10)){ 
      if(value < 0 || value > 255) { THROW("OOB uint8_t") return false; }
    }
    else if((paramNum >= 12 && paramNum <= 25)) {
      if(value < 0 || value > 4294967) { THROW("OOB uint32_t") return false; } // 4294967 is max uint32_t / 1000
    }

    #define ASSIGN_PARAM(x) x = ((x)==0 && value==0) ? 0xFFFF : value;
    uint8_t next = 0;
    if(paramNum == next++) {
      loop->CleanupBaseLayer(loop->baseParams.animation);
      loop->baseParams.animation = BaseAnimation(value);
      loop->InitBaseLayer();
    }
    else if(paramNum == next++) { loop->baseParams.colorSpeed = value; }
    else if(paramNum == next++) { loop->baseParams.dimSpeed = value; }
    else if(paramNum == next++) { ASSIGN_PARAM(loop->baseParams.brightLength) }
    else if(paramNum == next++) { ASSIGN_PARAM(loop->baseParams.transLength) }
    else if(paramNum == next++) { ASSIGN_PARAM(loop->baseParams.numColors) }
    else if(paramNum == next++) { ASSIGN_PARAM(loop->baseParams.displayMode) }
    else if(paramNum == next++) { ASSIGN_PARAM(loop->baseParams.dimPeriod) }
    else if(paramNum == next++) { ASSIGN_PARAM(loop->baseParams.colorPeriod) }
    else if(paramNum == next++) {
      loop->CleanupTopLayer(loop->topParams.animation);
      loop->topParams.animation = TopAnimation(value);
      loop->InitTopLayer();
    }
    else if(paramNum == next++) { ASSIGN_PARAM(loop->topParams.portion) }
    else if(paramNum == next++) { ASSIGN_PARAM(loop->topParams.speed) }
    else if(paramNum == next++) { loop->pm->setPauseLength(IO_TIME_FACTOR * value); }
    else if(paramNum == next++) { loop->pm->setWalkLength(IO_TIME_FACTOR * value); }
    else if(paramNum == next++) { loop->layerConfig.basePauseLength = IO_TIME_FACTOR * value; }
    else if(paramNum == next++) { loop->layerConfig.baseTransOutLength = IO_TIME_FACTOR * value; }
    else if(paramNum == next++) { loop->layerConfig.baseTransInLength = IO_TIME_FACTOR * value; }
    else if(paramNum == next++) { loop->layerConfig.baseParamWalkTime = IO_TIME_FACTOR * value; }
    else if(paramNum == next++) { loop->layerConfig.topPauseLength = IO_TIME_FACTOR * value; }
    else if(paramNum == next++) { loop->layerConfig.topTransOutLength = IO_TIME_FACTOR * value; }
    else if(paramNum == next++) { loop->layerConfig.topTransInLength = IO_TIME_FACTOR * value; }
    else if(paramNum == next++) { loop->layerConfig.topParamWalkTime = IO_TIME_FACTOR * value; }
    else if(paramNum == next++) { loop->pc.setColorPauseLength(IO_TIME_FACTOR * value); }
    else if(paramNum == next++) { loop->pc.setColorBlendLength(IO_TIME_FACTOR * value); }
    else if(paramNum == next++) { loop->pc.setDimPauseLength(IO_TIME_FACTOR * value); }
    else if(paramNum == next++) { loop->pc.setDimBlendLength(IO_TIME_FACTOR * value); }
    else if(paramNum == next++) { loop->pc.setDimPatternChangeType(DimPatternChangeType(value % NUM_DIM_PARAM_CHANGE_TYPES), value >= NUM_DIM_PARAM_CHANGE_TYPES); }
    else { THROW("Invalid paramNum") return false; }

    return true;
  #endif
  
  return false;
}

void PrintParams() {
  #ifndef TEST_COLOR_CORRECTION
    PRINTLN((interiorIsActive ? "\n-------- InteriorStrip --------" : "\n--------  Upper Strip --------"))
    PRINTLN(F("Commands: cs (ChangeStrip), np (NextPalette), nb (NextBase), nt (NextTop)\n\t  sb ### (setBrightness), rb (RandomizeBaseParams), rt (RandomizeTopParams)\n\t  tp (TargetPalette), sp # ### ### ### (SetPalette)"))
    
    LEDLoop* activeLoop = interiorIsActive ? &interior : &upper;
    PrintBaseTopAndPMParams(activeLoop);
    PrintLayerParams(activeLoop);
    PrintPatternControllerParams(activeLoop);
  #endif
}

void PrintBaseTopAndPMParams(LEDLoop* loop) {
  uint8_t parameterCounter = 0;
  uint8_t topParameterCounter = NUM_BASE_PARAMS;
  uint8_t pmParameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS;
  
  String output = "Base Parameters:\t\tTop Parameters:\n";
  
  output += String(parameterCounter++) + ".Animation:\t  " + uint8_t(loop->baseParams.animation) + "\t\t" + (topParameterCounter++) + ".Animation:\t  " + (uint8_t)loop->topParams.animation + "\n";
  output += String(parameterCounter++) + ".ColorSpeed:\t  " + loop->baseParams.colorSpeed + "\t\t" + (topParameterCounter++) + ".Portion:\t  " + loop->topParams.portion + "\n";
  output += String(parameterCounter++) + ".DimSpeed:\t  " + loop->baseParams.dimSpeed + "\t\t" + (topParameterCounter++) + ".Speed:\t  " + loop->topParams.speed + "\n";
  output += String(parameterCounter++) + ".BrightLength:\t  " + loop->baseParams.brightLength + "\n";
  output += String(parameterCounter++) + ".TransLength:\t  " + loop->baseParams.transLength + "\t\tPaletteManager Parameters:\n";
  output += String(parameterCounter++) + ".NumColors:\t  " + loop->baseParams.numColors + "\t\t" + (pmParameterCounter++) + ".PauseLength:\t  " + (loop->pm->getPauseLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DisplayMode:    " + loop->baseParams.displayMode + "\t\t" + (pmParameterCounter++) + ".WalkLength:\t  " + (loop->pm->getWalkLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DimPeriod:\t  " + loop->baseParams.dimPeriod + "\n";
  output += String(parameterCounter++) + ".ColorPeriod:\t  " + loop->baseParams.colorPeriod + "\n";

  if(parameterCounter != NUM_BASE_PARAMS) { DUMP(parameterCounter) }
  if(topParameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS) { DUMP(topParameterCounter) }
  if(pmParameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS) { DUMP(pmParameterCounter) }
  PRINTLN(output)
}

void PrintBaseParams(LEDLoop* loop) {
  uint8_t parameterCounter = 0;
  String output = "Base Parameters:\n";

  output += "\t" + String(parameterCounter++) + ".Animation:\t  " + uint8_t(loop->baseParams.animation) + "\n";
  output += "\t" + String(parameterCounter++) + ".ColorSpeed:\t  " + loop->baseParams.colorSpeed + "\n";
  output += "\t" + String(parameterCounter++) + ".DimSpeed:\t  " + loop->baseParams.dimSpeed + "\n";
  output += "\t" + String(parameterCounter++) + ".BrightLength:\t  " + loop->baseParams.brightLength + "\n";
  output += "\t" + String(parameterCounter++) + ".TransLength:\t  " + loop->baseParams.transLength + "\n";
  output += "\t" + String(parameterCounter++) + ".NumColors:\t  " + loop->baseParams.numColors + "\n";
  output += "\t" + String(parameterCounter++) + ".DisplayMode:\t  " + loop->baseParams.displayMode + "\n";
  output += "\t" + String(parameterCounter++) + ".DimPeriod:\t  " + loop->baseParams.dimPeriod + "\n";
  output += "\t" + String(parameterCounter++) + ".ColorPeriod:\t  " + loop->baseParams.colorPeriod + "\n";
  
  PRINT(output)
  if(parameterCounter != NUM_BASE_PARAMS) { THROW_DUMP("Parameter count mismatch.", parameterCounter) }
}

void PrintTopParams(LEDLoop* loop) {
  uint8_t parameterCounter = NUM_BASE_PARAMS;
  String output = "Top Parameters:\n";

  output += "\t" + String(parameterCounter++) + ".Animation:\t  " + uint8_t(loop->topParams.animation) + "\n";
  output += "\t" + String(parameterCounter++) + ".Portion:\t  " + loop->topParams.portion + "\n";
  output += "\t" + String(parameterCounter++) + ".Speed:\t  " + loop->topParams.speed + "\n";
  PRINT(output)

  if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS) { THROW_DUMP("Parameter count mismatch.", parameterCounter) }

}

void PrintPaletteManagerParams(LEDLoop* loop) {
  uint8_t parameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS;
  String output = "PaletteManager Parameters:\n";

  output += "\t" + String(parameterCounter++) + ".PauseLength:\t  " + (loop->pm->getPauseLength()/IO_TIME_FACTOR) + "\n";
  output += "\t" + String(parameterCounter++) + ".WalkLength:  " + (loop->pm->getWalkLength()/IO_TIME_FACTOR) + "\n";
  
  PRINT(output)
  if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS) { THROW_DUMP("Parameter count mismatch.", parameterCounter) }
}

void PrintLayerParams(LEDLoop* loop) {
  uint8_t parameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS;
  String output = "Base Layer Parameters:\t\tTop Layer Parameters:\n";

  output += String(parameterCounter) + ".PauseLength:\t   " + (loop->layerConfig.basePauseLength/IO_TIME_FACTOR) + "   \t" + (parameterCounter+4) + ".PauseLength:\t   " + (loop->layerConfig.topPauseLength/IO_TIME_FACTOR) + "\n";
  parameterCounter++;
  output += String(parameterCounter) + ".TransOutLength: " + (loop->layerConfig.baseTransOutLength/IO_TIME_FACTOR) + "   \t" + (parameterCounter+4) + ".TransOutLength: " + (loop->layerConfig.topTransOutLength/IO_TIME_FACTOR) + "\n";
  parameterCounter++;
  output += String(parameterCounter) + ".TransInLength:  " + (loop->layerConfig.baseTransInLength/IO_TIME_FACTOR) + "   \t" + (parameterCounter+4) + ".TransInLength:  " + (loop->layerConfig.topTransInLength/IO_TIME_FACTOR) + "\n";
  parameterCounter++;
  output += String(parameterCounter) + ".ParamWalkTime:  " + (loop->layerConfig.baseParamWalkTime/IO_TIME_FACTOR) + "   \t" + (parameterCounter+4) + ".ParamWalkTime:  " + (loop->layerConfig.topParamWalkTime/IO_TIME_FACTOR) + "\n";
  parameterCounter++;
  
  PRINTLN(output)
  if(parameterCounter+4 != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS) { THROW_DUMP("Parameter count mismatch.", parameterCounter) }
}

void PrintPatternControllerParams(LEDLoop* loop) {
  uint8_t parameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS;
  String output = "Pattern Controller Parameters:\n";
  
  output += String(parameterCounter++) + ".ColorPauseLength:\t  " + (loop->pc.getColorPauseLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".ColorBlendLength:\t  " + (loop->pc.getColorBlendLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DimPauseLength:\t  " + (loop->pc.getDimPauseLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DimBlendLength:\t  " + (loop->pc.getDimBlendLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DimPatternChangeType:  " + (NUM_DIM_PARAM_CHANGE_TYPES*loop->pc.getChangeDimParamsWithMovement() + int(loop->pc.getDimPatternChangeType())) + "\n";
  
  PRINT(output)
  if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS + NUM_PATTERN_PARAMS) { THROW_DUMP("Parameter count mismatch.", parameterCounter) }
}
