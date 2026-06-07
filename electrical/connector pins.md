# pin description of used connectors
### mains power in (`X1`)
`X1:1` : L1  
`X1:2` : PE  
`X1:3` : N  

### endstops connector (`X2`)
`X2:1` : top_es  
`X2:2` : DGND  
`X2:3` : bottom_es  
`X2:4` : DGND  

### door detection connector (`X3`)
`X3:1` : DGND  
`X3:2` : door_closed 

### start buttons connector (`X4`)
`X4:1` : start_pressed  
`X4:2` : DGND  

### linear actuator connector (`X5`)
`X5:1` : M+  
`X5:2` : M-

### motor driver (BTS7960 / IBT-2) control signals
Wiring between the ESP32-S3 and the IBT-2 H-bridge control header. GPIO numbers
mirror `code/include/AppConfig.h`.

| IBT-2 pin | Signal            | ESP32-S3 GPIO    | Notes                                |
|-----------|-------------------|------------------|--------------------------------------|
| RPWM      | up-stroke PWM     | GPIO45           | `ActuatorPins::kRpwmpin` (moved off GPIO1) |
| LPWM      | down-stroke PWM   | GPIO47           | `ActuatorPins::kLpwmpin`             |
| R_EN      | up-stroke enable  | GPIO38           | `ActuatorPins::kRenablePin`          |
| L_EN      | down-stroke enable| GPIO48           | `ActuatorPins::kLenablePin`          |
| L_IS      | down-stroke sense | GPIO1 (ADC1_CH0) | `Sensor::kLIsAdcChannel` — see below |
| R_IS      | up-stroke sense   | not connected    | no 2nd exposed ADC pin on this board |
| VCC / GND | 5 V logic supply  | 5 V / GND        |                                      |

#### Over-current sensing (L_IS)
The press leaves the down stroke and returns up once the sensed motor current
reaches `Sensor::kOverCurrentLimitAmps` (default 20 A). The current limit is
converted to a raw ADC threshold from the BTS7960 sense ratio (`k_ILIS`) and the
IS resistor (`R_IS`); see `Sensor::currentAmpsToRaw()` in `AppConfig.h`.

ADC pin constraint: this board (`ESP32-S3-POE-ETH-8DI-8DO`) only exposes one
ADC-capable pin — GPIO1 (`ADC_IO1` / `ADC1_CH0`). RPWM was therefore moved from
GPIO1 to GPIO45 (a free exposed pin) so GPIO1 can read the down-stroke sense line
**L_IS**. R_IS (up-stroke) has no second ADC pin and is left unconnected; in
firmware both `Sensor::kLIsAdcChannel` and `kRIsAdcChannel` point at `ADC1_CH0`,
so the sensor's max-of-both reduces to L_IS. If GPIO2/GPIO3 (`ADC_IO2/3`) ever
become reachable, wire R_IS there and split the two constants for dual sensing.

> Rewiring required: move the RPWM lead from header pin **1** to header pin
> **45**, and connect the IBT-2 **L_IS** pin to header pin **1** (GPIO1).

Use the input-diagnostics mode (`Diagnostics::kEnableInputDiagnosticsOnly`) to
read the live `lIs`/`current` raw counts versus `thr` and calibrate
`kOverCurrentLimitAmps`.
