#pragma once

#include "PapppresseTypes.h"

class IActuatorDrive {
public:
    virtual ~IActuatorDrive() = default;
    virtual bool apply(DriveCommand cmd) = 0;
};
