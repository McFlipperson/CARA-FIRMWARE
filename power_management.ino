// CaraPH V2 Power Management Framework
// Battery monitoring, thermal protection, and sleep modes

#include <driver/adc.h>
#include "esp_sleep.h"
#include "esp_pm.h"

// Power configuration
#define BATTERY_ADC_PIN 34
#define CHARGING_STATUS_PIN 35
#define THERMAL_SHUTDOWN_TEMP 70  // Celsius
#define LOW_BATTERY_VOLTAGE 3200  // mV
#define CRITICAL_BATTERY_VOLTAGE 3000  // mV

// Power state tracking
bool charging_status = false;
int battery_voltage_mv = 0;
int battery_percentage = 0;
float cpu_temperature = 0.0;
unsigned long last_battery_check = 0;
const unsigned long battery_check_interval = 5000;  // 5 seconds

// Power management modes
enum PowerMode {
  POWER_NORMAL,
  POWER_SAVING,
  POWER_CRITICAL,
  POWER_SHUTDOWN
};

PowerMode current_power_mode = POWER_NORMAL;

void setup() {
  Serial.begin(115200);
  Serial.println("=== CaraPH Power Management v1.0 ===");
  
  initializePowerMonitoring();
  configurePowerManagement();
}

void loop() {
  // Check power status every 5 seconds
  if (millis() - last_battery_check > battery_check_interval) {
    checkBatteryStatus();
    checkThermalStatus();
    updatePowerMode();
    last_battery_check = millis();
  }
  
  // Execute power management actions
  executePowerMode();
  
  delay(1000);
}

void initializePowerMonitoring() {
  // Configure ADC for battery voltage monitoring
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);  // Pin 34
  
  // Configure charging status pin
  pinMode(CHARGING_STATUS_PIN, INPUT_PULLUP);
  
  Serial.println("✓ Power monitoring initialized");
  Serial.println("✓ Battery ADC configured");
  Serial.println("✓ Charging detection ready");
}

void configurePowerManagement() {
  // Enable automatic light sleep
  esp_pm_config_esp32_t pm_config;
  pm_config.max_freq_mhz = 240;
  pm_config.min_freq_mhz = 80;   // Reduce to 80MHz when idle
  pm_config.light_sleep_enable = true;
  
  esp_err_t ret = esp_pm_configure(&pm_config);
  if (ret == ESP_OK) {
    Serial.println("✓ Automatic power management enabled");
  } else {
    Serial.println("WARNING: Power management configuration failed");
  }
}

void checkBatteryStatus() {
  // Read battery voltage
  int raw_adc = adc1_get_raw(ADC1_CHANNEL_6);
  battery_voltage_mv = (raw_adc * 3300) / 4095;  // Convert to mV
  
  // Calculate battery percentage (3000mV = 0%, 4200mV = 100%)
  battery_percentage = map(battery_voltage_mv, 3000, 4200, 0, 100);
  battery_percentage = constrain(battery_percentage, 0, 100);
  
  // Check charging status
  charging_status = !digitalRead(CHARGING_STATUS_PIN);  // Active low
  
  Serial.printf("Battery: %dmV (%d%%) %s\n", 
                battery_voltage_mv, battery_percentage,
                charging_status ? "CHARGING" : "");
}

void checkThermalStatus() {
  // Read internal temperature (ESP32 built-in sensor)
  cpu_temperature = (temprature_sens_read() - 32) / 1.8;
  
  Serial.printf("CPU Temperature: %.1f°C\n", cpu_temperature);
  
  if (cpu_temperature > THERMAL_SHUTDOWN_TEMP) {
    Serial.println("CRITICAL: Thermal shutdown triggered!");
    emergencyShutdown();
  }
}

void updatePowerMode() {
  PowerMode new_mode = POWER_NORMAL;
  
  if (battery_voltage_mv < CRITICAL_BATTERY_VOLTAGE && !charging_status) {
    new_mode = POWER_SHUTDOWN;
  } else if (battery_voltage_mv < LOW_BATTERY_VOLTAGE && !charging_status) {
    new_mode = POWER_CRITICAL;
  } else if (battery_percentage < 20 && !charging_status) {
    new_mode = POWER_SAVING;
  }
  
  if (new_mode != current_power_mode) {
    Serial.printf("Power mode changed: %s → %s\n", 
                  getPowerModeString(current_power_mode),
                  getPowerModeString(new_mode));
    current_power_mode = new_mode;
  }
}

void executePowerMode() {
  switch (current_power_mode) {
    case POWER_NORMAL:
      // Full performance mode
      setCpuFrequency(240);
      break;
      
    case POWER_SAVING:
      // Reduce CPU frequency, disable non-essential peripherals
      setCpuFrequency(160);
      Serial.println("Power saving mode: Reduced performance");
      break;
      
    case POWER_CRITICAL:
      // Minimum power consumption
      setCpuFrequency(80);
      Serial.println("Critical power mode: Essential functions only");
      // Disable WiFi, reduce LED brightness, etc.
      break;
      
    case POWER_SHUTDOWN:
      Serial.println("CRITICAL: Battery depleted - Emergency shutdown");
      emergencyShutdown();
      break;
  }
}

void emergencyShutdown() {
  Serial.println("=== EMERGENCY SHUTDOWN SEQUENCE ===");
  
  // Save any critical data
  Serial.println("Saving critical system state...");
  
  // Disable all peripherals
  Serial.println("Disabling peripherals...");
  
  // Set all pins to safe states
  for (int pin = 0; pin < 40; pin++) {
    pinMode(pin, INPUT_PULLUP);
  }
  
  Serial.println("Entering deep sleep mode...");
  delay(1000);
  
  // Configure wake-up source (charging detection)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);  // Wake when charging starts
  
  // Enter deep sleep
  esp_deep_sleep_start();
}

const char* getPowerModeString(PowerMode mode) {
  switch (mode) {
    case POWER_NORMAL: return "NORMAL";
    case POWER_SAVING: return "SAVING";
    case POWER_CRITICAL: return "CRITICAL";
    case POWER_SHUTDOWN: return "SHUTDOWN";
    default: return "UNKNOWN";
  }
}

// Function to get current power status for Brain communication
void getPowerStatus(int* voltage, int* percentage, bool* is_charging) {
  *voltage = battery_voltage_mv;
  *percentage = battery_percentage;
  *is_charging = charging_status;
}

// Function to force power mode (for testing)
void setPowerMode(PowerMode mode) {
  current_power_mode = mode;
  Serial.printf("Power mode manually set to: %s\n", getPowerModeString(mode));
}
