#ifndef BTS7960ACTUATORDRIVE_H
#define BTS7960ACTUATORDRIVE_H

#include <cstdint>
#include <Arduino.h>
#include "IActuatorDrive.h"

class Bts7960ActuatorDrive : public IActuatorDrive {
    public:
        Bts7960ActuatorDrive(uint8_t lPwmPin_, uint8_t rPwmPin_, uint8_t lEnPin_, uint8_t rEnPin_);
        void extend() override;
        void retract() override;
        void stop() override;

    private:
        uint8_t lPwmPin_;
        uint8_t rPwmPin_;
        uint8_t lEnPin_;
        uint8_t rEnPin_;
};

#endif