# State Machine Implementation Change Log

This document records all class-level changes made while implementing the updated Papppresse state machine.

## Design principles used

The implementation follows a strict **trigger -> guard -> action** model:

- **Triggers** are external events/signals (`start_pressed`, `door_closed` changes, `estop`, `fault_reset_requested`).
- **Guards** are boolean conditions over current inputs (`top_es`, `bottom_es || current_over`, etc.).
- **Actions** are outputs/effects (`driveCommand`, `successPulse`, `abortPulse`).
- Transition priority in `PRESS_DOWN` is implemented as specified:
  1. `hardSafetyFault` (global, via `estop`)
  2. `doorOpened`
  3. completion (`bottom_es || current_over`)
  4. `startPressed` (abort)

## Class and type changes

## `PressState` (in `include/PapppresseTypes.h`)

### Changed enum set

Removed legacy states:
- `INIT_CHECK`
- `SUCCESS_LATCH`
- `ABORT_LATCH`
- `RETURN_UP_ABORT`
- `PAUSE_RETURN_ABORT`

Added/renamed states to match the diagram:
- `RETURN_UP_ABORTED` (renamed from `RETURN_UP_ABORT`)
- `PAUSE_RETURN_ABORTED` (renamed from `PAUSE_RETURN_ABORT`)
- `SAFE_STOP` (new)

Current state set:
- `INIT_PAUSE`
- `INIT_UP`
- `READY_TOP`
- `PRESS_DOWN`
- `PAUSE_PRESS`
- `RETURN_UP_ABORTED`
- `RETURN_UP_SUCCESS`
- `PAUSE_RETURN_ABORTED`
- `PAUSE_RETURN_SUCCESS`
- `SAFE_STOP`

## `InputSnapshot` (in `include/PapppresseTypes.h`)

### New trigger field

Added:
- `bool fault_reset_requested{false};`

Purpose:
- Carries the explicit `faultReset` trigger into the FSM for `SAFE_STOP -> Operational` re-entry.

### Active vs edge semantics

Replaced mixed level/event fields with explicit channels:

- Level (state/guard) inputs:
  - `top_endstop_active`
  - `bottom_endstop_active`
  - `over_current_active`
- Edge (event/trigger) inputs:
  - `top_endstop_reached` (rising edge)
  - `bottom_endstop_reached` (rising edge)
  - `over_current_detected` (rising edge)

Result:
- Guards use `*_active`.
- Event-triggered transitions use `*_reached` / `*_detected`.

## `InputSampler` (in `include/InputSampler.h`, `InputSampler.cpp`)

### New internal memory for edge detection

Added private members:
- `initialized_`
- `prevTopEndstopActive_`
- `prevBottomEndstopActive_`
- `prevOverCurrentActive_`

Behavior:
- Samples current levels every control cycle.
- Emits rising-edge events for reached/detected signals by comparing current vs previous levels.
- On the first sample, edge events are suppressed to avoid false startup events.

## `FsmStepResult` (in `include/PapppresseTypes.h`)

### Default state update

Changed default `state` initialization from `INIT_CHECK` to `INIT_PAUSE`.

## `PressFsm` (in `include/PressFsm.h`, `PressFsm.cpp`)

### New private member

Added:
- `bool needsOperationalEntry_{true};`

Purpose:
- Represents the composite-state entry flow into `Operational` without using a synthetic public state.

### Transition engine rewrite

Updated `step(const InputSnapshot&)` to match the formal model:

- Global hard-safety transition:
  - Any non-`SAFE_STOP` state with `estop == true` transitions to `SAFE_STOP`.
- Fault reset transition:
  - `SAFE_STOP` exits only on `fault_reset_requested == true` and `estop == false`.
- Operational initial branching implemented via guard resolution:
  - `!door_closed` -> `INIT_PAUSE`
  - `door_closed && !top_es` -> `INIT_UP`
  - `door_closed && top_es` -> `READY_TOP`
- Pause/resume choices are guard-driven and state-local (no latch states).
- Trigger/guard separation for sensor semantics:
  - `top_endstop_reached`, `bottom_endstop_reached`, `over_current_detected` are used as triggers.
  - `top_endstop_active`, `bottom_endstop_active`, `over_current_active` are used as guards.
- `PAUSE_PRESS` intentionally has no `startPressed` abort transition (per note: not implemented).
- `successPulse` is emitted only on transitions to `RETURN_UP_SUCCESS` caused by completion.
- `abortPulse` is emitted only on transition `PRESS_DOWN -> RETURN_UP_ABORTED`.

### Reset behavior

Updated `reset()`:
- Sets `currentState_ = INIT_PAUSE`
- Sets `needsOperationalEntry_ = true`

## `PressController` (in `include/PressController.h`, `PressController.cpp`)

### Member changes

Removed:
- `bool estopLatched_{false};`
- `bool doorOpenLatched_{false};`

Added:
- `bool faultResetRequested_{false};`

### Behavior changes

- `controlStep()` now forwards `faultResetRequested_` into `inputs.fault_reset_requested` as a one-shot trigger.
- Removed forced input overrides for latched door/e-stop values.
  - This restores direct sensor-driven guard evaluation for pause/resume transitions.
- `handleCommand(PressCommand::RESET)` now requests fault reset (one-shot) instead of directly calling `fsm_.reset()`.
- `estopInterrupt()` and `doorOpenedInterrupt()` now perform immediate actuator stop only.
- State string mapping updated to new state names, including `SAFE_STOP`.
- Return-state helper predicates updated to `RETURN_UP_ABORTED` / `PAUSE_RETURN_ABORTED`.

## `MqttClient` (in `MqttClient.cpp`)

### State serialization update

- Updated state-to-string mapping to match new `PressState` names.
- Added `SAFE_STOP` output mapping.
- Removed references to legacy latch states.

## `StateQueue` (in `StateQueue.cpp`)

### Internal default placeholder update

- Changed local dequeue placeholder from `PressState::INIT_CHECK` to `PressState::INIT_PAUSE`.
- No API/signature changes.

## Impact on class diagram

Classes/types with diagram-relevant changes:
- `PressFsm` (new private attribute and transition logic semantics)
- `PressController` (attribute changes and command handling semantics)
- `InputSnapshot` (new field)
- `PressState` (enum set changed)

Classes with implementation-only mapping updates:
- `MqttClient`
- `StateQueue`
