#pragma once

#include <cstdint>

#include "esp_adc/adc_oneshot.h"

#include "IOverCurrentDetection.h"

class AnalogCurrentSensor : public IOverCurrentDetection {
public:
    AnalogCurrentSensor(adc_channel_t adcChannel, uint16_t thresholdRaw);

    uint16_t readRawCurrent() const;
    bool read() override;

    void setThresholdRaw(uint16_t thresholdRaw);
    uint16_t getThresholdRaw() const;

private:
    adc_oneshot_unit_handle_t adcHandle_;
    adc_channel_t adcChannel_;
    uint16_t thresholdRaw_;
};
