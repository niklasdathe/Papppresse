# Papppresse

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

### Source layout
- `code/app_main.cpp`: Runtime wiring and task startup
- `code/include`: Interfaces, data types, and class headers
- `code/*.cpp`: Adapter and control implementations

The adapter and control classes from the UML class diagram are implemented as ESP-IDF C++ classes.

## Diagrams
### class diagram (UML 2.5.1)
![Architecture](<diagrams/export/class diagramm Papppresse-Seite-1.svg>)
### electrical schematic
![Architecture](<diagrams/export/electrical schematic Papppresse-Seite-1.svg>)
### sequence diagrams
#### sequence control cycle (UML 2.5.1)
![Architecture](<diagrams/export/sequence diagram control_cycle Papppresse-Seite-1.svg>)
#### sequence mqtt command ingress (UML 2.5.1)
![Architecture](<diagrams/export/sequence diagram mqtt_command_ingress Papppresse-Seite-1.svg>)
#### sequence mqtt publsih state (UML 2.5.1)
![Architecture](<diagrams/export/sequence diagram mqtt_state_publish Papppresse-Seite-1.svg>)
#### sequence estop tripped (UML 2.5.1)
(`sd door_opened` is vitually the same as hadware power cut is not modeled in estop_trip)  

![Architecture](<diagrams/export/sequence diagram estop_trip Papppresse-Seite-1.svg>)
#### sequence endstop reached (UML 2.5.1)
![Architecture](<diagrams/export/sequence diagram endstop_reached Papppresse-Seite-1.svg>)
### state diagram
![Architecture](<diagrams/export/state diagram Papppresse-Seite-1.svg>)
