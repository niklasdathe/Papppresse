#include "CommandQueue.h"

#include "AppConfig.h"

CommandQueue::CommandQueue()
    : CommandQueue(AppConfig::Runtime::kCommandQueueDefaultLength)
{
}

CommandQueue::CommandQueue(uint32_t queueLength)
    : queue_(xQueueCreate(queueLength, sizeof(PressCommand)))
{
}

CommandQueue::~CommandQueue()
{
    if (queue_ != nullptr) {
        vQueueDelete(queue_);
    }
}

bool CommandQueue::enqueue(PressCommand cmd)
{
    if (queue_ == nullptr) {
        return false;
    }

    return xQueueSend(queue_, &cmd, AppConfig::Runtime::kQueueNoWaitTicks) == pdTRUE;
}

std::optional<PressCommand> CommandQueue::dequeue()
{
    if (queue_ == nullptr) {
        return std::nullopt;
    }

    PressCommand cmd = PressCommand::STOP;
    if (xQueueReceive(queue_, &cmd, AppConfig::Runtime::kQueueNoWaitTicks) == pdTRUE) {
        return cmd;
    }

    return std::nullopt;
}
