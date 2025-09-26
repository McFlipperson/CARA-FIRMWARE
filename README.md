# CaraPH V2 Firmware
ESP32-C3 firmware for CaraPH wearable medical device

## Safety Framework v1.0
Hardware protection system that initializes all GPIO pins to safe states and provides continuous system monitoring.

### Features
- Safe GPIO initialization on startup
- Continuous system health monitoring  
- Visual status indication (LED heartbeat)
- Hardware damage prevention
- Serial logging for debugging

### Testing
Test in Wokwi Arduino simulator:
1. Create new Arduino Uno project
2. Copy safety_framework.ino code
3. Run simulation - LED should blink, serial output shows safety status

### Development Status
- ✅ Safety framework foundation complete
- 🔄 Audio drivers (in progress - Jay)
- 🔄 WiFi communication (planned - Nova)
- 🔄 File encryption (planned - Nova)
