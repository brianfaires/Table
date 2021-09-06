/* Uses palettes defined in PaletteManagerConfig.h.  Accepts params and handles smooth fading between them with timing according to the set params.
   Since this is intended to be used with GammaManager.h, all colors will go through Gamma correction.
   Blending is done in CHSV space, even though this means colors will be double Gamma Corrected, overly saturating them. Considering this preferable to blending in RGB and diluting them.
*/
#pragma once
#include "Arduino.h"
#include "FastLED.h"

#define PALETTE_SIZE 6

enum class PaletteIndex : uint8_t;

class PaletteManager {
  public:
	/// Fields
	CHSV palette[PALETTE_SIZE];
	
	/// Object management
  PaletteManager(); 
  void Init(uint32_t* _curTime, uint32_t intialWalkLength, uint32_t intialPauseLength, PaletteIndex initialPalette, bool fadeInFromBlack = false);
  void SkipTime(uint32_t amount);
	
    /// Accessors
	PaletteIndex getTarget();
  void setTarget(PaletteIndex newTarget);

  uint32_t getWalkLength();
  void setWalkLength(uint32_t newWalkLength);
  
  uint32_t getPauseLength();
  void setPauseLength(uint32_t newPauseLength);

	/// Logic
  void Update();
  void NextPalette();


  private:
	  uint32_t* curTime;
    PaletteIndex target;
    uint32_t walkLength;
    uint32_t pauseLength;
    
    CHSV oldPalette[PALETTE_SIZE];
    uint32_t lastSwitchTime;
};
