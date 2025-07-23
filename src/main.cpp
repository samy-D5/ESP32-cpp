#include <Arduino.h>
#include "WiFi.h"
#include "wifi_manager.h"
#include "ble_scanner.h"
#include "tcp_client.h"
#include "bluetooth_handler.h"
#include "cmd_processor.h"
#include "state_controller.h"
#include "filter_storage.h"
#include <time.h>
//#include "esp_wifi.h" // <-- Add this include for esp_wifi_set_ps

bool handleBluetoothInput();
bool isWaitingForWifiInput();
extern BluetoothSerial SerialBt;
void performBeaconScan();
void initBluetooth();
void initBLEScanner();

unsigned long lastScanTime = 0;
unsigned long lastSendTime = 0;
//const unsigned long scanInterval = 3000; // scan every 3s
const unsigned long sendInterval = 10000; // send every 10s


void setup() {
  Serial.begin(115200);
  delay(200);

  
  WiFi.setSleep(true);  // Enable WiFi modem sleep (reduces interference)
  handleInitialWifiSetup();         // ✅ Connects or enables Bluetooth for credentials
  //esp_wifi_set_ps(WIFI_PS_NONE);  // ✅ Improves BLE reliability
  loadFiltersFromFlash();           // ✅ Load any saved filters
  initBluetooth();                  // ✅ Keep Bluetooth ON always
  initBLEScanner();                 // ✅ Ready to scan if Wi-Fi is connected
  
  setScanFlag();  // ✅ <--- Automatically start scanning
  //setSendFlag();  // ✅ <--- Automatically prepare to send data
  //setFilteringEnabled(true); // ✅ Enable filtering by default
  configTime(19800, 0, "pool.ntp.org"); // set Timezone to IST (GMT+5:30)

  // wait for time to be set
    struct tm timeinfo; 
    while (!getLocalTime(&timeinfo)) {
        Serial.println("⏳ Waiting for NTP time...");
        delay(500);
    }
}

void loop() {

  // ✅ Handle ALL Bluetooth input in one go
  if (SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();

    if (isWaitingForWifiInput()) {
      if (handleBluetoothInput(input)) {
        Serial.println("🔵 Bluetooth input handled.");
      } else {
        Serial.println("❌ Invalid Bluetooth input.");
      }
    } else {
      if (!processBluetoothCommand(input)) {
        SerialBT.println("❓ Unknown command. Try view, register <mac>, delete <mac>, send, reset");
      }
    }
    return;  // Skip rest of loop until command fully processed
  }

  // ✅ Scan BLE and send data only if Wi-Fi is connected and not waiting
  if (shouldScanBeacons() && WiFi.status() == WL_CONNECTED) {
    unsigned long currentMillis = millis();
     // Scan frequently (every 3 seconds)
    // if (currentMillis - lastScanTime >= scanInterval) {
    //   lastScanTime = currentMillis;
    // }
    // Send accumulated data every 10 seconds
    if (currentMillis - lastSendTime >= sendInterval) {
      Serial.println("🔍 Scanning for beacons...");
      scanForBeacons();  // appends Teltonika beacons to collectedBeacons
      const auto& beacons = getCollectedBeacons();
      if (!beacons.empty() && hasSendBeenTriggered()) {
        Serial.println("✅ All Teltonika Beacons:");
        for (const auto& b : beacons) {
            Serial.println("MAC: " + String(b.mac.c_str()) + " RSSI: " + String(b.rssi));
        }
            const auto& filtered = getFilteredBeaconList();
            if (!filtered.empty()) {
                Serial.println("📤 Filtered Beacons:");
                for (const auto &b : filtered) {
                    Serial.println("MAC: " + String(b.mac.c_str()) + " RSSI: " + String(b.rssi));
                }
                sendBeaconDataViaTCP(filtered);
              } else {
                Serial.println("⚠️ No filtered beacons found.");
              }
              clearCollectedBeacons();
              lastSendTime = currentMillis;
          }
       }
  }
}
