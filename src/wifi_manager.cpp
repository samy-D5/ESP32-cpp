#include "wifi_manager.h"
#include <WiFi.h>
#include <Preferences.h>
#include "state_controller.h"
#include "bluetooth_handler.h"
#include <ESP32Ping.h>

Preferences pref;

bool loadWifiCredentials(String &ssid, String &password) {
    pref.begin("wifi", true); //readonly
    ssid = pref.getString("ssid", "");
    password = pref.getString("password","");
    pref.end();
    return (ssid.length() > 0 && password.length() > 0);
}

void saveWifiCredentials(const String &ssid, const String &password) {
    pref.begin("wifi", false);
    pref.putString("ssid", ssid);
    pref.putString("password", password);
    pref.end();
}

void clearWifiCredentials() {
    pref.begin("wifi", false);
    pref.clear();
    pref.end();
}

bool isInternetAvailable() {
    if (WiFi.status() != WL_CONNECTED) return false;
    // pinging Google's DNS server
    return Ping.ping("8.8.8.8", 3);  // 3 attempts
}

bool connectToWifi(const String &ssid, const String &password) {
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Connecting to Wi-Fi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WIFI");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
         // check internet access via ping
        if (isInternetAvailable()) {
            Serial.println("🌐 Internet is available.");
            return true;
        } else {
            Serial.println("🚫 Wi-Fi connected, but NO internet access.");
            WiFi.disconnect();  // disconnect if internet isn't available
            //clearWifiCredentials(); // clear saved credentials
           // Serial.println("❌ Cleared saved Wi-Fi credentials due to no internet access.");
            return false;
        }
    }else {
        Serial.println("Failed to connect to WIFI");
        return false;
    }
}

void handleInitialWifiSetup() {
    String savedSSID, savedPass;
    if (loadWifiCredentials(savedSSID, savedPass)) {
        if (connectToWifi(savedSSID, savedPass)) {
            Serial.println("✅ Wi-Fi Connected with saved credentials.");
            setWaitingForWifiInput(false);
            return;
        }
    }

    Serial.println("📶 No valid Wi-Fi. Waiting for Bluetooth input...");
    setWaitingForWifiInput(true);
    initBluetooth();

     while (isWaitingForWifiInput()) {
        if (SerialBT.available()) {
            String input = SerialBT.readStringUntil('\n');
            input.trim();
            handleBluetoothInput(input);
        }
        delay(500);
    }
}