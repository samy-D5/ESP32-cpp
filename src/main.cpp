#include <Arduino.h>
#include "WiFi.h"
#include "wifi_manager.h"
#include "ble_scanner.h"
#include "tcp_client.h"
#include "bluetooth_handler.h"
#include "cmd_processor.h"
#include "state_controller.h"

// Forward declaration if not included from a header
bool handleBluetoothInput();
bool isWaitingForWifiInput();
extern BluetoothSerial SerialBt;
void performBeaconScan();
void initBluetooth();
void initBLEScanner();

unsigned long lastScanTime = 0;
const unsigned long scanInterval = 10000; // in milliseconds (10 sec)

const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  handleInitialWifiSetup();         // ✅ Connects or enables Bluetooth for credentials
  initBluetooth();                  // ✅ Keep Bluetooth ON always
  initBLEScanner();                 // ✅ Ready to scan if Wi-Fi is connected
}

void loop() {

  // ✅ All Bluetooth handling (SSID/pass or commands) inside this function
  if (SerialBT.available()) {
    if (handleBluetoothInput()) {
      Serial.println("🔵 Bluetooth input handled.");
    } else {
      Serial.println("❓ No Bluetooth input to handle.");
    }
    return;
  }

  // ✅ Optional: Only allow advanced commands if Wi-Fi credentials are done
  if (!isWaitingForWifiInput() && SerialBT.available()) {
    String input = SerialBT.readStringUntil('\n');
    input.trim();

    if (!processBluetoothCommand(input) && input.length() > 0) {
      SerialBT.println("❓ Unknown command. Try VIEW, FILTER <mac>, DELETE <mac>, SEND, RESET");
    }
  }

  // ✅ Scan BLE and send data only if Wi-Fi is connected and not waiting
  if (shouldScanBeacons() && WiFi.status() == WL_CONNECTED) {
    if (millis() - lastScanTime >= scanInterval) {
      Serial.println("🔍 Scanning for beacons...");
      scanForBeacons();

      const auto& beacons = getCollectedBeacons();
      if (!beacons.empty() && shouldSendBeacons()) {
        sendBeaconDataViaTCP(beacons);
      }

      lastScanTime = millis();
    }
  }
}
