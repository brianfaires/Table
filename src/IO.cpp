#include "Globals.h"

const uint32_t DEBOUNCE_TIME = 30000;
const uint32_t DOUBLE_CLICK_TIME = 300000;
const uint32_t LONG_PRESS_INCREMENT = 250000;
const uint32_t LONG_PRESS_INIT_TIME = 500000;

uint32_t btn1_lastPressed = 0;
uint32_t btn1_lastClicked = 0;
uint32_t btn1_debounceStart = 0;
uint32_t btn1_pressTime = 0;

void UpdateIO() {
static bool btn1_pressed = false;

  if(timing.now - btn1_debounceStart <= DEBOUNCE_TIME) {
    // Debouncing
    return;
  }

  if(digitalRead(BTN1_PIN) == LOW) {
    // Button 1 pressed
    if(!btn1_pressed) {
      // State change from open to closed
      btn1_debounceStart = timing.now;
      btn1_lastPressed = timing.now;
      btn1_pressTime = 0;
      btn1_pressed = true;
    }
    else {
      // Pressed but not released
      if(timing.now - btn1_lastPressed > LONG_PRESS_INIT_TIME) {
        bool firstTick = btn1_pressTime == 0;
        uint32_t last_btn1_pressTime = btn1_pressTime;
        btn1_pressTime = timing.now - btn1_lastPressed;
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
    if(btn1_lastClicked != 0 && timing.now - btn1_lastClicked > DOUBLE_CLICK_TIME) {
      btn1_lastClicked = 0;
      btn1_pressed = false;
      DEBUG_BTN("Button 1 single click");
      NextBaseAnimation(timing.now);
    }

    if(btn1_pressed) {
      // Button 1 released
      btn1_pressed = false;
      btn1_debounceStart = timing.now;
    
      if(timing.now - btn1_lastClicked <= DOUBLE_CLICK_TIME) {
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
          btn1_lastClicked = timing.now;
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


#define NUM_PM_PARAMS 2 // Todo: Define this in PaletteManager

//----------------------------- Serial IO -----------------------------
bool ProcessSerialInput() {
  #ifdef MANUAL_PARAMS
    if(Serial.available() > 0) {
      String s = Serial.readString();
      s.trim();
      PRINTLN("\nEntered: " + s);

      if(s.startsWith("sp")) {
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
        
        pm.palette[colorNum] = CHSV(h,sat,v);
        String output = "";
        for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
          output += String(i) + ": " + pm.palette[i].ToString();
          CRGB temp = pm.palette[i];
          output += "\tRGB: " + temp.ToString();
        }
        PRINT(output);
        return true;
      }
      else if(s.startsWith("np")) {
        pm.NextPalette();
        return true;
      }
      else if(s.startsWith("nb")) {
        NextBaseAnimation(timing.now);
        return true;
      }
      else if(s.startsWith("nt")) {
        NextTopAnimation(timing.now);
        return true;
      }
      else if(s.startsWith("sb")) {
        s = s.substring(2);
        long b = s.toInt();
        if(b < 0 || b > 255) { return false; }
        globalBrightness = b;
        return true;
      }
      else if(s.startsWith("rb")) {
        baseParams.numColors = random8();
        baseParams.colorSpeed = random8() - 128;
        baseParams.dimSpeed = random8() - 128;
        baseParams.brightLength = random8();
        baseParams.transLength = random8();
        baseParams.displayMode = random8();
        baseParams.dimPeriod = random8();
        baseParams.colorPeriod = random8();
        return true;
      }
      else if(s.startsWith("rt")) {
        topParams.portion = random8();
        topParams.speed = random8();
        return true;
      }
      
      long paramNum = s.toInt();
      if(paramNum < 0 || paramNum >= NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS + NUM_PATTERN_PARAMS) { return false; }
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

      uint8_t next = 0;
      if(paramNum == next++) {
        CleanupBaseLayer(baseParams.animation);
        baseParams.animation = value;
        InitBaseLayer();
      }
      else if(paramNum == next++) { baseParams.colorSpeed = value; }
      else if(paramNum == next++) { baseParams.dimSpeed = value; }
      else if(paramNum == next++) { baseParams.brightLength = value; }
      else if(paramNum == next++) { baseParams.transLength = value; }
      else if(paramNum == next++) { baseParams.numColors = value; }
      else if(paramNum == next++) { baseParams.displayMode = value; }
      else if(paramNum == next++) { baseParams.dimPeriod = value; }
      else if(paramNum == next++) { baseParams.colorPeriod = value; }
      else if(paramNum == next++) {
        CleanupTopLayer(topParams.animation);
        topParams.animation = value;
        InitTopLayer();
      }
      else if(paramNum == next++) { topParams.portion = value; }
      else if(paramNum == next++) { topParams.speed = value; }
      else if(paramNum == next++) { pm.SetPauseLength(IO_TIME_FACTOR * value); }
      else if(paramNum == next++) { pm.SetWalkLength(IO_TIME_FACTOR * value); }
      else if(paramNum == next++) { layerConfig.basePauseLength = IO_TIME_FACTOR * value; }
      else if(paramNum == next++) { layerConfig.baseTransOutLength = IO_TIME_FACTOR * value; }
      else if(paramNum == next++) { layerConfig.baseTransInLength = IO_TIME_FACTOR * value; }
      else if(paramNum == next++) { layerConfig.baseParamWalkTime = IO_TIME_FACTOR * value; }
      else if(paramNum == next++) { layerConfig.topPauseLength = IO_TIME_FACTOR * value; }
      else if(paramNum == next++) { layerConfig.topTransOutLength = IO_TIME_FACTOR * value; }
      else if(paramNum == next++) { layerConfig.topTransInLength = IO_TIME_FACTOR * value; }
      else if(paramNum == next++) { layerConfig.topParamWalkTime = IO_TIME_FACTOR * value; }
      else if(paramNum == next++) { pc.SetColorPauseLength(IO_TIME_FACTOR * value); }
      else if(paramNum == next++) { pc.SetColorBlendLength(IO_TIME_FACTOR * value); }
      else if(paramNum == next++) { pc.SetDimPauseLength(IO_TIME_FACTOR * value); }
      else if(paramNum == next++) { pc.SetDimBlendLength(IO_TIME_FACTOR * value); }
      else if(paramNum == next++) { pc.SetDimParamChangeType((param_change_type)(value % NUM_DIM_PARAM_CHANGE_TYPES), value >= NUM_DIM_PARAM_CHANGE_TYPES); }
      else { THROW("Invalid paramNum") return false; }
  
      return true;
    }
  #endif
  
  return false;
}

void PrintParams() {
  #ifndef TEST_PALETTES
    PRINTLN("\nCommands: np (NextPalette), nb (NextBase), nt (NextTop)\n\t  rb (RandomizeBaseParams), rt (RandomizeTopParams)\n\t  sp # ### ### ### (SetPalette)");
    PrintBaseTopAndPMParams();
    PrintLayerParams();
    PrintPatternControllerParams(); 
  #endif
}

void PrintBaseTopAndPMParams() {
  uint8_t parameterCounter = 0;
  uint8_t topParameterCounter = NUM_BASE_PARAMS;
  uint8_t pmParameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS;
  String output = "Base Parameters:\t\tTop Parameters:\n";
  
  output += String(parameterCounter++) + ".Animation:\t  " + String(baseParams.animation) + "\t\t" + String(topParameterCounter++) + ".Animation:\t  " + String(topParams.animation) + "\n";
  output += String(parameterCounter++) + ".ColorSpeed:\t  " + String(baseParams.colorSpeed) + "\t\t" + String(topParameterCounter++) + ".Portion:\t  " + String(topParams.portion) + "\n";
  output += String(parameterCounter++) + ".BrightSpeed:\t  " + String(baseParams.dimSpeed) + "\t\t" + String(topParameterCounter++) + ".Speed:\t  " + String(topParams.speed) + "\n";
  output += String(parameterCounter++) + ".BrightLength:\t  " + String(baseParams.brightLength) + "\n";
  output += String(parameterCounter++) + ".TransLength:\t  " + String(baseParams.transLength) + "\t\tPaletteManager Parameters:\n";
  output += String(parameterCounter++) + ".NumColors:\t  " + String(baseParams.numColors) + "\t\t" + String(pmParameterCounter++) + ".PauseLength:\t  " + String(pm.GetPauseLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DisplayMode:\t  " + String(baseParams.displayMode) + "\t\t" + String(pmParameterCounter++) + ".WalkLength:\t  " + String(pm.GetWalkLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DimPeriod:\t  " + String(baseParams.dimPeriod) + "\n";
  output += String(parameterCounter++) + ".ColorPeriod:\t  " + String(baseParams.colorPeriod) + "\n";

  if(parameterCounter != NUM_BASE_PARAMS) { DUMP(parameterCounter) }
  if(topParameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS) { DUMP(topParameterCounter) }
  if(pmParameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS) { DUMP(pmParameterCounter) }
  PRINTLN(output);
}

void PrintBaseParams() {
  uint8_t parameterCounter = 0;
  String output = "Base Parameters:\n";
  
  output += "\t" + String(parameterCounter++) + ".Animation:\t  " + String(baseParams.animation) + "\n";
  output += "\t" + String(parameterCounter++) + ".ColorSpeed:\t  " + String(baseParams.colorSpeed) + "\n";
  output += "\t" + String(parameterCounter++) + ".BrightSpeed:\t  " + String(baseParams.dimSpeed) + "\n";
  output += "\t" + String(parameterCounter++) + ".BrightLength:\t  " + String(baseParams.brightLength) + "\n";
  output += "\t" + String(parameterCounter++) + ".TransLength:\t  " + String(baseParams.transLength) + "\n";
  output += "\t" + String(parameterCounter++) + ".NumColors:\t  " + String(baseParams.numColors) + "\n";
  output += "\t" + String(parameterCounter++) + ".DisplayMode:\t  " + String(baseParams.displayMode) + "\n";
  output += "\t" + String(parameterCounter++) + ".DimPeriod:\t  " + String(baseParams.dimPeriod) + "\n";
  output += "\t" + String(parameterCounter++) + ".ColorPeriod:\t  " + String(baseParams.colorPeriod) + "\n";

  if(parameterCounter != NUM_BASE_PARAMS) { output += "ERROR: PrintBaseParams(), parameter count mismatch.\n"; }
  PRINT(output);
}

void PrintTopParams() {
  uint8_t parameterCounter = NUM_BASE_PARAMS;
  String output = "Top Parameters:\n";
  
  output += "\t" + String(parameterCounter++) + ".Animation:\t  " + String(topParams.animation) + "\n";
  output += "\t" + String(parameterCounter++) + ".Portion:\t  " + String(topParams.portion) + "\n";
  output += "\t" + String(parameterCounter++) + ".Speed:\t  " + String(topParams.speed) + "\n";
  
  if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS) { output += "ERROR: PrintTopParams(), parameter count mismatch.\n"; }
  PRINT(output);
}

void PrintPaletteManagerParams() {
  uint8_t parameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS;
  String output = "PaletteManager Parameters:\n";
  
  output += "\t" + String(parameterCounter++) + ".PauseLength:\t  " + String(pm.GetPauseLength()/IO_TIME_FACTOR) + "\n";
  output += "\t" + String(parameterCounter++) + ".WalkLength:\t  " + String(pm.GetWalkLength()/IO_TIME_FACTOR) + "\n";
  
  if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS) { output += "ERROR: PrintPaletteManagerParams(), parameter count mismatch.\n"; }
  PRINT(output);
}

void PrintLayerParams() {
  uint8_t parameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS;
  String output = "Base Layer Parameters:\t\tTop Layer Parameters:\n";

  output += String(parameterCounter) + ".PauseLength:\t   " + String(layerConfig.basePauseLength/IO_TIME_FACTOR) + "   \t" + String(parameterCounter+4) + ".PauseLength:\t   " + String(layerConfig.topPauseLength/IO_TIME_FACTOR) + "\n";
  parameterCounter++;
  output += String(parameterCounter) + ".TransOutLength: " + String(layerConfig.baseTransOutLength/IO_TIME_FACTOR) + "   \t" + String(parameterCounter+4) + ".TransOutLength: " + String(layerConfig.topTransOutLength/IO_TIME_FACTOR) + "\n";
  parameterCounter++;
  output += String(parameterCounter) + ".TransInLength:  " + String(layerConfig.baseTransInLength/IO_TIME_FACTOR) + "   \t" + String(parameterCounter+4) + ".TransInLength:  " + String(layerConfig.topTransInLength/IO_TIME_FACTOR) + "\n";
  parameterCounter++;
  output += String(parameterCounter) + ".ParamWalkTime:  " + String(layerConfig.baseParamWalkTime/IO_TIME_FACTOR) + "   \t" + String(parameterCounter+4) + ".ParamWalkTime:  " + String(layerConfig.topParamWalkTime/IO_TIME_FACTOR) + "\n";
  parameterCounter++;
  
  if(parameterCounter+4 != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS) { DUMP(parameterCounter) }
  PRINTLN(output);
}

void PrintPatternControllerParams() {
  uint8_t parameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS;
  String output = "Pattern Controller Parameters:\n";
  
  output += String(parameterCounter++) + ".ColorPauseLength:\t  " + String(pc.GetColorPauseLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".ColorBlendLength:\t  " + String(pc.GetColorBlendLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DimPauseLength:\t  " + String(pc.GetDimPauseLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DimBlendLength:\t  " + String(pc.GetDimBlendLength()/IO_TIME_FACTOR) + "\n";
  output += String(parameterCounter++) + ".DimParamChangeType:\t  " + String(NUM_DIM_PARAM_CHANGE_TYPES*pc.GetChangeDimParamsWithMovement() + pc.GetDimParamChangeType()) + "\n";
  
  if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS + NUM_PATTERN_PARAMS) { output += "ERROR: PrintPaletteManagerParams(), parameter count mismatch.\n"; }
  PRINT(output);
}
