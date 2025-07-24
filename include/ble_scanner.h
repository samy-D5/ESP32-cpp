#ifndef BLE_SCANNER_H
#define BLE_SCANNER_H
#pragma once

#include <Arduino.h>
#include <vector>

#define SEND_INTERVAL 10000  // milliseconds (10 seconds)
#define DIFF 2000            // milliseconds (2 seconds) for connection stability
#define SCAN_DURATION (SEND_INTERVAL - DIFF) / 1000  // convert to seconds

struct BeaconData
{
    std::string mac;
    int rssi;
    std::vector<uint8_t> rawPayload;
};

void initBLEScanner();
void scanForBeacons();
const std::vector<BeaconData> &getCollectedBeacons();
void clearCollectedBeacons();

#endif
