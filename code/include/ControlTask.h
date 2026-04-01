#pragma once

#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "PressController.h"

class ControlTask {
public:
    explicit ControlTask(PressController& controller, uint32_t periodMs);

    bool start(const char* taskName, UBaseType_t priority, BaseType_t coreId);
    void run();

private:
    static void taskEntry(void* context);

    PressController& controller_;
    uint32_t periodMs_;
    TaskHandle_t taskHandle_{nullptr};
};
