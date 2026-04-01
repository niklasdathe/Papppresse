#pragma once

class IOverCurrentDetection {
public:
    virtual ~IOverCurrentDetection() = default;
    virtual bool read() = 0;
};
