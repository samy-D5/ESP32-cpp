#ifndef BLE_SCANNER_H
#define BLE_SCANNER_H
#pragma once

#include <Arduino.h>
#include <vector>

struct BeaconData
{
    std::string mac;
    int rssi;
    std::vector<uint8_t> rawPayload;
};

void initBLEScanner();
void scanForBeacons();
const std::vector<BeaconData> &getCollectedBeacons();

#endif
