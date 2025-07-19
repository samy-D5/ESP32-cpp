#include <Arduino.h>
#include "WiFi.h"
#include "wifi_manager.h"
#include "ble_scanner.h"
#include "tcp_client.h"
#include "bluetooth_handler.h"
#include "cmd_processor.h"
#include "state_controller.h"
#include "esp_bt.h" // Add this include for ESP_BT_MODE_CLASSIC_BT

void performBeaconScan();

unsigned long lastScanTime = 0;
const unsigned long scanInterval = 10000; // 10 seconds

const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT); // Release Classic BT memory

  handleInitialWifiSetup();    // 📡 Setup WiFi or wait for BLE credentials
  initBluetooth();             // 🔵 Initialize BLE server
  initBLEScanner();            // 🛰️ Prepare BLE scanning
}

void loop() {
  // 🎯 Handle BLE input from characteristic
  handleBluetoothInput();

  // ✅ Scan and send beacons only if Wi-Fi is connected
  if (shouldScanBeacons() && WiFi.status() == WL_CONNECTED) {
    if (millis() - lastScanTime >= scanInterval) {
      Serial.println("🔍 Scanning for beacons...");
      scanForBeacons();

      const auto& beacons = getCollectedBeacons();
      if (!beacons.empty() && shouldSendBeacons()) {
        sendBeaconDataViaTCP(beacons);
        Serial.println("✅ Beacon data sent.");
      } else {
        Serial.println("⚠️ No beacon data to send.");
      }

      lastScanTime = millis();
    }
  }
}
