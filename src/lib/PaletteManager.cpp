#include "lib/PaletteManager.h"
#include "config/PaletteManagerConfig.h"

/// Object management
PaletteManager::PaletteManager() { }
void PaletteManager::Init(uint32_t* _curTime, uint32_t initialWalkLength, uint32_t intialPauseLength, PaletteIndex initialPalette, bool fadeInFromBlack) {
  curTime = _curTime;
  walkLength = initialWalkLength;
  pauseLength = intialPauseLength;
  target = initialPalette;
  if(fadeInFromBlack) {
    lastSwitchTime = *curTime;
    memset(oldPalette, 0, sizeof(CHSV)*PALETTE_SIZE);
    memset(palette, 0, sizeof(CHSV)*PALETTE_SIZE);
  }
  else {
    NextPalette(); // Loads initialPalette, sets next target, and resets timer
  }
}

void PaletteManager::SkipTime(uint32_t amount) {
  lastSwitchTime += amount;
}

/// Accessors
PaletteIndex PaletteManager::getTarget() { return target; }
void PaletteManager::setTarget(PaletteIndex newTarget) {
  target = PaletteIndex(uint8_t(newTarget) % NUM_PALETTES);
  
  if(*curTime - lastSwitchTime > pauseLength) {
    // Already blending. Reset target and start blending from current palette.
    memcpy(oldPalette, palette, sizeof(CHSV)*PALETTE_SIZE);
    lastSwitchTime = *curTime - pauseLength;
  }
}

uint32_t PaletteManager::getWalkLength() { return walkLength; }
void PaletteManager::setWalkLength(uint32_t newWalkLength) {
  if(*curTime - lastSwitchTime > pauseLength) {
    // Already blending.  Determine what % has been done and create same percentage in the new timer
    uint64_t blendTime = *curTime - lastSwitchTime - pauseLength;
    fract16 percBlend = blendTime * 0x10000 / walkLength;
    uint32_t newBlendTime = percBlend * uint64_t(newWalkLength) / 0x10000;
    lastSwitchTime = *curTime - pauseLength - newBlendTime;
  }

  walkLength = newWalkLength;
}

uint32_t PaletteManager::getPauseLength() { return pauseLength; }
void PaletteManager::setPauseLength(uint32_t newPauseLength) {
  if(*curTime - lastSwitchTime <= pauseLength) {
    // Haven't started blending yet
    if(*curTime - lastSwitchTime > newPauseLength) {
      // Start blending immediately; but start at 0
      lastSwitchTime = *curTime - newPauseLength;
    }
  }
  else {
    // Middle of blending, offset by difference to keep blend the same
    lastSwitchTime -= (newPauseLength - pauseLength);
  }

  pauseLength = newPauseLength;
}

/// Logic
void PaletteManager::Update() {
  const CHSV* targetPalette = allPalettes[uint8_t(target)];

  if(*curTime - lastSwitchTime >= pauseLength) {
    // Currently transitioning
    uint32_t transitionTime = *curTime - lastSwitchTime - pauseLength;
    if(transitionTime < walkLength) {
      fract8 blendAmount = uint64_t(transitionTime) * 0x100 / walkLength;
      for(uint8_t i = 0; i < PALETTE_SIZE; i++) {
        palette[i] = blend(oldPalette[i], targetPalette[i], blendAmount, SHORTEST_HUES);
      }
    }
    else {
      // Blending just finished
      NextPalette();
    }
  }
}

void PaletteManager::NextPalette() {
  memcpy(oldPalette, allPalettes[int(target)], sizeof(CHSV)*PALETTE_SIZE);
  memcpy(palette, allPalettes[int(target)], sizeof(CHSV)*PALETTE_SIZE);
  target = PaletteIndex((int(target) + 1) % NUM_PALETTES);
  lastSwitchTime = *curTime;
}

