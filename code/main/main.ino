#include <Arduino.h>
#include "includes/Bts7960ActuatorDrive.h"
#include "includes/RawDigitalInput.h"
#include "includes/DebouncedDigitalInput.h"
#include "includes/IActuatorDrive.h"
#include "includes/IDigitalInput.h"

namespace {
constexpr uint8_t rPwmPin = 5;
constexpr uint8_t lPwmPin = 6;
constexpr uint8_t rEnPin = 7;
constexpr uint8_t lEnPin = 8;

constexpr uint8_t upperEndstopPin = 2;
constexpr uint8_t lowerEndstopPin = 3;

constexpr uint32_t debounceMs = 30;
constexpr uint32_t readIntervalMs = 20;
}

RawDigitalInput upperRaw(upperEndstopPin);
RawDigitalInput lowerRaw(lowerEndstopPin);
DebouncedDigitalInput upperDebounced(upperRaw, true, debounceMs);
DebouncedDigitalInput lowerDebounced(lowerRaw, true, debounceMs);

Bts7960ActuatorDrive actuator(rPwmPin, lPwmPin, rEnPin, lEnPin);
IActuatorDrive& drive = actuator;
IDigitalInput& upperEndstop = upperDebounced;
IDigitalInput& lowerEndstop = lowerDebounced;

enum class Motion {
	Extending,
	Retracting,
	Stopped
};

Motion motion = Motion::Stopped;
uint32_t lastReadMs = 0;
bool lastUpper = false;
bool lastLower = false;

void setMotion(Motion nextMotion)
{
	if (nextMotion == motion) {
		return;
	}

	motion = nextMotion;

	switch (motion) {
	case Motion::Extending:
		drive.extend();
		Serial.println("Drive: EXTEND");
		break;
	case Motion::Retracting:
		drive.retract();
		Serial.println("Drive: RETRACT");
		break;
	case Motion::Stopped:
		drive.stop();
		Serial.println("Drive: STOP");
		break;
	}
}

void setup()
{
	Serial.begin(115200);
	delay(250);

	lastUpper = upperEndstop.read();
	lastLower = lowerEndstop.read();

	Serial.println("Papppresse interface test start");
	Serial.print("Upper endstop: ");
	Serial.println(lastUpper ? "ACTIVE" : "INACTIVE");
	Serial.print("Lower endstop: ");
	Serial.println(lastLower ? "ACTIVE" : "INACTIVE");

	if (!lastUpper) {
		setMotion(Motion::Extending);
	} else if (!lastLower) {
		setMotion(Motion::Retracting);
	} else {
		setMotion(Motion::Stopped);
	}
}

void loop()
{
	const uint32_t now = millis();
	if ((now - lastReadMs) < readIntervalMs) {
		return;
	}
	lastReadMs = now;

	const bool upper = upperEndstop.read();
	const bool lower = lowerEndstop.read();

	if (upper != lastUpper) {
		lastUpper = upper;
		Serial.print("Upper endstop -> ");
		Serial.println(upper ? "ACTIVE" : "INACTIVE");
	}

	if (lower != lastLower) {
		lastLower = lower;
		Serial.print("Lower endstop -> ");
		Serial.println(lower ? "ACTIVE" : "INACTIVE");
	}

	if (motion == Motion::Extending && upper) {
		setMotion(Motion::Retracting);
	} else if (motion == Motion::Retracting && lower) {
		setMotion(Motion::Extending);
	} else if (upper && lower) {
		setMotion(Motion::Stopped);
	}
}
