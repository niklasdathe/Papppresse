#pragma once

#include <optional>

#include "PapppresseTypes.h"

class IConnectivity {
public:
    virtual ~IConnectivity() = default;
    virtual void publishState(PressState state) = 0;
    virtual std::optional<PressCommand> pollCommand() = 0;
    virtual bool isConnected() const = 0;
};
