#pragma once

#include <cstdint>

enum class DriveCommand : uint8_t {
    STOP = 0,
    MOVE_UP,
    MOVE_DOWN,
};

enum class PressState : uint8_t {
    INIT_CHECK = 0,
    INIT_UP,
    INIT_PAUSE,
    READY_TOP,
    PRESS_DOWN,
    PAUSE_PRESS,
    SUCCESS_LATCH,
    RETURN_UP_SUCCESS,
    PAUSE_RETURN_SUCCESS,
    ABORT_LATCH,
    RETURN_UP_ABORT,
    PAUSE_RETURN_ABORT,
};

enum class PressCommand : uint8_t {
    START = 0,
    STOP,
    RESET,
};

struct InputSnapshot {
    bool start_pressed{false};
    bool top_es{false};
    bool bottom_es{false};
    bool door_closed{false};
    bool estop{false};
    bool current_over{false};
};

struct FsmOutput {
    DriveCommand driveCommand{DriveCommand::STOP};
    bool successPulse{false};
    bool abortPulse{false};
};

struct FsmStepResult {
    PressState state{PressState::INIT_CHECK};
    FsmOutput output{};
};
