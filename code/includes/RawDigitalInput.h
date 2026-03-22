#include <cstdint>
#include "IDigitalInput.h"


class RawDigitalInput : public IDigitalInput {
    private:
        uint8_t pin_; 
    public:
        RawDigitalInput(uint8_t pin_);
        bool read() override;
};