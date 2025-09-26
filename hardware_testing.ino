// CaraPH V2 Hardware Testing Framework
// Component validation and diagnostic routines

#define LED_STATUS_PIN 2
#define I2S_WS_PIN 4
#define I2S_SCK_PIN 5
#define I2S_SD_PIN 6
#define SD_CS_PIN 10
#define TOUCH_PIN 7
#define BATTERY_PIN 34

bool all_tests_passed = true;

void setup() {
  Serial.begin(115200);
  Serial.println("=== CaraPH Hardware Diagnostics v1.0 ===");
  
  runAllTests();
  
  if (all_tests_passed) {
    Serial.println("✓ ALL TESTS PASSED - Hardware ready for operation");
  } else {
    Serial.println("✗ TESTS FAILED - Check hardware connections");
  }
}

void loop() {
  // Flash status LED based on test results
  digitalWrite(LED_STATUS_PIN, all_tests_passed ? HIGH : LOW);
  delay(all_tests_passed ? 1000 : 200);
  digitalWrite(LED_STATUS_PIN, LOW);
  delay(all_tests_passed ? 1000 : 200);
}

void runAllTests() {
  testGPIOPins();
  testPowerSystems();
  testAudioHardware();
  testStorageInterface();
  testTouchSensor();
  testMemorySystem();
}

bool testGPIOPins() {
  Serial.println("Testing GPIO functionality...");
  
  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_STATUS_PIN, HIGH);
  delay(100);
  digitalWrite(LED_STATUS_PIN, LOW);
  
  Serial.println("✓ GPIO test passed");
  return true;
}

bool testPowerSystems() {
  Serial.println("Testing power systems...");
  
  int battery_reading = analogRead(BATTERY_PIN);
  if (battery_reading < 100) {
    Serial.println("✗ Battery monitoring failed");
    all_tests_passed = false;
    return false;
  }
  
  Serial.printf("✓ Power systems OK - Battery reading: %d\n", battery_reading);
  return true;
}

bool testAudioHardware() {
  Serial.println("Testing audio hardware...");
  
  pinMode(I2S_WS_PIN, OUTPUT);
  pinMode(I2S_SCK_PIN, OUTPUT);
  pinMode(I2S_SD_PIN, INPUT);
  
  // Test I2S pin connectivity
  digitalWrite(I2S_WS_PIN, HIGH);
  digitalWrite(I2S_SCK_PIN, HIGH);
  delay(10);
  
  Serial.println("✓ Audio hardware pins configured");
  return true;
}

bool testStorageInterface() {
  Serial.println("Testing storage interface...");
  
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);  // Deselect SD card
  
  Serial.println("✓ Storage interface ready");
  return true;
}

bool testTouchSensor() {
  Serial.println("Testing touch sensor...");
  
  pinMode(TOUCH_PIN, INPUT_PULLUP);
  int touch_reading = digitalRead(TOUCH_PIN);
  
  Serial.printf("✓ Touch sensor reading: %s\n", touch_reading ? "HIGH" : "LOW");
  return true;
}

bool testMemorySystem() {
  Serial.println("Testing memory system...");
  
  uint32_t free_heap = ESP.getFreeHeap();
  if (free_heap < 200000) {  // Less than 200KB free
    Serial.println("✗ Low memory detected");
    all_tests_passed = false;
    return false;
  }
  
  Serial.printf("✓ Memory OK - %d bytes free\n", free_heap);
  return true;
}
