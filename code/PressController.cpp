#include "PressController.h"

#include <string>

namespace {
const char* toStateString(PressState state)
{
    switch (state) {
    case PressState::INIT_CHECK:
        return "INIT_CHECK";
    case PressState::INIT_UP:
        return "INIT_UP";
    case PressState::INIT_PAUSE:
        return "INIT_PAUSE";
    case PressState::READY_TOP:
        return "READY_TOP";
    case PressState::PRESS_DOWN:
        return "PRESS_DOWN";
    case PressState::PAUSE_PRESS:
        return "PAUSE_PRESS";
    case PressState::SUCCESS_LATCH:
        return "SUCCESS_LATCH";
    case PressState::RETURN_UP_SUCCESS:
        return "RETURN_UP_SUCCESS";
    case PressState::PAUSE_RETURN_SUCCESS:
        return "PAUSE_RETURN_SUCCESS";
    case PressState::ABORT_LATCH:
        return "ABORT_LATCH";
    case PressState::RETURN_UP_ABORT:
        return "RETURN_UP_ABORT";
    case PressState::PAUSE_RETURN_ABORT:
        return "PAUSE_RETURN_ABORT";
    default:
        return "UNKNOWN";
    }
}

bool isReturnMoveState(PressState state)
{
    return state == PressState::RETURN_UP_SUCCESS || state == PressState::RETURN_UP_ABORT;
}

bool isReturnPauseState(PressState state)
{
    return state == PressState::PAUSE_RETURN_SUCCESS || state == PressState::PAUSE_RETURN_ABORT;
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

    if (startOverride_) {
        inputs.start_pressed = true;
    }

    if (estopLatched_) {
        inputs.estop = true;
    }

    if (doorOpenLatched_) {
        inputs.door_closed = false;
    }

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
        onStateTransition(oldState, stepResult.state, nowMs);
    }

    if (stepResult.state == PressState::READY_TOP) {
        doorOpenLatched_ = false;
        if (!inputs.estop) {
            estopLatched_ = false;
        }
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
        startOverride_ = true;
        break;

    case PressCommand::STOP:
        startOverride_ = false;
        applyDriveCommand(DriveCommand::STOP);
        break;

    case PressCommand::RESET:
        fsm_.reset();
        startOverride_ = false;
        estopLatched_ = false;
        doorOpenLatched_ = false;
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
    estopLatched_ = true;
    applyDriveCommand(DriveCommand::STOP);
}

void PressController::doorOpenedInterrupt()
{
    doorOpenLatched_ = true;
    applyDriveCommand(DriveCommand::STOP);
}
