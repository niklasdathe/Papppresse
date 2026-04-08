#pragma once

#include <cstdint>

enum class DriveCommand : uint8_t {
    STOP = 0,
    MOVE_UP,
    MOVE_DOWN,
};

enum class PressState : uint8_t {
    INIT_PAUSE = 0,
    INIT_UP,
    READY_TOP,
    PRESS_DOWN,
    PAUSE_PRESS,
    RETURN_UP_ABORTED,
    RETURN_UP_SUCCESS,
    PAUSE_RETURN_ABORTED,
    PAUSE_RETURN_SUCCESS,
    SAFE_STOP,
};

enum class PressCommand : uint8_t {
    START = 0,
    STOP,
    RESET,
};

struct InputSnapshot {
    bool start_pressed{false};
    bool top_endstop_active{false};
    bool top_endstop_reached{false};
    bool bottom_endstop_active{false};
    bool bottom_endstop_reached{false};
    bool door_closed{false};
    bool estop{false};
    bool over_current_active{false};
    bool over_current_detected{false};
    bool fault_reset_requested{false};
};

struct FsmOutput {
    DriveCommand driveCommand{DriveCommand::STOP};
    bool successPulse{false};
    bool abortPulse{false};
};

struct FsmStepResult {
    PressState state{PressState::INIT_PAUSE};
    FsmOutput output{};
};
