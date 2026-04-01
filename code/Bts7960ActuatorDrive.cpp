#include "Bts7960ActuatorDrive.h"

Bts7960ActuatorDrive::Bts7960ActuatorDrive(uint8_t rPwmPin, uint8_t lPwmPin, uint8_t rEnPin, uint8_t lEnPin)
    : rPwmPin_(static_cast<gpio_num_t>(rPwmPin))
    , lPwmPin_(static_cast<gpio_num_t>(lPwmPin))
    , rEnPin_(static_cast<gpio_num_t>(rEnPin))
    , lEnPin_(static_cast<gpio_num_t>(lEnPin))
{
    const uint64_t pinMask = (1ULL << rPwmPin_) | (1ULL << lPwmPin_) | (1ULL << rEnPin_) | (1ULL << lEnPin_);

    gpio_config_t config{};
    config.pin_bit_mask = pinMask;
    config.mode = GPIO_MODE_OUTPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&config);

    apply(DriveCommand::STOP);
}

bool Bts7960ActuatorDrive::apply(DriveCommand cmd)
{
    gpio_set_level(rEnPin_, 1);
    gpio_set_level(lEnPin_, 1);

    switch (cmd) {
    case DriveCommand::STOP:
        gpio_set_level(rPwmPin_, 0);
        gpio_set_level(lPwmPin_, 0);
        return true;

    case DriveCommand::MOVE_UP:
        gpio_set_level(rPwmPin_, 1);
        gpio_set_level(lPwmPin_, 0);
        return true;

    case DriveCommand::MOVE_DOWN:
        gpio_set_level(rPwmPin_, 0);
        gpio_set_level(lPwmPin_, 1);
        return true;

    default:
        gpio_set_level(rPwmPin_, 0);
        gpio_set_level(lPwmPin_, 0);
        return false;
    }
}
