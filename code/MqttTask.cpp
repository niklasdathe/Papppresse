#include "MqttTask.h"

#include "AppConfig.h"

MqttTask::MqttTask(IConnectivity& connectivity, CommandQueue& commandQueue, StateQueue& stateQueue, uint32_t periodMs)
    : connectivity_(connectivity)
    , commandQueue_(commandQueue)
    , stateQueue_(stateQueue)
    , periodMs_(periodMs)
{
}

bool MqttTask::start(const char* taskName, UBaseType_t priority, BaseType_t coreId)
{
    const BaseType_t result = xTaskCreatePinnedToCore(
        &MqttTask::taskEntry,
        taskName,
        AppConfig::Runtime::kMqttTaskStackSize,
        this,
        priority,
        &taskHandle_,
        coreId);

    return result == pdPASS;
}

void MqttTask::run()
{
    while (true) {
        if (connectivity_.isConnected()) {
            const std::optional<PressCommand> cmd = connectivity_.pollCommand();
            if (cmd.has_value()) {
                commandQueue_.enqueue(*cmd);
            }
        }

        while (true) {
            const std::optional<PressState> state = stateQueue_.dequeue();
            if (!state.has_value()) {
                break;
            }

            connectivity_.publishState(*state);
        }

        vTaskDelay(pdMS_TO_TICKS(periodMs_));
    }
}

void MqttTask::taskEntry(void* context)
{
    auto* self = static_cast<MqttTask*>(context);
    self->run();
}
