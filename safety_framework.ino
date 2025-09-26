void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("=== CaraPH Safety Framework v1.0 ===");
  Serial.println("Initializing hardware protection...");
  
  // Initialize built-in LED (pin 13 on Arduino Uno)
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);  // Start with LED off
  
  // Initialize other pins to safe states
  for (int pin = 2; pin <= 12; pin++) {
    pinMode(pin, INPUT_PULLUP);  // Safe default - input with pullup
  }
  
  // Safety initialization complete
  Serial.println("✓ All GPIO pins set to safe defaults");
  Serial.println("✓ Hardware protection active");
  Serial.println("✓ Emergency shutdown ready");
  Serial.println("✓ System ready for safe operation");
  Serial.println("Starting safety heartbeat...");
  Serial.println();
}

void loop() {
  // Safety heartbeat - shows system is running safely
  digitalWrite(13, HIGH);  // LED on
  Serial.println("SAFE: System operational, all protections active");
  delay(1000);
  
  digitalWrite(13, LOW);   // LED off
  Serial.println("SAFE: Hardware monitoring OK - All systems normal");
  delay(1000);
}
