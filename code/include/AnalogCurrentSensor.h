#pragma once

#include <cstdint>

#include "esp_adc/adc_oneshot.h"

#include "IOverCurrentDetection.h"

// Reads the BTS7960 current-sense feedback (R_IS / L_IS) on two ADC1 channels.
//
// The H-bridge exposes one IS pin per half (R_IS for the RPWM-side chip, L_IS
// for the LPWM-side chip). Only the chip whose high-side switch is active drives
// a sense current; the idle side reads ~0. Because the down stroke runs on LPWM
// (see Bts7960ActuatorDrive::MOVE_DOWN), the relevant pin there is L_IS, while the
// up stroke loads R_IS. Taking the maximum of both channels yields the active
// load current regardless of direction, so a single threshold works everywhere.
//
// On any ADC error the affected channel reads 0 (fail-safe: no false over-current
// trip; the stroke still terminates on the bottom endstop).
class AnalogCurrentSensor : public IOverCurrentDetection {
public:
    static constexpr uint8_t kChannelCount = 2;

    AnalogCurrentSensor(adc_channel_t rIsChannel, adc_channel_t lIsChannel, uint16_t thresholdRaw);
    // Convenience overload for setups where only a single IS line is wired.
    AnalogCurrentSensor(adc_channel_t isChannel, uint16_t thresholdRaw);

    // Raw ADC count of a single sense channel (0 == R_IS, 1 == L_IS). For diagnostics.
    uint16_t readChannelRaw(uint8_t index) const;
    // Highest raw count across both sense channels.
    uint16_t readRawCurrent() const;
    bool read() override;

    void setThresholdRaw(uint16_t thresholdRaw);
    uint16_t getThresholdRaw() const;

private:
    void configureChannel(adc_channel_t channel);

    adc_oneshot_unit_handle_t adcHandle_;
    adc_channel_t channels_[kChannelCount];
    uint16_t thresholdRaw_;
};
