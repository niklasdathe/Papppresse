#include <Arduino.h>
#include "includes/Bts7960ActuatorDrive.h"
#include "includes/RawDigitalInput.h"
#include "includes/DebouncedDigitalInput.h"
#include "includes/IActuatorDrive.h"
#include "includes/IDigitalInput.h"

namespace {
constexpr uint8_t rPwmPin = 1;
constexpr uint8_t lPwmPin = 47;
constexpr uint8_t rEnPin = 38;
constexpr uint8_t lEnPin = 48;

// DI mapping of the controller board:
// DI1->GPIO4, DI2->GPIO5, DI3->GPIO6, DI4->GPIO7, DI5->GPIO8
constexpr uint8_t eStopPin = 4;
constexpr uint8_t topEndstopPin = 5;
constexpr uint8_t bottomEndstopPin = 6;
constexpr uint8_t doorClosedPin = 7;
constexpr uint8_t startPulsePin = 8;

constexpr uint32_t debounceMs = 30;
constexpr uint32_t readIntervalMs = 20;
}

RawDigitalInput eStopRaw(eStopPin);
RawDigitalInput topRaw(topEndstopPin);
RawDigitalInput bottomRaw(bottomEndstopPin);
RawDigitalInput doorRaw(doorClosedPin);
RawDigitalInput startRaw(startPulsePin);

DebouncedDigitalInput eStopDebounced(eStopRaw, true, debounceMs);
DebouncedDigitalInput topDebounced(topRaw, true, debounceMs);
DebouncedDigitalInput bottomDebounced(bottomRaw, true, debounceMs);
DebouncedDigitalInput doorDebounced(doorRaw, true, debounceMs);
DebouncedDigitalInput startDebounced(startRaw, true, debounceMs);

Bts7960ActuatorDrive actuator(rPwmPin, lPwmPin, rEnPin, lEnPin);
IActuatorDrive& drive = actuator;
IDigitalInput& eStop = eStopDebounced;
IDigitalInput& topEndstop = topDebounced;
IDigitalInput& bottomEndstop = bottomDebounced;
IDigitalInput& doorClosed = doorDebounced;
IDigitalInput& startPulse = startDebounced;

enum class Motion {
	Extending,
	Retracting,
	Stopped
};

Motion motion = Motion::Stopped;
uint32_t lastReadMs = 0;
bool lastEStop = false;
bool lastTop = false;
bool lastBottom = false;
bool lastDoorClosed = false;
bool lastStartPulse = false;
bool runEnabled = false;

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

	lastEStop = eStop.read();
	lastTop = topEndstop.read();
	lastBottom = bottomEndstop.read();
	lastDoorClosed = doorClosed.read();
	lastStartPulse = startPulse.read();

	Serial.println("Papppresse interface test start");
	Serial.print("E-Stop: ");
	Serial.println(lastEStop ? "ACTIVE" : "INACTIVE");
	Serial.print("Top endstop: ");
	Serial.println(lastTop ? "ACTIVE" : "INACTIVE");
	Serial.print("Bottom endstop: ");
	Serial.println(lastBottom ? "ACTIVE" : "INACTIVE");
	Serial.print("Door closed: ");
	Serial.println(lastDoorClosed ? "ACTIVE" : "INACTIVE");
	Serial.print("Start pulse: ");
	Serial.println(lastStartPulse ? "ACTIVE" : "INACTIVE");

	setMotion(Motion::Stopped);
}

void loop()
{
	const uint32_t now = millis();
	if ((now - lastReadMs) < readIntervalMs) {
		return;
	}
	lastReadMs = now;

	const bool currentEStop = eStop.read();
	const bool currentTop = topEndstop.read();
	const bool currentBottom = bottomEndstop.read();
	const bool currentDoorClosed = doorClosed.read();
	const bool currentStartPulse = startPulse.read();

	if (currentEStop != lastEStop) {
		lastEStop = currentEStop;
		Serial.print("E-Stop -> ");
		Serial.println(currentEStop ? "ACTIVE" : "INACTIVE");
	}

	if (currentTop != lastTop) {
		lastTop = currentTop;
		Serial.print("Top endstop -> ");
		Serial.println(currentTop ? "ACTIVE" : "INACTIVE");
	}

	if (currentBottom != lastBottom) {
		lastBottom = currentBottom;
		Serial.print("Bottom endstop -> ");
		Serial.println(currentBottom ? "ACTIVE" : "INACTIVE");
	}

	if (currentDoorClosed != lastDoorClosed) {
		lastDoorClosed = currentDoorClosed;
		Serial.print("Door closed -> ");
		Serial.println(currentDoorClosed ? "ACTIVE" : "INACTIVE");
	}

	if (currentStartPulse != lastStartPulse) {
		lastStartPulse = currentStartPulse;
		Serial.print("Start pulse -> ");
		Serial.println(currentStartPulse ? "ACTIVE" : "INACTIVE");

		if (currentStartPulse) {
			runEnabled = true;
			Serial.println("Run enabled by start pulse");
		}
	}

	if (currentEStop || !currentDoorClosed) {
		runEnabled = false;
		setMotion(Motion::Stopped);
		return;
	}

	if (!runEnabled) {
		setMotion(Motion::Stopped);
		return;
	}

	if (motion == Motion::Stopped) {
		if (!currentTop) {
			setMotion(Motion::Extending);
		} else if (!currentBottom) {
			setMotion(Motion::Retracting);
		}
		return;
	}

	if (motion == Motion::Extending && currentTop) {
		setMotion(Motion::Retracting);
	} else if (motion == Motion::Retracting && currentBottom) {
		setMotion(Motion::Extending);
	}
}
