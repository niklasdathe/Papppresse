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

### Source layout
- `code/app_main.cpp`: Runtime wiring and task startup
- `code/include`: Interfaces, data types, and class headers
- `code/*.cpp`: Adapter and control implementations

The adapter and control classes from the UML class diagram are implemented as ESP-IDF C++ classes.
