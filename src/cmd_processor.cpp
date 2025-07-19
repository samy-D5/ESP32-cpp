#include "cmd_processor.h"
#include "wifi_manager.h"
#include "ble_scanner.h"
#include "tcp_client.h"
#include "state_controller.h"
#include "filter_storage.h"

void processCommand(String input)
{
    input.trim();
    input.toLowerCase();

    if (input == "view")
    {
        Serial.println("🔧 Received command: view");
        delay(200);
        setScanFlag();
        Serial.println("🕒 Scan scheduled. Will run shortly...");
        // performBeaconScan();
    }
    else if (input.startsWith("filter "))
    {
        Serial.println("🔧 Received command: " + input);
        String mac = input.substring(7);
        delay(200);
        addFilter(mac);
        Serial.println("✅ Added to filter: " + mac);
    }
    else if (input.startsWith("delete "))
    {
        Serial.println("🔧 Received command: " + input);
        String mac = input.substring(7);
        delay(200);
        removeFilter(mac);
        Serial.println("🗑️ Removed from filter: " + mac);
    }
    else if (input == "send")
    {
        Serial.println("🔧 Received command: send");
        delay(200);
        setSendFlag();
        Serial.println("📤 Preparing to send data...");

        scanForBeacons(); // Trigger scan
        const auto &beacons = getCollectedBeacons();
        if (beacons.empty())
        {
            Serial.println("❌ No beacons found to send.");
        }
        else
        {
            sendBeaconDataViaTCP(beacons);
            Serial.println("✅ Sent beacons to server.");
        }
    }
    else if (input == "reset")
    {
        Serial.println("🔧 Received command: reset");
        delay(200);
        clearFiltersFromFlash();
        clearWifiCredentials();
        Serial.println("🧹 Resetting device...");
        ESP.restart();
    }
    else
    {
        Serial.println("❓ Unknown command: " + input);
    }
}

void performBeaconScan()
{
    scanForBeacons();
    const auto &beacons = getCollectedBeacons();

    if (beacons.empty())
    {
        Serial.println("❌ No beacons found.");
    }
    else
    {
        for (const auto &b : beacons)
        {
            Serial.println("MAC: " + String(b.mac.c_str()) + " RSSI: " + String(b.rssi));
        }
    }
}

void sendFilteredBeacons()
{
    const auto &filtered = getFilteredBeaconList();
    if (filtered.empty())
    {
        Serial.println("⚠️ No filtered beacons to send.");
    }
    else
    {
        sendBeaconDataViaTCP(filtered);
        Serial.println("✅ Filtered beacon data sent.");
    }
}
