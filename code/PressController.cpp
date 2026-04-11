#include "PressController.h"

#include <string>

#include "esp_log.h"

namespace {
const char* toStateString(PressState state)
{
    switch (state) {
    case PressState::INIT_PAUSE:
        return "INIT_PAUSE";
    case PressState::INIT_UP:
        return "INIT_UP";
    case PressState::READY_TOP:
        return "READY_TOP";
    case PressState::PRESS_DOWN:
        return "PRESS_DOWN";
    case PressState::PAUSE_PRESS:
        return "PAUSE_PRESS";
    case PressState::RETURN_UP_ABORTED:
        return "RETURN_UP_ABORTED";
    case PressState::RETURN_UP_SUCCESS:
        return "RETURN_UP_SUCCESS";
    case PressState::PAUSE_RETURN_ABORTED:
        return "PAUSE_RETURN_ABORTED";
    case PressState::PAUSE_RETURN_SUCCESS:
        return "PAUSE_RETURN_SUCCESS";
    case PressState::SAFE_STOP:
        return "SAFE_STOP";
    default:
        return "UNKNOWN";
    }
}

bool isReturnMoveState(PressState state)
{
    return state == PressState::RETURN_UP_SUCCESS || state == PressState::RETURN_UP_ABORTED;
}

bool isReturnPauseState(PressState state)
{
    return state == PressState::PAUSE_RETURN_SUCCESS || state == PressState::PAUSE_RETURN_ABORTED;
}
}

PressController::PressController(
    PressFsm& fsm,
    InputSampler& inputSampler,
    IActuatorDrive& actuatorDrive,
    IDisplay& display,
    CommandQueue& commandQueue,
    StateQueue& stateQueue,
    FillLevelEstimator& fillLevelEstimator)
    : fsm_(fsm)
    , inputSampler_(inputSampler)
    , actuatorDrive_(actuatorDrive)
    , display_(display)
    , commandQueue_(commandQueue)
    , stateQueue_(stateQueue)
    , fillLevelEstimator_(fillLevelEstimator)
{
}

void PressController::controlStep(uint32_t nowMs)
{
    while (true) {
        const std::optional<PressCommand> cmd = commandQueue_.dequeue();
        if (!cmd.has_value()) {
            break;
        }

        handleCommand(*cmd);
    }

    InputSnapshot inputs = inputSampler_.sampleInputs();

    if (startPulseRequested_) {
        inputs.start_pressed = true;
        inputs.start_pressed_rising = true;
        startPulseRequested_ = false;
    }
    inputs.fault_reset_requested = faultResetRequested_;
    faultResetRequested_ = false;

    const PressState oldState = fsm_.getCurrentState();
    const FsmStepResult stepResult = fsm_.step(inputs);

    applyDriveCommand(stepResult.output.driveCommand);
    if (!lastEnqueuedState_.has_value() || *lastEnqueuedState_ != stepResult.state) {
        if (stateQueue_.enqueue(stepResult.state)) {
            lastEnqueuedState_ = stepResult.state;
        }
    }
    if (!lastDisplayedState_.has_value() || *lastDisplayedState_ != stepResult.state) {
        updateDisplay(stepResult.state);
        lastDisplayedState_ = stepResult.state;
    }

    if (stepResult.state != oldState) {
        // Temporary debug trace to verify transition conditions from real inputs.
        ESP_LOGI(
            "PressController",
            "FSM %s -> %s | start=%d startR=%d topA=%d topR=%d bottomA=%d bottomR=%d door=%d estop=%d overA=%d overD=%d",
            toStateString(oldState),
            toStateString(stepResult.state),
            static_cast<int>(inputs.start_pressed),
            static_cast<int>(inputs.start_pressed_rising),
            static_cast<int>(inputs.top_endstop_active),
            static_cast<int>(inputs.top_endstop_reached),
            static_cast<int>(inputs.bottom_endstop_active),
            static_cast<int>(inputs.bottom_endstop_reached),
            static_cast<int>(inputs.door_closed),
            static_cast<int>(inputs.estop),
            static_cast<int>(inputs.over_current_active),
            static_cast<int>(inputs.over_current_detected));
        onStateTransition(oldState, stepResult.state, nowMs);
    }

}

void PressController::applyDriveCommand(DriveCommand cmd)
{
    actuatorDrive_.apply(cmd);
}

void PressController::handleCommand(PressCommand cmd)
{
    switch (cmd) {
    case PressCommand::START:
        startPulseRequested_ = true;
        break;

    case PressCommand::STOP:
        startPulseRequested_ = false;
        applyDriveCommand(DriveCommand::STOP);
        break;

    case PressCommand::RESET:
        startPulseRequested_ = false;
        faultResetRequested_ = true;
        applyDriveCommand(DriveCommand::STOP);
        break;

    default:
        break;
    }
}

void PressController::updateDisplay(PressState state)
{
    display_.write(std::string("State: ") + toStateString(state));
}

void PressController::onStateTransition(PressState oldState, PressState newState, uint32_t nowMs)
{
    updateFillLevelOnTransition(oldState, newState, nowMs);
}

void PressController::updateFillLevelOnTransition(PressState oldState, PressState newState, uint32_t nowMs)
{
    if ((oldState != newState) && isReturnMoveState(newState)) {
        fillLevelEstimator_.startReturnMeasurement(nowMs);
    }

    if ((oldState != newState) && isReturnPauseState(newState)) {
        fillLevelEstimator_.stopReturnMeasurement(nowMs);
        fillLevelEstimator_.estimatePercent();
    }
}

void PressController::estopInterrupt()
{
    applyDriveCommand(DriveCommand::STOP);
}

void PressController::doorOpenedInterrupt()
{
    applyDriveCommand(DriveCommand::STOP);
}
