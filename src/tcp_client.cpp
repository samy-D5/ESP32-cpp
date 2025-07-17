#include "tcp_client.h"
#include <WiFi.h>

const char *SERVER_IP = "192.168.0.114"; 
const uint16_t SERVER_PORT = 5000;

void sendBeaconDataViaTCP(const std::vector<BeaconData> &beacons)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("❌ Not connected to Wi-Fi");
        return;
    }

    WiFiClient client;
    if (!client.connect(SERVER_IP, SERVER_PORT))
    {
        Serial.println("❌ TCP Connection Failed");
        return;
    }
    delay(100); // Gives some time for connection to stabilize

    for (const auto &b : beacons)
    {
        String message = "MAC: " + String(b.mac.c_str()) + ", ";
        message += "RSSI: " + String(b.rssi) + ", ";
        message += "RAW: ";

        for (size_t i = 0; i < b.rawPayload.size(); ++i)
        {
            char hex[4];
            sprintf(hex, "%02X", b.rawPayload[i]);
            message += hex;
            if (i < b.rawPayload.size() - 1)
                message += " ";
        }
        message += "\n";

        client.print(message); // Send to server
        client.flush(); //ensure data is sent immediately
        delay(100); //small delay to avoid flodding
        Serial.print("📤 Sent to TCP server: ");
        Serial.print(message);
    }

    client.stop();
    Serial.println("🔌 TCP connection closed.");
}
