#include "PressFsm.h"

namespace {
DriveCommand driveForState(PressState state)
{
    switch (state) {
    case PressState::INIT_UP:
    case PressState::RETURN_UP_SUCCESS:
    case PressState::RETURN_UP_ABORTED:
        return DriveCommand::MOVE_UP;

    case PressState::PRESS_DOWN:
        return DriveCommand::MOVE_DOWN;

    default:
        return DriveCommand::STOP;
    }
}

bool triggerHardSafetyFault(const InputSnapshot& inputs)
{
    return inputs.estop;
}

bool triggerFaultReset(const InputSnapshot& inputs)
{
    return inputs.fault_reset_requested;
}

PressState resolveOperationalEntry(const InputSnapshot& inputs)
{
    if (!inputs.door_closed) {
        return PressState::INIT_PAUSE;
    }

    if (!inputs.top_endstop_active) {
        return PressState::INIT_UP;
    }

    return PressState::READY_TOP;
}

bool guardPressCompletion(const InputSnapshot& inputs)
{
    return inputs.bottom_endstop_active || inputs.over_current_active;
}

bool triggerPressCompletion(const InputSnapshot& inputs)
{
    return inputs.bottom_endstop_reached || inputs.over_current_detected;
}
}

PressFsm::PressFsm()
{
    reset();
}

FsmStepResult PressFsm::step(const InputSnapshot& inputs)
{
    const PressState oldState = currentState_;
    PressState nextState = currentState_;
    bool successPulse = false;
    bool abortPulse = false;

    if (oldState == PressState::SAFE_STOP) {
        if (triggerFaultReset(inputs) && !triggerHardSafetyFault(inputs)) {
            nextState = resolveOperationalEntry(inputs);
            needsOperationalEntry_ = false;
        }
    } else if (triggerHardSafetyFault(inputs)) {
        nextState = PressState::SAFE_STOP;
        needsOperationalEntry_ = true;
    } else if (needsOperationalEntry_) {
        nextState = resolveOperationalEntry(inputs);
        needsOperationalEntry_ = false;
    } else {
        switch (oldState) {
        case PressState::INIT_PAUSE:
            if (inputs.door_closed) {
                if (inputs.top_endstop_active) {
                    nextState = PressState::READY_TOP;
                } else {
                    nextState = PressState::INIT_UP;
                }
            }
            break;

        case PressState::INIT_UP:
            if (!inputs.door_closed) {
                nextState = PressState::INIT_PAUSE;
            } else if (inputs.top_endstop_reached) {
                nextState = PressState::READY_TOP;
            }
            break;

        case PressState::READY_TOP:
            if (inputs.door_closed && inputs.start_pressed_rising) {
                nextState = PressState::PRESS_DOWN;
            }
            break;

        case PressState::PRESS_DOWN:
            if (!inputs.door_closed) {
                nextState = PressState::PAUSE_PRESS;
            } else if (triggerPressCompletion(inputs)) {
                nextState = PressState::RETURN_UP_SUCCESS;
                successPulse = true;
            } else if (inputs.start_pressed_rising) {
                nextState = PressState::RETURN_UP_ABORTED;
                abortPulse = true;
            }
            break;

        case PressState::PAUSE_PRESS:
            if (inputs.door_closed) {
                if (guardPressCompletion(inputs)) {
                    nextState = PressState::RETURN_UP_SUCCESS;
                    successPulse = true;
                } else {
                    nextState = PressState::PRESS_DOWN;
                }
            }
            break;

        case PressState::RETURN_UP_ABORTED:
            if (!inputs.door_closed) {
                nextState = PressState::PAUSE_RETURN_ABORTED;
            } else if (inputs.top_endstop_reached) {
                nextState = PressState::READY_TOP;
            }
            break;

        case PressState::PAUSE_RETURN_ABORTED:
            if (inputs.door_closed) {
                if (inputs.top_endstop_active) {
                    nextState = PressState::READY_TOP;
                } else {
                    nextState = PressState::RETURN_UP_ABORTED;
                }
            }
            break;

        case PressState::RETURN_UP_SUCCESS:
            if (!inputs.door_closed) {
                nextState = PressState::PAUSE_RETURN_SUCCESS;
            } else if (inputs.top_endstop_reached) {
                nextState = PressState::READY_TOP;
            }
            break;

        case PressState::PAUSE_RETURN_SUCCESS:
            if (inputs.door_closed) {
                if (inputs.top_endstop_active) {
                    nextState = PressState::READY_TOP;
                } else {
                    nextState = PressState::RETURN_UP_SUCCESS;
                }
            }
            break;

        case PressState::SAFE_STOP:
            break;

        default:
            nextState = resolveOperationalEntry(inputs);
            break;
        }
    }

    FsmOutput output{};
    output.driveCommand = driveForState(nextState);
    output.successPulse = (oldState != nextState) && successPulse;
    output.abortPulse = (oldState != nextState) && abortPulse;

    currentState_ = nextState;

    FsmStepResult result{};
    result.state = currentState_;
    result.output = output;
    return result;
}

PressState PressFsm::getCurrentState() const
{
    return currentState_;
}

void PressFsm::reset()
{
    currentState_ = PressState::INIT_PAUSE;
    needsOperationalEntry_ = true;
}
