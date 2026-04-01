#pragma once

#include <cstdint>
#include <optional>

#include "CommandQueue.h"
#include "FillLevelEstimator.h"
#include "IActuatorDrive.h"
#include "IDisplay.h"
#include "InputSampler.h"
#include "PapppresseTypes.h"
#include "PressFsm.h"
#include "StateQueue.h"

class PressController {
public:
    PressController(
        PressFsm& fsm,
        InputSampler& inputSampler,
        IActuatorDrive& actuatorDrive,
        IDisplay& display,
        CommandQueue& commandQueue,
        StateQueue& stateQueue,
        FillLevelEstimator& fillLevelEstimator);

    void controlStep(uint32_t nowMs);

    void estopInterrupt();
    void doorOpenedInterrupt();

private:
    void applyDriveCommand(DriveCommand cmd);
    void handleCommand(PressCommand cmd);
    void updateDisplay(PressState state);
    void onStateTransition(PressState oldState, PressState newState, uint32_t nowMs);
    void updateFillLevelOnTransition(PressState oldState, PressState newState, uint32_t nowMs);

    PressFsm& fsm_;
    InputSampler& inputSampler_;
    IActuatorDrive& actuatorDrive_;
    IDisplay& display_;
    CommandQueue& commandQueue_;
    StateQueue& stateQueue_;
    FillLevelEstimator& fillLevelEstimator_;

    bool startOverride_{false};
    bool estopLatched_{false};
    bool doorOpenLatched_{false};
    std::optional<PressState> lastEnqueuedState_{};
    std::optional<PressState> lastDisplayedState_{};
};
