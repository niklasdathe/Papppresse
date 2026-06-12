#pragma once

#include <cstdint>

#include "hal/adc_types.h"

namespace AppConfig {
constexpr const char* kLogTag = "Papppresse";

namespace ActuatorPins {
// RPWM lives on GPIO45 (not GPIO1) so that GPIO1 (ADC1_CH0) is free for the
// L_IS current-sense input — GPIO1 is the only exposed ADC-capable pin on this
// board. GPIO45 is a VDD_SPI strapping pin; it is only driven after boot, and
// RPWM idles low (= the default strap level), so boot is unaffected.
constexpr uint8_t kRpwmpin = 45;
constexpr uint8_t kLpwmpin = 47;
constexpr uint8_t kRenablePin = 38;
constexpr uint8_t kLenablePin = 48;
}

namespace InputPins {
constexpr uint8_t kEStopPin = 11; // DI8
constexpr uint8_t kTopEndstopPin = 10; //DI7
constexpr uint8_t kBottomEndstopPin = 9; //DI6
constexpr uint8_t kDoorClosedPin = 8; //DI5
constexpr uint8_t kStartPulsePin = 7; //DI4
}

namespace InputLogic {
// E-stop is currently handled in hardware/wiring only. The software input pin is
// unused and left floating, which would otherwise trip SAFE_STOP erratically, so
// the state machine ignores it. Set true to re-enable software e-stop handling
// once the pin is properly wired/pulled.
constexpr bool kEStopEnabled = false;

// Active-low means electrical low on the pin equals logical active.
constexpr bool kEStopActiveLow = true;
constexpr bool kTopEndstopActiveLow = false;
constexpr bool kBottomEndstopActiveLow = false;
constexpr bool kDoorClosedActiveLow = true;
constexpr bool kStartPulseActiveLow = true;
}


namespace Sensor {
constexpr uint32_t kDebounceMs = 30;

// --- Over-current detection (IS sense voltage on the ADC) ----------------------
// During the down stroke the press leaves PRESS_DOWN and returns up once the ADC
// voltage on the IS sense line reaches this threshold. The ADC reads 0 V (no load) up to
// 3.3 V (full scale). Set this trip point directly in volts, calibrated by probing
// the sense voltage with a multimeter at the pressing force you want. Raise it to
// press harder, lower it to return earlier.
constexpr float kOverCurrentTripVolts = 1.2f;

// Motor inrush at the start of the down stroke briefly raises the sense voltage,
// which would otherwise abort the press on the very first sample. Ignore
// over-current for this long after the press (re)enters PRESS_DOWN. The cardboard
// is not yet compressed during this window, so suppressing protection here is
// safe; raise it if startup still trips, lower it to react to real stalls sooner.
constexpr uint32_t kOverCurrentStartupBlankingMs = 150;

// ESP32-S3 ADC1: 12-bit, 12 dB attenuation (~0..3.3 V usable full scale).
constexpr uint16_t kAdcMaxCount = 4095;
constexpr float kAdcFullScaleMilliVolts = 3300.0f;

// Converts an ADC trip voltage into a raw ADC count, clamped to the ADC range.
constexpr uint16_t voltsToRaw(float volts)
{
    const float raw = volts * 1000.0f * static_cast<float>(kAdcMaxCount) / kAdcFullScaleMilliVolts;
    if (raw <= 0.0f) {
        return 0;
    }
    if (raw >= static_cast<float>(kAdcMaxCount)) {
        return kAdcMaxCount;
    }
    return static_cast<uint16_t>(raw);
}

constexpr uint16_t kCurrentThresholdRaw = voltsToRaw(kOverCurrentTripVolts);

// ADC1 channels wired to the BTS7960 current-sense pins.
//   ADC1 channel -> GPIO map (ESP32-S3): CH0=GPIO1 .. CH9=GPIO10.
// This board only exposes one ADC-capable pin (GPIO1 = ADC1_CH0), so only the
// down-stroke sense line L_IS is wired (RPWM was moved off GPIO1 to free it).
// R_IS is not sensed; both constants point at the same channel, so the sensor's
// max-of-both reduces to L_IS. Wire L_IS -> GPIO1.
constexpr adc_channel_t kLIsAdcChannel = ADC_CHANNEL_0; // GPIO1 (L_IS, down stroke)
constexpr adc_channel_t kRIsAdcChannel = ADC_CHANNEL_0; // R_IS not wired (no 2nd ADC pin)
}

namespace Display {
constexpr uint8_t kI2cAddress = 0x27;
}

namespace Connectivity {
constexpr const char* kBroker = "127.0.0.1";
constexpr uint16_t kPort = 1883;
constexpr const char* kUser = "";
constexpr const char* kPassword = "";
}

namespace Runtime {
constexpr uint32_t kCommandQueueLength = 8;
constexpr uint32_t kCommandQueueDefaultLength = kCommandQueueLength;
constexpr uint32_t kStateQueueLength = 16;
constexpr uint32_t kStateQueueDefaultLength = 8;
constexpr uint32_t kQueueNoWaitTicks = 0;
constexpr uint32_t kControlPeriodMs = 20;
constexpr uint32_t kMqttPeriodMs = 50;
constexpr uint32_t kControlTaskStackSize = 4096;
constexpr uint32_t kMqttTaskStackSize = 4096;
constexpr const char* kControlTaskName = "control_task";
constexpr const char* kMqttTaskName = "mqtt_task";
constexpr int kControlPriority = 10;
constexpr int kMqttPriority = 5;
constexpr int kControlCore = 0;
constexpr int kMqttCore = 1;
}

namespace Diagnostics {
// Run only the input diagnostic loop instead of the full runtime.
constexpr bool kEnableInputDiagnosticsOnly = false;
// Sampling period for the diagnostic snapshot loop.
constexpr uint32_t kInputDiagnosticsSamplePeriodMs = Runtime::kControlPeriodMs;
// Full heartbeat period; change-triggered logs are emitted immediately.
constexpr uint32_t kInputDiagnosticsHeartbeatMs = 1000;
}

namespace FillLevel {
constexpr uint32_t kNotCalibratedPercent = 0;
constexpr uint32_t kLowCalibrationPercent = 20;
constexpr uint32_t kFullPercent = 100;
}
}