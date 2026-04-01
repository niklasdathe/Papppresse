#pragma once

#include <cstdint>
#include <optional>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "PapppresseTypes.h"

class CommandQueue {
public:
    CommandQueue();
    explicit CommandQueue(uint32_t queueLength);
    ~CommandQueue();

    bool enqueue(PressCommand cmd);
    std::optional<PressCommand> dequeue();

private:
    QueueHandle_t queue_;
};
