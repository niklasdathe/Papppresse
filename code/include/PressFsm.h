#pragma once

#include "PapppresseTypes.h"

class PressFsm {
public:
    PressFsm();

    FsmStepResult step(const InputSnapshot& inputs);
    PressState getCurrentState() const;
    void reset();

private:
    PressState currentState_;
};
