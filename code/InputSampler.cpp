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
    InputSnapshot snapshot{};
    snapshot.start_pressed = startButton_.read();
    snapshot.top_es = topEndstop_.read();
    snapshot.bottom_es = bottomEndstop_.read();
    snapshot.door_closed = doorDetection_.read();
    snapshot.estop = estop_.read();
    snapshot.current_over = overCurrentDetection_.read();
    return snapshot;
}
