#include "MqttClient.h"

#include "esp_log.h"

namespace {
constexpr const char* kTag = "MqttClient";

const char* toStateString(PressState state)
{
    switch (state) {
    case PressState::INIT_CHECK:
        return "INIT_CHECK";
    case PressState::INIT_UP:
        return "INIT_UP";
    case PressState::INIT_PAUSE:
        return "INIT_PAUSE";
    case PressState::READY_TOP:
        return "READY_TOP";
    case PressState::PRESS_DOWN:
        return "PRESS_DOWN";
    case PressState::PAUSE_PRESS:
        return "PAUSE_PRESS";
    case PressState::SUCCESS_LATCH:
        return "SUCCESS_LATCH";
    case PressState::RETURN_UP_SUCCESS:
        return "RETURN_UP_SUCCESS";
    case PressState::PAUSE_RETURN_SUCCESS:
        return "PAUSE_RETURN_SUCCESS";
    case PressState::ABORT_LATCH:
        return "ABORT_LATCH";
    case PressState::RETURN_UP_ABORT:
        return "RETURN_UP_ABORT";
    case PressState::PAUSE_RETURN_ABORT:
        return "PAUSE_RETURN_ABORT";
    default:
        return "UNKNOWN";
    }
}
}

MqttClient::MqttClient(std::string mqttBroker, uint16_t mqttPort, std::string mqttUser, std::string mqttPassword)
    : mqttBroker_(std::move(mqttBroker))
    , mqttPort_(mqttPort)
    , mqttUser_(std::move(mqttUser))
    , mqttPassword_(std::move(mqttPassword))
    , connected_(!mqttBroker_.empty())
{
    ESP_LOGI(kTag, "MQTT stub initialized for broker %s:%u", mqttBroker_.c_str(), mqttPort_);
}

void MqttClient::publishState(PressState state)
{
    ESP_LOGI(kTag, "publishState: %s", toStateString(state));
}

std::optional<PressCommand> MqttClient::pollCommand()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (commandBuffer_.empty()) {
        return std::nullopt;
    }

    const PressCommand cmd = commandBuffer_.front();
    commandBuffer_.pop();
    return cmd;
}

bool MqttClient::isConnected() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return connected_;
}
