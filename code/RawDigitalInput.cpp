#include "includes/RawDigitalInput.h"

RawDigitalInput::RawDigitalInput(uint8_t pin) : pin_(pin) {
    // Initialize the pin as an input
    pinMode(pin_, INPUT);
}

bool RawDigitalInput::read() {
    // Read the digital value from the pin
    return digitalRead(pin_) == HIGH;
}

