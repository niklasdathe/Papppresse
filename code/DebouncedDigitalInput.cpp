#include "DebouncedDigitalInput.h"

#include "esp_timer.h"

namespace {
uint32_t nowMs()
{
    return static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
}
}

DebouncedDigitalInput::DebouncedDigitalInput(IDigitalInput& source, bool activeLow, uint32_t debounceTimeMs)
    : source_(source)
    , activeLow_(activeLow)
    , debounceTimeMs_(debounceTimeMs)
    , initialized_(false)
    , stableState_(false)
    , candidateState_(false)
    , lastCandidateChangeMs_(0)
{
}

bool DebouncedDigitalInput::read()
{
    bool value = source_.read();
    if (activeLow_) {
        value = !value;
    }

    const uint32_t now = nowMs();

    if (!initialized_) {
        initialized_ = true;
        stableState_ = value;
        candidateState_ = value;
        lastCandidateChangeMs_ = now;
        return stableState_;
    }

    if (value != candidateState_) {
        candidateState_ = value;
        lastCandidateChangeMs_ = now;
    }

    if ((candidateState_ != stableState_) && ((now - lastCandidateChangeMs_) >= debounceTimeMs_)) {
        stableState_ = candidateState_;
    }

    return stableState_;
}
