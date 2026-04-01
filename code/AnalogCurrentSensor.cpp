#include "AnalogCurrentSensor.h"

AnalogCurrentSensor::AnalogCurrentSensor(uint8_t adcChannel, uint16_t thresholdRaw)
    : adcChannel_(static_cast<adc1_channel_t>(adcChannel))
    , thresholdRaw_(thresholdRaw)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(adcChannel_, ADC_ATTEN_DB_11);
}

uint16_t AnalogCurrentSensor::readRawCurrent() const
{
    const int value = adc1_get_raw(adcChannel_);
    return static_cast<uint16_t>(value < 0 ? 0 : value);
}

bool AnalogCurrentSensor::read()
{
    return readRawCurrent() >= thresholdRaw_;
}

void AnalogCurrentSensor::setThresholdRaw(uint16_t thresholdRaw)
{
    thresholdRaw_ = thresholdRaw;
}

uint16_t AnalogCurrentSensor::getThresholdRaw() const
{
    return thresholdRaw_;
}
