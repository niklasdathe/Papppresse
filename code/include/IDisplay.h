#pragma once

#include <string>

class IDisplay {
public:
    virtual ~IDisplay() = default;
    virtual void write(const std::string& text) = 0;
    virtual void clear() = 0;
};
