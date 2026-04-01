#pragma once

#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "CommandQueue.h"
#include "IConnectivity.h"
#include "StateQueue.h"

class MqttTask {
public:
    MqttTask(IConnectivity& connectivity, CommandQueue& commandQueue, StateQueue& stateQueue, uint32_t periodMs);

    bool start(const char* taskName, UBaseType_t priority, BaseType_t coreId);
    void run();

private:
    static void taskEntry(void* context);

    IConnectivity& connectivity_;
    CommandQueue& commandQueue_;
    StateQueue& stateQueue_;
    uint32_t periodMs_;
    TaskHandle_t taskHandle_{nullptr};
};
