#include "AnalogCurrentSensor.h"

AnalogCurrentSensor::AnalogCurrentSensor(adc_channel_t rIsChannel, adc_channel_t lIsChannel, uint16_t thresholdRaw)
    : adcHandle_(nullptr)
    , channels_{rIsChannel, lIsChannel}
    , thresholdRaw_(thresholdRaw)
{
    adc_oneshot_unit_init_cfg_t unitConfig = {};
    unitConfig.unit_id = ADC_UNIT_1;

    if (adc_oneshot_new_unit(&unitConfig, &adcHandle_) != ESP_OK) {
        adcHandle_ = nullptr;
        return;
    }

    for (uint8_t i = 0; i < kChannelCount; ++i) {
        configureChannel(channels_[i]);
    }
}

AnalogCurrentSensor::AnalogCurrentSensor(adc_channel_t isChannel, uint16_t thresholdRaw)
    : AnalogCurrentSensor(isChannel, isChannel, thresholdRaw)
{
}

void AnalogCurrentSensor::configureChannel(adc_channel_t channel)
{
    if (adcHandle_ == nullptr) {
        return;
    }

    adc_oneshot_chan_cfg_t channelConfig = {};
    channelConfig.atten = ADC_ATTEN_DB_12;
    channelConfig.bitwidth = ADC_BITWIDTH_12;

    // A channel that fails to configure simply reads 0 (fail-safe); the other
    // channel and the rest of the unit stay usable.
    (void)adc_oneshot_config_channel(adcHandle_, channel, &channelConfig);
}

uint16_t AnalogCurrentSensor::readChannelRaw(uint8_t index) const
{
    if (adcHandle_ == nullptr || index >= kChannelCount) {
        return 0;
    }

    int value = 0;
    if (adc_oneshot_read(adcHandle_, channels_[index], &value) != ESP_OK) {
        return 0;
    }

    return static_cast<uint16_t>(value < 0 ? 0 : value);
}

uint16_t AnalogCurrentSensor::readRawCurrent() const
{
    uint16_t maxRaw = 0;
    for (uint8_t i = 0; i < kChannelCount; ++i) {
        const uint16_t raw = readChannelRaw(i);
        if (raw > maxRaw) {
            maxRaw = raw;
        }
    }
    return maxRaw;
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
