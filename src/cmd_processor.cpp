#include <Arduino.h>
#include "cmd_processor.h"
#include "wifi_manager.h"
#include "ble_scanner.h"
#include "tcp_client.h"
#include "state_controller.h"
#include "bluetooth_handler.h"
#include "filter_storage.h"

extern BluetoothSerial SerialBT;

bool processBluetoothCommand(const String &input)
{
    if (input == "view")
    {
        Serial.println("🔧 Received command: " + input);
        setScanFlag();
        SerialBT.println("🔍 Scanning for beacons...");
        return true;
    }
    else if (input.startsWith("filter "))
    {
        Serial.println("🔧 Received command: " + input);
        String mac = input.substring(7);
        addFilter(String(mac.c_str()));
        SerialBT.println("✅ Added to filter: " + mac);
        return true;
    }
    else if (input.startsWith("delete"))
    {
        Serial.println("🔧 Received command: " + input);
        String mac = input.substring(7);
        removeFilter(String(mac.c_str()));
        SerialBT.println("🗑️ Removed from filter: " + mac);
        return true;
    }
    else if (input == "send")
    {
        Serial.println("🔧 Received command: " + input);
        setSendFlag();
        SerialBT.println("📤 Preparing to send data...");

        // 🔁 Trigger scan and send immediately
        scanForBeacons(); // Reuse your existing scanner
        const auto &beacons = getCollectedBeacons();
        if (beacons.empty()){
            SerialBT.println("❌ No beacons found to send.");
        }
        else{
            sendBeaconDataViaTCP(beacons);
            SerialBT.println("✅ Sent beacons to server.");
        }
        return true;
    }
    else if (input == "reset")
    {
        Serial.println("🔧 Received command: " + input);
        clearFiltersFromFlash();
        clearWifiCredentials();
        SerialBT.println("🧹 Resetting device...");
        ESP.restart();
    }
    return false;
}

void performBeaconScan()
{
    scanForBeacons();
    const auto &beacons = getCollectedBeacons();

    if (beacons.empty())
    {
        SerialBT.println("❌ No beacons found.");
    }
    else
    {
        for (const auto &b : beacons)
        {
            SerialBT.println("MAC: " + String(b.mac.c_str()) + " RSSI: " + String(b.rssi));
        }
    }
}

void sendFilteredBeacons()
{
    const auto &filtered = getFilteredBeaconList();
    if (filtered.empty())
    {
        SerialBT.println("⚠️ No filtered beacons to send.");
    }
    else
    {
        sendBeaconDataViaTCP(filtered);
    }
}
