#pragma once

#include <cstdint>

#include "driver/adc.h"

#include "IOverCurrentDetection.h"

class AnalogCurrentSensor : public IOverCurrentDetection {
public:
    AnalogCurrentSensor(uint8_t adcChannel, uint16_t thresholdRaw);

    uint16_t readRawCurrent() const;
    bool read() override;

    void setThresholdRaw(uint16_t thresholdRaw);
    uint16_t getThresholdRaw() const;

private:
    adc1_channel_t adcChannel_;
    uint16_t thresholdRaw_;
};
