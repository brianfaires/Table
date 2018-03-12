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
          #ifdef DEBUG_BUTTONS
            Serial.println("Button 1 long press tick");
          #endif
        }
      }
    }
  }
  else {
    // Button 1 open
    if(btn1_lastClicked != 0 && timing.now - btn1_lastClicked > DOUBLE_CLICK_TIME) {
      btn1_lastClicked = 0;
      btn1_pressed = false;
      #ifdef DEBUG_BUTTONS
        Serial.println("Button 1 single click");
      #endif

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
        #ifdef DEBUG_BUTTONS
          Serial.println("Button 1 double click");
        #endif
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


//----------------------------- Serial IO -----------------------------
bool ProcessSerialInput() {
  #if defined(DEBUG_SERIAL) && defined(MANUAL_PARAMS)
    if(Serial.available() > 0) {
      String s = Serial.readString();
      s.trim();
      Serial.println("\nEntered: " + s);

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
        return true;
      }
      else if(s.startsWith("np")) {
        pm.NextPalette(timing.now);
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
      
      long paramNum = s.toInt();
      if(paramNum < 0 || paramNum > NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS) { return false; }
      if(paramNum < 10) { s = s.substring(1); }
      else { s = s.substring(2); }
      long value = s.toInt();
  
      if(paramNum == 0) {
        if(value < 0 || value > NUM_BASE_ANIMATIONS) { return false; }
      }
      else if(paramNum == 9) {
        if(value < 0 || value > NUM_TOP_ANIMATIONS) { return false; }
      }
      else if(paramNum == 1 || paramNum == 2 || paramNum == 11) {
        if(value < -128 || value > 127) { return false; }
      }
      else if((paramNum >= 3 && paramNum <= 8) || (paramNum >= 10 && paramNum <= 10)){ 
        if(value < 0 || value > 255) { return false; }
      }
      else if((paramNum >= 12 && paramNum <= 19)) {
        if(value < 0 || value > 4294967) { return false; } // 4294967 is max uint32_t / 1000
      }
  
      if(paramNum == 0) {
        CleanupBaseLayer(baseParams.animation);
        baseParams.animation = value;
        InitBaseLayer();
      }
      else if(paramNum == 1) { baseParams.colorSpeed = value; }
      else if(paramNum == 2) { baseParams.brightnessSpeed = value; }
      else if(paramNum == 3) { baseParams.spacing = value; }
      else if(paramNum == 4) { baseParams.colorThickness = value; }
      else if(paramNum == 5) { baseParams.brightLength = value; }
      else if(paramNum == 6) { baseParams.transLength = value; }
      else if(paramNum == 7) { baseParams.numColors = value; }
      else if(paramNum == 8) { baseParams.displayMode = value; }
      else if(paramNum == 9) {
        CleanupTopLayer(topParams.animation);
        topParams.animation = value;
        InitTopLayer();
      }
      else if(paramNum == 10) { topParams.portion = value; }
      else if(paramNum == 11) { topParams.speed = value; }
      else if(paramNum == 12) { pm.SetPauseLength(1000 * value, timing.now); }
      else if(paramNum == 13) { pm.SetWalkLength(1000 * value, timing.now); }
      else if(paramNum == 14) { config.basePauseLength = 1000 * value; }
      else if(paramNum == 15) { config.baseTransOutLength = 1000 * value; }
      else if(paramNum == 16) { config.baseTransInLength = 1000 * value; }
      else if(paramNum == 17) { config.baseParamWalkTime = 1000 * value; }
      else if(paramNum == 18) { config.topPauseLength = 1000 * value; }
      else if(paramNum == 19) { config.topTransOutLength = 1000 * value; }
      else if(paramNum == 20) { config.topTransInLength = 1000 * value; }
      else if(paramNum == 21) { config.topParamWalkTime = 1000 * value; }
      else { return false; }
  
      return true;
    }
  #endif
  
  return false;
}

void PrintParams() {
  #ifdef DEBUG_SERIAL
    Serial.println("\nCommands: np (NextPalette), nb (NextBase), nt (NextTop)");
    Serial.println("\t\tsp # ### ### ### (SetPalette)");
    PrintBaseParams();
    PrintTopParams();
    PrintPaletteManagerParams();
    PrintLayerParams();

    #ifdef TEST_PALETTES
      String output = "";
      for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
        output += String(i) + ": (" + String(pm.palette[i].h) + ", " +  String(pm.palette[i].s) + ", " +  String(pm.palette[i].v) + ")\n";
      }
      Serial.print(output);
    #endif
  #endif
}

void PrintBaseParams() {
  #ifdef DEBUG_SERIAL
    uint8_t parameterCounter = 0;
    String output = "Base Parameters:\n";
    
    output += "\t" + String(parameterCounter++) + ".Animation:\t  " + String(baseParams.animation) + "\n";
    output += "\t" + String(parameterCounter++) + ".ColorSpeed:\t  " + String(baseParams.colorSpeed) + "\n";
    output += "\t" + String(parameterCounter++) + ".BrightSpeed:\t  " + String(baseParams.brightnessSpeed) + "\n";
    output += "\t" + String(parameterCounter++) + ".Spacing:\t  " + String(baseParams.spacing) + "\n";
    output += "\t" + String(parameterCounter++) + ".ColorThickness: " + String(baseParams.colorThickness) + "\n";
    output += "\t" + String(parameterCounter++) + ".BrightLength:\t  " + String(baseParams.brightLength) + "\n";
    output += "\t" + String(parameterCounter++) + ".TransLength:\t  " + String(baseParams.transLength) + "\n";
    output += "\t" + String(parameterCounter++) + ".NumColors:\t  " + String(baseParams.numColors) + "\n";
    output += "\t" + String(parameterCounter++) + ".DisplayMode:\t  " + String(baseParams.displayMode) + "\n";

    if(parameterCounter != NUM_BASE_PARAMS) { output += "ERROR: PrintBaseParams(), parameter count mismatch.\n"; }
    Serial.print(output);
  #endif
}

void PrintTopParams() {
  #ifdef DEBUG_SERIAL
    uint8_t parameterCounter = NUM_BASE_PARAMS;
    String output = "Top Parameters:\n";
    
    output += "\t" + String(parameterCounter++) + ".Animation:\t  " + String(topParams.animation) + "\n";
    output += "\t" + String(parameterCounter++) + ".Portion:\t  " + String(topParams.portion) + "\n";
    output += "\t" + String(parameterCounter++) + ".Speed:\t  " + String(topParams.speed) + "\n";
    
    if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS) { output += "ERROR: PrintTopParams(), parameter count mismatch.\n"; }
    Serial.print(output);
  #endif
}

void PrintPaletteManagerParams() {
  #ifdef DEBUG_SERIAL
    uint8_t parameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS;
    String output = "PaletteManager Parameters:\n";
    
    output += "\t" + String(parameterCounter++) + ".PauseLength:\t  " + String(pm.GetPauseLength()/1000) + "\n";
    output += "\t" + String(parameterCounter++) + ".WalkLength:\t  " + String(pm.GetWalkLength()/1000) + "\n";
    
    if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS) { output += "ERROR: PrintPaletteManagerParams(), parameter count mismatch.\n"; }
    Serial.print(output);
  #endif
}

void PrintLayerParams() {
  #ifdef DEBUG_SERIAL
    uint8_t parameterCounter = NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS;
    String output = "Base Layer Parameters:\n";

    output += "\t" + String(parameterCounter++) + ".PauseLength:\t   " + String(config.basePauseLength/1000) + "\n";
    output += "\t" + String(parameterCounter++) + ".TransOutLength: " + String(config.baseTransOutLength/1000) + "\n";
    output += "\t" + String(parameterCounter++) + ".TransInLength:  " + String(config.baseTransInLength/1000) + "\n";
    output += "\t" + String(parameterCounter++) + ".ParamWalkTime:  " + String(config.baseParamWalkTime/1000) + "\n";
    output += "Top Layer Parameters:\n";
    output += "\t" + String(parameterCounter++) + ".PauseLength:\t   " + String(config.topPauseLength/1000) + "\n";
    output += "\t" + String(parameterCounter++) + ".TransOutLength: " + String(config.topTransOutLength/1000) + "\n";
    output += "\t" + String(parameterCounter++) + ".TransInLength:  " + String(config.topTransInLength/1000) + "\n";
    output += "\t" + String(parameterCounter++) + ".ParamWalkTime:  " + String(config.topParamWalkTime/1000) + "\n";
    
    if(parameterCounter != NUM_BASE_PARAMS + NUM_TOP_PARAMS + NUM_PM_PARAMS + NUM_LAYER_PARAMS) { output += "ERROR: PrintLayerParams(), parameter count mismatch.\n"; }
    Serial.print(output);
  #endif
}

