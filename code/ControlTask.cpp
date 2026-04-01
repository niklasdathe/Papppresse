#include "ControlTask.h"

#include "AppConfig.h"

#include "esp_timer.h"

ControlTask::ControlTask(PressController& controller, uint32_t periodMs)
    : controller_(controller)
    , periodMs_(periodMs)
{
}

bool ControlTask::start(const char* taskName, UBaseType_t priority, BaseType_t coreId)
{
    const BaseType_t result = xTaskCreatePinnedToCore(
        &ControlTask::taskEntry,
        taskName,
        AppConfig::Runtime::kControlTaskStackSize,
        this,
        priority,
        &taskHandle_,
        coreId);

    return result == pdPASS;
}

void ControlTask::run()
{
    TickType_t lastWake = xTaskGetTickCount();

    while (true) {
        const uint32_t nowMs = static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
        controller_.controlStep(nowMs);
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(periodMs_));
    }
}

void ControlTask::taskEntry(void* context)
{
    auto* self = static_cast<ControlTask*>(context);
    self->run();
}
