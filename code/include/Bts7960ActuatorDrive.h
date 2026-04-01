#pragma once

#include <cstdint>

#include "driver/gpio.h"

#include "IActuatorDrive.h"

class Bts7960ActuatorDrive : public IActuatorDrive {
public:
    Bts7960ActuatorDrive(uint8_t rPwmPin, uint8_t lPwmPin, uint8_t rEnPin, uint8_t lEnPin);

    bool apply(DriveCommand cmd) override;

private:
    gpio_num_t rPwmPin_;
    gpio_num_t lPwmPin_;
    gpio_num_t rEnPin_;
    gpio_num_t lEnPin_;
};
