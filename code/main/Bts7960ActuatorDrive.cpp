#include "includes/Bts7960ActuatorDrive.h"

Bts7960ActuatorDrive::Bts7960ActuatorDrive(uint8_t rPwmPin,
                                           uint8_t lPwmPin,
                                           uint8_t rEnPin,
                                           uint8_t lEnPin)
        : rPwmPin_(rPwmPin),
            lPwmPin_(lPwmPin),
            rEnPin_(rEnPin),
            lEnPin_(lEnPin)
{
    pinMode(rPwmPin_, OUTPUT);
    pinMode(lPwmPin_, OUTPUT);
    pinMode(rEnPin_, OUTPUT);
    pinMode(lEnPin_, OUTPUT);

    stop();
}

void Bts7960ActuatorDrive::extend()
{
    digitalWrite(rEnPin_, HIGH);
    digitalWrite(lEnPin_, HIGH);

    digitalWrite(lPwmPin_, LOW);
    digitalWrite(rPwmPin_, HIGH);
}

void Bts7960ActuatorDrive::retract()
{
    digitalWrite(rEnPin_, HIGH);
    digitalWrite(lEnPin_, HIGH);

    digitalWrite(rPwmPin_, LOW);
    digitalWrite(lPwmPin_, HIGH);
}

void Bts7960ActuatorDrive::stop()
{
    digitalWrite(rPwmPin_, LOW);
    digitalWrite(lPwmPin_, LOW);

    digitalWrite(rEnPin_, LOW);
    digitalWrite(lEnPin_, LOW);
}