#include "InputSampler.h"

InputSampler::InputSampler(
    IDigitalInput& startButton,
    IDigitalInput& doorDetection,
    IDigitalInput& topEndstop,
    IDigitalInput& bottomEndstop,
    IDigitalInput& estop,
    IOverCurrentDetection& overCurrentDetection)
    : startButton_(startButton)
    , doorDetection_(doorDetection)
    , topEndstop_(topEndstop)
    , bottomEndstop_(bottomEndstop)
    , estop_(estop)
    , overCurrentDetection_(overCurrentDetection)
{
}

InputSnapshot InputSampler::sampleInputs()
{
    const bool startPressed = startButton_.read();
    const bool topEndstopActive = topEndstop_.read();
    const bool bottomEndstopActive = bottomEndstop_.read();
    const bool overCurrentActive = overCurrentDetection_.read();

    InputSnapshot snapshot{};
    snapshot.start_pressed = startPressed;
    snapshot.top_endstop_active = topEndstopActive;
    snapshot.bottom_endstop_active = bottomEndstopActive;
    snapshot.door_closed = doorDetection_.read();
    snapshot.estop = estop_.read();
    snapshot.over_current_active = overCurrentActive;

    if (initialized_) {
        snapshot.start_pressed_rising = startPressed && !prevStartPressed_;
        snapshot.top_endstop_reached = topEndstopActive && !prevTopEndstopActive_;
        snapshot.bottom_endstop_reached = bottomEndstopActive && !prevBottomEndstopActive_;
        snapshot.over_current_detected = overCurrentActive && !prevOverCurrentActive_;
    }

    prevStartPressed_ = startPressed;
    prevTopEndstopActive_ = topEndstopActive;
    prevBottomEndstopActive_ = bottomEndstopActive;
    prevOverCurrentActive_ = overCurrentActive;
    initialized_ = true;

    return snapshot;
}
