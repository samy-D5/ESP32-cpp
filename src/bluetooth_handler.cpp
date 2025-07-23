#include "bluetooth_handler.h"
#include "wifi_manager.h"
#include "state_controller.h"
#include "WiFi.h"
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

static String ssid, password;
static bool gotSSID = false, gotPass = false;

void initBluetooth() {
    SerialBT.begin("ESP32_BLUETOOTH");
    SerialBT.println("🔵 Bluetooth initialized.");
}

bool handleBluetoothInput(const String &input) {
    if (input.equalsIgnoreCase("RESET")) {
        SerialBT.println("📴 Clearing Wi-Fi credentials...");
        clearWifiCredentials();
        delay(1000);
        ESP.restart();
    }

    else if (!gotSSID) {
        ssid = input;
        SerialBT.println("➡️ Now send the Password:");
        gotSSID = true;
    } 
    else if (!gotPass) {
        password = input;
        gotPass = true;

        SerialBT.println("🔗 Connecting to Wi-Fi...");

        if (connectToWifi(ssid, password)) {
            saveWifiCredentials(ssid, password);
            SerialBT.println("✅ Connected! Credentials saved.");
            setWaitingForWifiInput(false);
        } else {
            SerialBT.println("❌ Failed to connect. Try again.");
            gotSSID = gotPass = false;
            ssid = password = "";
        }
    }
    return true;
}
