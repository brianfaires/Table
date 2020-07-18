#pragma once
#include "Definitions.h"
#include "PaletteManager.h"

class Stackers {
  private:
    #define MAX_TRANS_TIME (6*ONE_SEC)
    #define MAX_STACKS 40
    #define DEFAULT_STACK_MODE StackMode::Shutters
    #define DEFAULT_MOVE_CLOCKWISE true
    #define PIXEL_BRIGHTNESS 100
    #define MIN_STACK_LENGTH 5
    
    #define NUM_TRANSITION_STATES 3
    enum class TransitionState : uint8_t { None, Empty, Full, Messy };
    
    #define NUM_STACK_MODES 4
    enum class StackMode : uint8_t { None, Shutters, StutterStep, Stack4, Stack4Mirror };

    const bool allowedModes[NUM_TRANSITION_STATES+1][NUM_STACK_MODES+1] = { { 0, 0, 0, 0, 0 },
                                                                            { 0, 1, 0, 1, 1 },   // Empty
                                                                            { 0, 1, 1, 1, 1 },   // Full
                                                                            { 0, 0, 0, 0, 0 } }; // Messy
    
    // Initialized values
    uint16_t numLEDs;
    PaletteManager* pm;
    CRGB* leds;
    uint8_t* leds_b;
    uint8_t numAllowedDimPeriods;
    std::vector<uint16_t> allowedDimPeriods;
    uint32_t* curTime;
    
    // Initialized params
    uint8_t numColors;
    uint16_t dimPeriod;
    uint8_t maxStackLength = 0;
    
  public:
    struct_stack stacks[MAX_STACKS];
    uint8_t numStacks = 0;
    uint32_t lastModeTransition;
    bool moveClockwise = true;
    bool isFirstCycleOfNewMode = true;
    uint8_t stackLength = 0;
    
    TransitionState transitionState = TransitionState::None;
    StackMode stackMode = StackMode::None;

    void Init(uint16_t _numLEDs, PaletteManager* _pm, CRGB* _leds, uint8_t* _leds_b, struct_base_show_params& _params, std::vector<uint16_t> _allowedDimPeriods, uint8_t _numAllowedDimPeriods, uint32_t* curTime);
    void Stacks();
    uint8_t CreateStacks(uint8_t mode = 0);

  private:
    void DrawStack(struct_stack& s);
    void DrawAllStacks();
    void MoveStack(struct_stack& s, bool clockwise);
    void MoveAllStacks(bool clockwise);

    uint8_t Shutters();
    uint8_t StutterStepBands();
    uint8_t StackSections(uint8_t numSections);
    uint8_t StackSectionsUp(uint8_t numSections, uint16_t& progress, uint8_t &curStep);
    uint8_t StackSectionsDown(uint8_t numSections, uint16_t& progress, uint8_t &curStep);
    uint8_t StackSections_Mirror(uint8_t numSections);
    uint8_t StackSectionsUp_Mirror(uint8_t numSections, uint16_t& progress, uint8_t &curStep);
    uint8_t StackSectionsDown_Mirror(uint8_t numSections, uint16_t& progress, uint8_t &curStep);
    
    void PrepForInsert(uint8_t numSections);
    void PrepForInsert_Mirror(uint8_t numSections);
    uint8_t WipeClean(uint8_t numSections, uint16_t progress);
};