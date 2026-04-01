#pragma once

#include <cstdint>

#include "driver/adc.h"

namespace AppConfig {
constexpr const char* kLogTag = "Papppresse";

namespace ActuatorPins {
constexpr uint8_t kRpwmpin = 1;
constexpr uint8_t kLpwmpin = 47;
constexpr uint8_t kRenablePin = 38;
constexpr uint8_t kLenablePin = 48;
}

namespace InputPins {
constexpr uint8_t kEStopPin = 4;
constexpr uint8_t kTopEndstopPin = 5;
constexpr uint8_t kBottomEndstopPin = 6;
constexpr uint8_t kDoorClosedPin = 7;
constexpr uint8_t kStartPulsePin = 8;
}

namespace Sensor {
constexpr uint32_t kDebounceMs = 30;
constexpr uint16_t kCurrentThresholdRaw = 3000;
constexpr uint8_t kCurrentAdcChannel = ADC1_CHANNEL_0;
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

namespace FillLevel {
constexpr uint32_t kNotCalibratedPercent = 0;
constexpr uint32_t kLowCalibrationPercent = 20;
constexpr uint32_t kFullPercent = 100;
}
}