#include "includes/DebouncedDigitalInput.h"

DebouncedDigitalInput::DebouncedDigitalInput(IDigitalInput& source,
                                             bool activeLow,
                                             uint32_t debounceTimeMs)
    : source_(source),
      activeLow_(activeLow),
      debounceTimeMs_(debounceTimeMs),
      initialized_(false),
      stableState_(false),
      candidateState_(false),
      lastCandidateChangeMs_(0)
{
}

bool DebouncedDigitalInput::read()
{
    bool rawState = source_.read();

    if (activeLow_) {
        rawState = !rawState;
    }

    const uint32_t now = millis();

    if (!initialized_) {
        stableState_ = rawState;
        candidateState_ = rawState;
        lastCandidateChangeMs_ = now;
        initialized_ = true;
        return stableState_;
    }

    if (rawState != candidateState_) {
        candidateState_ = rawState;
        lastCandidateChangeMs_ = now;
    }

    if ((candidateState_ != stableState_) &&
        ((now - lastCandidateChangeMs_) >= debounceTimeMs_)) {
        stableState_ = candidateState_;
    }

    return stableState_;
}