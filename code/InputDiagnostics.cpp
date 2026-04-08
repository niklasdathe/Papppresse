#include "InputDiagnostics.h"

#include "AnalogCurrentSensor.h"
#include "AppConfig.h"
#include "DebouncedDigitalInput.h"
#include "InputSampler.h"
#include "RawDigitalInput.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {
constexpr const char* kDiagTag = "InputDiag";

struct DiagnosticSnapshot {
    bool estopRaw{false};
    bool topEndstopRaw{false};
    bool bottomEndstopRaw{false};
    bool doorClosedRaw{false};
    bool startPulseRaw{false};
    uint16_t currentRaw{0};
    InputSnapshot sampled{};
};

DiagnosticSnapshot takeSnapshot(
    RawDigitalInput& estopRaw,
    RawDigitalInput& topEndstopRaw,
    RawDigitalInput& bottomEndstopRaw,
    RawDigitalInput& doorClosedRaw,
    RawDigitalInput& startPulseRaw,
    AnalogCurrentSensor& currentSensor,
    InputSampler& inputSampler)
{
    DiagnosticSnapshot snapshot{};
    snapshot.estopRaw = estopRaw.read();
    snapshot.topEndstopRaw = topEndstopRaw.read();
    snapshot.bottomEndstopRaw = bottomEndstopRaw.read();
    snapshot.doorClosedRaw = doorClosedRaw.read();
    snapshot.startPulseRaw = startPulseRaw.read();
    snapshot.currentRaw = currentSensor.readRawCurrent();
    snapshot.sampled = inputSampler.sampleInputs();
    return snapshot;
}

bool hasAnyChange(const DiagnosticSnapshot& previous, const DiagnosticSnapshot& current)
{
    return previous.estopRaw != current.estopRaw
        || previous.topEndstopRaw != current.topEndstopRaw
        || previous.bottomEndstopRaw != current.bottomEndstopRaw
        || previous.doorClosedRaw != current.doorClosedRaw
        || previous.startPulseRaw != current.startPulseRaw
        || previous.sampled.start_pressed != current.sampled.start_pressed
        || previous.sampled.top_endstop_active != current.sampled.top_endstop_active
        || previous.sampled.bottom_endstop_active != current.sampled.bottom_endstop_active
        || previous.sampled.door_closed != current.sampled.door_closed
        || previous.sampled.estop != current.sampled.estop
        || previous.sampled.over_current_active != current.sampled.over_current_active
        || previous.sampled.top_endstop_reached != current.sampled.top_endstop_reached
        || previous.sampled.bottom_endstop_reached != current.sampled.bottom_endstop_reached
        || previous.sampled.over_current_detected != current.sampled.over_current_detected;
}

void logSnapshot(const char* reason, const DiagnosticSnapshot& snapshot)
{
    ESP_LOGI(
        kDiagTag,
        "%s | raw estop=%d top=%d bottom=%d door=%d start=%d current=%u | active start=%d topA=%d topR=%d bottomA=%d bottomR=%d door=%d estop=%d overA=%d overD=%d",
        reason,
        static_cast<int>(snapshot.estopRaw),
        static_cast<int>(snapshot.topEndstopRaw),
        static_cast<int>(snapshot.bottomEndstopRaw),
        static_cast<int>(snapshot.doorClosedRaw),
        static_cast<int>(snapshot.startPulseRaw),
        static_cast<unsigned int>(snapshot.currentRaw),
        static_cast<int>(snapshot.sampled.start_pressed),
        static_cast<int>(snapshot.sampled.top_endstop_active),
        static_cast<int>(snapshot.sampled.top_endstop_reached),
        static_cast<int>(snapshot.sampled.bottom_endstop_active),
        static_cast<int>(snapshot.sampled.bottom_endstop_reached),
        static_cast<int>(snapshot.sampled.door_closed),
        static_cast<int>(snapshot.sampled.estop),
        static_cast<int>(snapshot.sampled.over_current_active),
        static_cast<int>(snapshot.sampled.over_current_detected));
}

void logChangedFields(const DiagnosticSnapshot& previous, const DiagnosticSnapshot& current)
{
    if (previous.sampled.start_pressed != current.sampled.start_pressed) {
        ESP_LOGI(kDiagTag, "change start_pressed: %d -> %d", static_cast<int>(previous.sampled.start_pressed), static_cast<int>(current.sampled.start_pressed));
    }
    if (previous.sampled.door_closed != current.sampled.door_closed) {
        ESP_LOGI(kDiagTag, "change door_closed: %d -> %d", static_cast<int>(previous.sampled.door_closed), static_cast<int>(current.sampled.door_closed));
    }
    if (previous.sampled.estop != current.sampled.estop) {
        ESP_LOGI(kDiagTag, "change estop: %d -> %d", static_cast<int>(previous.sampled.estop), static_cast<int>(current.sampled.estop));
    }
    if (previous.sampled.top_endstop_active != current.sampled.top_endstop_active) {
        ESP_LOGI(kDiagTag, "change top_endstop_active: %d -> %d", static_cast<int>(previous.sampled.top_endstop_active), static_cast<int>(current.sampled.top_endstop_active));
    }
    if (previous.sampled.bottom_endstop_active != current.sampled.bottom_endstop_active) {
        ESP_LOGI(kDiagTag, "change bottom_endstop_active: %d -> %d", static_cast<int>(previous.sampled.bottom_endstop_active), static_cast<int>(current.sampled.bottom_endstop_active));
    }
    if (previous.sampled.over_current_active != current.sampled.over_current_active) {
        ESP_LOGI(kDiagTag, "change over_current_active: %d -> %d", static_cast<int>(previous.sampled.over_current_active), static_cast<int>(current.sampled.over_current_active));
    }
    if (current.sampled.top_endstop_reached) {
        ESP_LOGI(kDiagTag, "edge top_endstop_reached: 1 (rising)");
    }
    if (current.sampled.bottom_endstop_reached) {
        ESP_LOGI(kDiagTag, "edge bottom_endstop_reached: 1 (rising)");
    }
    if (current.sampled.over_current_detected) {
        ESP_LOGI(kDiagTag, "edge over_current_detected: 1 (rising)");
    }
}

void validateEdgeConsistency(const DiagnosticSnapshot& previous, const DiagnosticSnapshot& current)
{
    if (current.sampled.top_endstop_reached
        && (!current.sampled.top_endstop_active || previous.sampled.top_endstop_active)) {
        ESP_LOGE(kDiagTag, "top_endstop_reached inconsistent with active transition");
    }

    if (current.sampled.bottom_endstop_reached
        && (!current.sampled.bottom_endstop_active || previous.sampled.bottom_endstop_active)) {
        ESP_LOGE(kDiagTag, "bottom_endstop_reached inconsistent with active transition");
    }

    if (current.sampled.over_current_detected
        && (!current.sampled.over_current_active || previous.sampled.over_current_active)) {
        ESP_LOGE(kDiagTag, "over_current_detected inconsistent with active transition");
    }
}
}

void runInputDiagnostics()
{
    ESP_LOGW(kDiagTag, "Input diagnostics mode active. Production tasks are not started.");
    ESP_LOGI(
        kDiagTag,
        "Pins estop=%u top=%u bottom=%u door=%u start=%u, activeLow estop=%d top=%d bottom=%d door=%d start=%d",
        static_cast<unsigned int>(AppConfig::InputPins::kEStopPin),
        static_cast<unsigned int>(AppConfig::InputPins::kTopEndstopPin),
        static_cast<unsigned int>(AppConfig::InputPins::kBottomEndstopPin),
        static_cast<unsigned int>(AppConfig::InputPins::kDoorClosedPin),
        static_cast<unsigned int>(AppConfig::InputPins::kStartPulsePin),
        static_cast<int>(AppConfig::InputLogic::kEStopActiveLow),
        static_cast<int>(AppConfig::InputLogic::kTopEndstopActiveLow),
        static_cast<int>(AppConfig::InputLogic::kBottomEndstopActiveLow),
        static_cast<int>(AppConfig::InputLogic::kDoorClosedActiveLow),
        static_cast<int>(AppConfig::InputLogic::kStartPulseActiveLow));

    static RawDigitalInput estopRaw(AppConfig::InputPins::kEStopPin);
    static RawDigitalInput topEndstopRaw(AppConfig::InputPins::kTopEndstopPin);
    static RawDigitalInput bottomEndstopRaw(AppConfig::InputPins::kBottomEndstopPin);
    static RawDigitalInput doorClosedRaw(AppConfig::InputPins::kDoorClosedPin);
    static RawDigitalInput startPulseRaw(AppConfig::InputPins::kStartPulsePin);

    static DebouncedDigitalInput estopInput(
        estopRaw,
        AppConfig::InputLogic::kEStopActiveLow,
        AppConfig::Sensor::kDebounceMs);
    static DebouncedDigitalInput topEndstopInput(
        topEndstopRaw,
        AppConfig::InputLogic::kTopEndstopActiveLow,
        AppConfig::Sensor::kDebounceMs);
    static DebouncedDigitalInput bottomEndstopInput(
        bottomEndstopRaw,
        AppConfig::InputLogic::kBottomEndstopActiveLow,
        AppConfig::Sensor::kDebounceMs);
    static DebouncedDigitalInput doorClosedInput(
        doorClosedRaw,
        AppConfig::InputLogic::kDoorClosedActiveLow,
        AppConfig::Sensor::kDebounceMs);
    static DebouncedDigitalInput startButtonInput(
        startPulseRaw,
        AppConfig::InputLogic::kStartPulseActiveLow,
        AppConfig::Sensor::kDebounceMs);

    static AnalogCurrentSensor currentSensor(
        AppConfig::Sensor::kCurrentAdcChannel,
        AppConfig::Sensor::kCurrentThresholdRaw);

    static InputSampler inputSampler(
        startButtonInput,
        doorClosedInput,
        topEndstopInput,
        bottomEndstopInput,
        estopInput,
        currentSensor);

    TickType_t sampleTicks = pdMS_TO_TICKS(AppConfig::Diagnostics::kInputDiagnosticsSamplePeriodMs);
    if (sampleTicks == 0) {
        sampleTicks = 1;
    }

    TickType_t heartbeatTicks = pdMS_TO_TICKS(AppConfig::Diagnostics::kInputDiagnosticsHeartbeatMs);
    if (heartbeatTicks == 0) {
        heartbeatTicks = 1;
    }

    bool hasPrevious = false;
    DiagnosticSnapshot previous{};
    TickType_t lastHeartbeatTick = xTaskGetTickCount();

    while (true) {
        const DiagnosticSnapshot current = takeSnapshot(
            estopRaw,
            topEndstopRaw,
            bottomEndstopRaw,
            doorClosedRaw,
            startPulseRaw,
            currentSensor,
            inputSampler);

        if (!hasPrevious || hasAnyChange(previous, current)) {
            logSnapshot("CHANGE", current);
            if (hasPrevious) {
                logChangedFields(previous, current);
                validateEdgeConsistency(previous, current);
            }
        }

        const TickType_t nowTick = xTaskGetTickCount();
        if ((nowTick - lastHeartbeatTick) >= heartbeatTicks) {
            logSnapshot("HEARTBEAT", current);
            lastHeartbeatTick = nowTick;
        }

        previous = current;
        hasPrevious = true;
        vTaskDelay(sampleTicks);
    }
}
