#pragma once

class IDigitalInput {
public:
    virtual ~IDigitalInput() = default;
    virtual bool read() = 0;
};
