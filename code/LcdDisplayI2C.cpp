#include "LcdDisplayI2C.h"

#include "esp_log.h"

namespace {
constexpr const char* kTag = "LcdDisplayI2C";
}

LcdDisplayI2C::LcdDisplayI2C(uint8_t address)
    : address_(address)
{
}

void LcdDisplayI2C::write(const std::string& text)
{
    ESP_LOGI(kTag, "I2C[0x%02X] write: %s", address_, text.c_str());
}

void LcdDisplayI2C::clear()
{
    ESP_LOGI(kTag, "I2C[0x%02X] clear", address_);
}
