#include "RawDigitalInput.h"

RawDigitalInput::RawDigitalInput(uint8_t pin)
    : pin_(static_cast<gpio_num_t>(pin))
{
    gpio_config_t config{};
    config.pin_bit_mask = (1ULL << pin_);
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&config);
}

bool RawDigitalInput::read()
{
    return gpio_get_level(pin_) != 0;
}
