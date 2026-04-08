#pragma once

#include "PapppresseTypes.h"

class PressFsm {
public:
    PressFsm();

    FsmStepResult step(const InputSnapshot& inputs);
    PressState getCurrentState() const;
    void reset();

private:
    bool needsOperationalEntry_{true};
    PressState currentState_;
};
