#ifndef RAW_DIGITAL_INPUT_H
#define RAW_DIGITAL_INPUT_H

#include <Arduino.h>
#include <cstdint>
#include "IDigitalInput.h"


class RawDigitalInput : public IDigitalInput {
    private:
        uint8_t pin_; 
    public:
        explicit RawDigitalInput(uint8_t pin_);
        bool read() override;
};

#endif