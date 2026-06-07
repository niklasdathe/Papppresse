# Code for Papppresse
## Firmware (ESP-IDF)
The firmware is structured as a standard ESP-IDF project for VS Code.

### Build and flash
1. Open the project in VS Code with the ESP-IDF extension enabled.
2. Ensure `idf.currentSetup` points to your ESP-IDF installation.
3. Use the ESP-IDF commands in VS Code:
	- Set target: `esp32s3`
	- Build
	- Flash / Monitor

Equivalent CLI commands from project root:

```bash
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

### Input diagnostics mode (state machine input check)
Use this mode to validate the exact input stack used by the state machine, including:
- raw GPIO levels
- debounced and polarity-mapped active states
- InputSampler rising-edge flags (`*_reached`, `*_detected`)

How to enable:
1. In `code/include/AppConfig.h`, set `AppConfig::Diagnostics::kEnableInputDiagnosticsOnly` to `true`.
2. Build, flash, and open monitor.
3. Operate each input and verify logs from tag `InputDiag`.

The diagnostics loop reuses the same classes and configuration as production (`RawDigitalInput`, `DebouncedDigitalInput`, `AnalogCurrentSensor`, `InputSampler`, and `AppConfig` pin/polarity constants).

### Over-current return (BTS7960 L_IS / R_IS)
During the down stroke the press returns up once the motor current reaches a
configurable limit. `AnalogCurrentSensor` reads up to two H-bridge sense pins on
ADC1 and trips on the larger value, so the active direction is covered
automatically (down stroke loads `L_IS`, up stroke loads `R_IS`).

On the `ESP32-S3-POE-ETH-8DI-8DO` only one ADC pin is exposed (GPIO1 = ADC1_CH0),
so RPWM was moved to GPIO45 and only the down-stroke line `L_IS` is wired to
GPIO1; both channel constants point at `ADC1_CH0`. See `electrical/connector
pins.md` for the wiring.

Configure it in `code/include/AppConfig.h`, namespace `AppConfig::Sensor`:
- `kOverCurrentLimitAmps` — the trip current (sets the pressing force).
- `kLIsAdcChannel` / `kRIsAdcChannel` — ADC1 channels wired to `L_IS` / `R_IS`.
- `kSenseResistorOhms` / `kCurrentSenseRatio` — IS hardware constants used by
  `currentAmpsToRaw()` to turn the Ampere limit into a raw ADC threshold.

### Calibrating the current sensor
The Ampere limit is converted to a raw ADC threshold with nominal constants
(`k_ILIS = 8500`, `R_IS = 1 kOhm`, ADC full scale `3300 mV`). These are only
approximate: per the BTS7960 datasheet the sense ratio `k_ILIS` varies with load
and part-to-part (≈ 6000–14000), the IS resistor has tolerance, and the ESP32-S3
ADC at 12 dB attenuation is nonlinear near the rails. So for a trustworthy trip
point, calibrate against a measured current.

What the diagnostics log shows (tag `InputDiag`):
- `lIs` — raw ADC count on the L_IS channel (down-stroke sense line).
- `current` — value compared against the threshold (max of the channels).
- `thr` — the raw threshold derived from `kOverCurrentLimitAmps`.

Procedure:
1. In `code/include/AppConfig.h` set `Diagnostics::kEnableInputDiagnosticsOnly = true`,
   then build, flash, and open the monitor.
2. Put a clamp ammeter on a motor lead and run the actuator into increasing load
   on the **down stroke** (e.g. press against a full bale). Watch the `InputDiag`
   line and record a pair `(I_meas, lIs)` — the measured current and the raw count
   — at the force where you want the press to return up.
3. Choose how accurate you need the Ampere label to be:
   - **Quick (force is right, label approximate):** set `kOverCurrentLimitAmps`
     until the logged `thr` matches the `lIs` you recorded. The press now returns
     at that load; the number is nominal.
   - **Accurate (label matches reality):** the model gives `I ≈ raw * 0.00685 A`
     with the default constants, so the analytic current at your recorded count is
     `I_analytic = lIs * 0.00685`. Correct the sense ratio once:
     `kCurrentSenseRatio_new = 8500 * (I_meas / I_analytic)`. After that the
     analytic Amperes track the clamp meter, so just set `kOverCurrentLimitAmps`
     to the real current you want to trip at.
4. Set `Diagnostics::kEnableInputDiagnosticsOnly = false`, rebuild, and verify the
   press returns at the intended load.

Notes:
- Keep the trip point in the mid ADC range (raw well below 4095). At 20 A the
  default wiring sits around raw 2920 (~2.35 V), which is comfortable; pushing the
  limit toward ~28 A saturates the ADC input.
- Motor inrush at the start of the down stroke can briefly spike the reading. Set
  the limit above inrush, or add a persistence filter to `AnalogCurrentSensor` if
  you see false early returns.

### Source layout
- `code/app_main.cpp`: Runtime wiring and task startup
- `code/include`: Interfaces, data types, and class headers
- `code/*.cpp`: Adapter and control implementations

The adapter and control classes from the UML class diagram are implemented as ESP-IDF C++ classes.
