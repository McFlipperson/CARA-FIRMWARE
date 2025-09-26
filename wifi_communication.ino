// CaraPH V2 WiFi Communication Framework
// Connects to Brain system and handles file uploads

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configuration
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* brain_server = "192.168.1.100";  // Brain IP address
const int brain_port = 8080;

// Status tracking
bool wifi_connected = false;
bool brain_connected = false;
unsigned long last_connection_attempt = 0;
const unsigned long reconnect_interval = 30000;  // 30 seconds

void setup() {
  Serial.begin(115200);
  Serial.println("=== CaraPH WiFi Communication v1.0 ===");
  
  // Initialize WiFi
  initializeWiFi();
}

void loop() {
  // Check WiFi status every 30 seconds
  if (millis() - last_connection_attempt > reconnect_interval) {
    checkWiFiConnection();
    last_connection_attempt = millis();
  }
  
  // Test Brain communication if WiFi is connected
  if (wifi_connected && !brain_connected) {
    testBrainConnection();
  }
  
  delay(1000);
}

void initializeWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Serial.println();
    Serial.print("WiFi connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("WiFi connection failed");
  }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    wifi_connected = false;
    brain_connected = false;
    Serial.println("WiFi disconnected, attempting reconnection...");
    WiFi.reconnect();
  } else {
    wifi_connected = true;
  }
}

void testBrainConnection() {
  HTTPClient http;
  String url = "http://" + String(brain_server) + ":" + String(brain_port) + "/status";
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    brain_connected = true;
    Serial.println("Brain server connected!");
    String response = http.getString();
    Serial.println("Brain response: " + response);
  } else {
    brain_connected = false;
    Serial.println("Brain server connection failed: " + String(httpCode));
  }
  
  http.end();
}

// Function to upload file to Brain (placeholder for future)
bool uploadFileToBrain(String filename, uint8_t* data, size_t length) {
  if (!wifi_connected || !brain_connected) {
    Serial.println("Cannot upload: No connection to Brain");
    return false;
  }
  
  // File upload implementation goes here
  Serial.println("File upload function ready for implementation");
  return true;
}

// Function to send device status to Brain
void sendDeviceStatus() {
  if (!wifi_connected || !brain_connected) return;
  
  DynamicJsonDocument status(1024);
  status["device_id"] = "CaraPH_001";
  status["battery_level"] = 85;  // Placeholder
  status["status"] = "operational";
  status["free_memory"] = ESP.getFreeHeap();
  
  String statusJson;
  serializeJson(status, statusJson);
  
  HTTPClient http;
  String url = "http://" + String(brain_server) + ":" + String(brain_port) + "/device/status";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(statusJson);
  
  if (httpCode == 200) {
    Serial.println("Device status sent to Brain");
  } else {
    Serial.println("Failed to send device status: " + String(httpCode));
  }
  
  http.end();
}
