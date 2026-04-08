#include "AnalogCurrentSensor.h"
#include "AppConfig.h"
#include "Bts7960ActuatorDrive.h"
#include "CommandQueue.h"
#include "ControlTask.h"
#include "DebouncedDigitalInput.h"
#include "FillLevelEstimator.h"
#include "InputSampler.h"
#include "InputDiagnostics.h"
#include "LcdDisplayI2C.h"
#include "MqttClient.h"
#include "MqttTask.h"
#include "PressController.h"
#include "PressFsm.h"
#include "RawDigitalInput.h"
#include "StateQueue.h"

#include "esp_log.h"

extern "C" void app_main(void)
{
    ESP_LOGI(AppConfig::kLogTag, "Starting ESP-IDF Papppresse runtime");

    if (AppConfig::Diagnostics::kEnableInputDiagnosticsOnly) {
        runInputDiagnostics();
        return;
    }

    static RawDigitalInput estopRaw(AppConfig::InputPins::kEStopPin);
    static RawDigitalInput topEndstopRaw(AppConfig::InputPins::kTopEndstopPin);
    static RawDigitalInput bottomEndstopRaw(AppConfig::InputPins::kBottomEndstopPin);
    static RawDigitalInput doorClosedRaw(AppConfig::InputPins::kDoorClosedPin);
    static RawDigitalInput startPulseRaw(AppConfig::InputPins::kStartPulsePin);

    static DebouncedDigitalInput estopInput(estopRaw, AppConfig::InputLogic::kEStopActiveLow, AppConfig::Sensor::kDebounceMs);
    static DebouncedDigitalInput topEndstopInput(topEndstopRaw, AppConfig::InputLogic::kTopEndstopActiveLow, AppConfig::Sensor::kDebounceMs);
    static DebouncedDigitalInput bottomEndstopInput(bottomEndstopRaw, AppConfig::InputLogic::kBottomEndstopActiveLow, AppConfig::Sensor::kDebounceMs);
    static DebouncedDigitalInput doorClosedInput(doorClosedRaw, AppConfig::InputLogic::kDoorClosedActiveLow, AppConfig::Sensor::kDebounceMs);
    static DebouncedDigitalInput startButtonInput(startPulseRaw, AppConfig::InputLogic::kStartPulseActiveLow, AppConfig::Sensor::kDebounceMs);

    static AnalogCurrentSensor currentSensor(AppConfig::Sensor::kCurrentAdcChannel, AppConfig::Sensor::kCurrentThresholdRaw);
    static Bts7960ActuatorDrive actuatorDrive(
        AppConfig::ActuatorPins::kRpwmpin,
        AppConfig::ActuatorPins::kLpwmpin,
        AppConfig::ActuatorPins::kRenablePin,
        AppConfig::ActuatorPins::kLenablePin);
    static LcdDisplayI2C display(AppConfig::Display::kI2cAddress);
    static MqttClient connectivity(
        AppConfig::Connectivity::kBroker,
        AppConfig::Connectivity::kPort,
        AppConfig::Connectivity::kUser,
        AppConfig::Connectivity::kPassword);

    static InputSampler inputSampler(
        startButtonInput,
        doorClosedInput,
        topEndstopInput,
        bottomEndstopInput,
        estopInput,
        currentSensor);

    static PressFsm fsm;
    static CommandQueue commandQueue(AppConfig::Runtime::kCommandQueueLength);
    static StateQueue stateQueue(AppConfig::Runtime::kStateQueueLength);
    static FillLevelEstimator fillLevelEstimator;

    static PressController controller(
        fsm,
        inputSampler,
        actuatorDrive,
        display,
        commandQueue,
        stateQueue,
        fillLevelEstimator);

    static ControlTask controlTask(controller, AppConfig::Runtime::kControlPeriodMs);
    static MqttTask mqttTask(connectivity, commandQueue, stateQueue, AppConfig::Runtime::kMqttPeriodMs);

    const bool controlStarted = controlTask.start(
        AppConfig::Runtime::kControlTaskName,
        AppConfig::Runtime::kControlPriority,
        AppConfig::Runtime::kControlCore);
    const bool mqttStarted = mqttTask.start(
        AppConfig::Runtime::kMqttTaskName,
        AppConfig::Runtime::kMqttPriority,
        AppConfig::Runtime::kMqttCore);

    if (!controlStarted || !mqttStarted) {
        ESP_LOGE(AppConfig::kLogTag, "Failed to start one or more FreeRTOS tasks");
        return;
    }

    ESP_LOGI(AppConfig::kLogTag, "Control and MQTT tasks started");
}
