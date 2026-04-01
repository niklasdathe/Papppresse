#include "StateQueue.h"

#include "AppConfig.h"

StateQueue::StateQueue()
    : StateQueue(AppConfig::Runtime::kStateQueueDefaultLength)
{
}

StateQueue::StateQueue(uint32_t queueLength)
    : queue_(xQueueCreate(queueLength, sizeof(PressState)))
{
}

StateQueue::~StateQueue()
{
    if (queue_ != nullptr) {
        vQueueDelete(queue_);
    }
}

bool StateQueue::enqueue(PressState state)
{
    if (queue_ == nullptr) {
        return false;
    }

    return xQueueSend(queue_, &state, AppConfig::Runtime::kQueueNoWaitTicks) == pdTRUE;
}

std::optional<PressState> StateQueue::dequeue()
{
    if (queue_ == nullptr) {
        return std::nullopt;
    }

    PressState state = PressState::INIT_CHECK;
    if (xQueueReceive(queue_, &state, AppConfig::Runtime::kQueueNoWaitTicks) == pdTRUE) {
        return state;
    }

    return std::nullopt;
}
