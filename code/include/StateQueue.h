#pragma once

#include <cstdint>
#include <optional>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "PapppresseTypes.h"

class StateQueue {
public:
    StateQueue();
    explicit StateQueue(uint32_t queueLength);
    ~StateQueue();

    bool enqueue(PressState state);
    std::optional<PressState> dequeue();

private:
    QueueHandle_t queue_;
};
