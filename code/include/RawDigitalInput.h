#pragma once

#include <cstdint>

#include "driver/gpio.h"

#include "IDigitalInput.h"

class RawDigitalInput : public IDigitalInput {
public:
    explicit RawDigitalInput(uint8_t pin);

    bool read() override;

private:
    gpio_num_t pin_;
};
