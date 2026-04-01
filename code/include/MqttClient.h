#pragma once

#include <cstdint>
#include <mutex>
#include <optional>
#include <queue>
#include <string>

#include "IConnectivity.h"

class MqttClient : public IConnectivity {
public:
    MqttClient(std::string mqttBroker, uint16_t mqttPort, std::string mqttUser, std::string mqttPassword);

    void publishState(PressState state) override;
    std::optional<PressCommand> pollCommand() override;
    bool isConnected() const override;

private:
    std::string mqttBroker_;
    uint16_t mqttPort_;
    std::string mqttUser_;
    std::string mqttPassword_;

    mutable std::mutex mutex_;
    std::queue<PressCommand> commandBuffer_;
    bool connected_;
};
