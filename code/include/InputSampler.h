#pragma once

#include "IDigitalInput.h"
#include "IOverCurrentDetection.h"
#include "PapppresseTypes.h"

class InputSampler {
public:
    InputSampler(
        IDigitalInput& startButton,
        IDigitalInput& doorDetection,
        IDigitalInput& topEndstop,
        IDigitalInput& bottomEndstop,
        IDigitalInput& estop,
        IOverCurrentDetection& overCurrentDetection);

    InputSnapshot sampleInputs();

private:
    bool initialized_{false};
    bool prevTopEndstopActive_{false};
    bool prevBottomEndstopActive_{false};
    bool prevOverCurrentActive_{false};

    IDigitalInput& startButton_;
    IDigitalInput& doorDetection_;
    IDigitalInput& topEndstop_;
    IDigitalInput& bottomEndstop_;
    IDigitalInput& estop_;
    IOverCurrentDetection& overCurrentDetection_;
};
