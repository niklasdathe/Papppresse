#pragma once

#include <cstdint>
#include <string>

#include "IDisplay.h"

class LcdDisplayI2C : public IDisplay {
public:
    explicit LcdDisplayI2C(uint8_t address);

    void write(const std::string& text) override;
    void clear() override;

private:
    uint8_t address_;
};
