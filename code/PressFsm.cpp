#include "PressFsm.h"

namespace {
DriveCommand driveForState(PressState state)
{
    switch (state) {
    case PressState::INIT_UP:
    case PressState::RETURN_UP_SUCCESS:
    case PressState::RETURN_UP_ABORT:
        return DriveCommand::MOVE_UP;

    case PressState::PRESS_DOWN:
        return DriveCommand::MOVE_DOWN;

    default:
        return DriveCommand::STOP;
    }
}
}

PressFsm::PressFsm()
{
    reset();
}

FsmStepResult PressFsm::step(const InputSnapshot& inputs)
{
    PressState nextState = currentState_;

    switch (currentState_) {
    case PressState::INIT_CHECK:
        if (inputs.top_es) {
            nextState = PressState::READY_TOP;
        } else if (inputs.door_closed) {
            nextState = PressState::INIT_UP;
        } else {
            nextState = PressState::INIT_PAUSE;
        }
        break;

    case PressState::INIT_UP:
        if (!inputs.door_closed) {
            nextState = PressState::INIT_PAUSE;
        } else if (inputs.top_es) {
            nextState = PressState::READY_TOP;
        }
        break;

    case PressState::INIT_PAUSE:
        if (!inputs.top_es && inputs.door_closed) {
            nextState = PressState::INIT_UP;
        } else if (inputs.top_es) {
            nextState = PressState::READY_TOP;
        }
        break;

    case PressState::READY_TOP:
        if (inputs.door_closed && inputs.start_pressed) {
            nextState = PressState::PRESS_DOWN;
        }
        break;

    case PressState::PRESS_DOWN:
        if (!inputs.door_closed) {
            nextState = PressState::PAUSE_PRESS;
        } else if (inputs.start_pressed) {
            nextState = PressState::ABORT_LATCH;
        } else if (inputs.current_over || inputs.bottom_es) {
            nextState = PressState::SUCCESS_LATCH;
        }
        break;

    case PressState::PAUSE_PRESS:
        if (!inputs.door_closed) {
            nextState = PressState::PAUSE_PRESS;
        } else if (inputs.start_pressed) {
            nextState = PressState::ABORT_LATCH;
        } else {
            nextState = PressState::PRESS_DOWN;
        }
        break;

    case PressState::SUCCESS_LATCH:
        nextState = PressState::RETURN_UP_SUCCESS;
        break;

    case PressState::RETURN_UP_SUCCESS:
        if (!inputs.door_closed) {
            nextState = PressState::PAUSE_RETURN_SUCCESS;
        } else if (inputs.start_pressed) {
            nextState = PressState::PRESS_DOWN;
        }
        break;

    case PressState::PAUSE_RETURN_SUCCESS:
        if (!inputs.door_closed) {
            nextState = PressState::PAUSE_RETURN_SUCCESS;
        } else if (inputs.start_pressed) {
            nextState = PressState::PRESS_DOWN;
        } else {
            nextState = PressState::RETURN_UP_SUCCESS;
        }
        break;

    case PressState::ABORT_LATCH:
        nextState = PressState::RETURN_UP_ABORT;
        break;

    case PressState::RETURN_UP_ABORT:
        if (!inputs.door_closed) {
            nextState = PressState::PAUSE_RETURN_ABORT;
        } else if (inputs.start_pressed) {
            nextState = PressState::PRESS_DOWN;
        } else if (inputs.top_es) {
            nextState = PressState::READY_TOP;
        }
        break;

    case PressState::PAUSE_RETURN_ABORT:
        if (!inputs.door_closed) {
            nextState = PressState::PAUSE_RETURN_ABORT;
        } else if (inputs.start_pressed) {
            nextState = PressState::PRESS_DOWN;
        } else {
            nextState = PressState::RETURN_UP_ABORT;
        }
        break;

    default:
        nextState = PressState::INIT_CHECK;
        break;
    }

    FsmOutput output{};
    output.driveCommand = driveForState(nextState);
    output.successPulse = (currentState_ != nextState) && (nextState == PressState::SUCCESS_LATCH);
    output.abortPulse = (currentState_ != nextState) && (nextState == PressState::ABORT_LATCH);

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
    currentState_ = PressState::INIT_CHECK;
}
