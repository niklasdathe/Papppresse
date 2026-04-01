#include "AnalogCurrentSensor.h"

AnalogCurrentSensor::AnalogCurrentSensor(adc_channel_t adcChannel, uint16_t thresholdRaw)
    : adcHandle_(nullptr)
    , adcChannel_(adcChannel)
    , thresholdRaw_(thresholdRaw)
{
    adc_oneshot_unit_init_cfg_t unitConfig = {};
    unitConfig.unit_id = ADC_UNIT_1;

    if (adc_oneshot_new_unit(&unitConfig, &adcHandle_) != ESP_OK) {
        adcHandle_ = nullptr;
        return;
    }

    adc_oneshot_chan_cfg_t channelConfig = {};
    channelConfig.atten = ADC_ATTEN_DB_12;
    channelConfig.bitwidth = ADC_BITWIDTH_12;

    if (adc_oneshot_config_channel(adcHandle_, adcChannel_, &channelConfig) != ESP_OK) {
        (void)adc_oneshot_del_unit(adcHandle_);
        adcHandle_ = nullptr;
    }
}

uint16_t AnalogCurrentSensor::readRawCurrent() const
{
    if (adcHandle_ == nullptr) {
        return 0;
    }

    int value = 0;
    if (adc_oneshot_read(adcHandle_, adcChannel_, &value) != ESP_OK) {
        return 0;
    }

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
