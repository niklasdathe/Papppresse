#ifndef DEBOUNCED_DIGITAL_INPUT_H
#define DEBOUNCED_DIGITAL_INPUT_H

#include <cstdint>
#include <Arduino.h>
#include "IDigitalInput.h"

class DebouncedDigitalInput : public IDigitalInput {
public:
    DebouncedDigitalInput(IDigitalInput& source, bool activeLow, uint32_t debounceTimeMs);

    bool read() override;

private:
    IDigitalInput& source_;
    bool activeLow_;
    uint32_t debounceTimeMs_;

    bool initialized_;
    bool stableState_;
    bool candidateState_;
    uint32_t lastCandidateChangeMs_;
};
#endif