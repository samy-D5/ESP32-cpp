#include "ble_scanner.h"
#include "filter_storage.h"
#include "state_controller.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <map>
#include <time.h>


#define SCAN_DURATION 10  // in seconds
#define TELTONIKA_COMPANY_ID 0x089A

static BLEScan* pBLEScan = nullptr;
static std::vector<BeaconData> collectedBeacons;

String getCurrentTimeString() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        char timeStr[16];
        sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        return String(timeStr);
    }
    return String("--:--:--");
}


class TeltonikaAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        // Only process if manufacturer data is present
        if (!advertisedDevice.haveManufacturerData()) return;

        std::string manufacturerData = advertisedDevice.getManufacturerData();
        const uint8_t* data = (uint8_t*)manufacturerData.data();
        size_t length = manufacturerData.length();

        // Need at least 4 bytes: 2 for company ID, 2 for manufacturer data header
        if (length < 4) return;

        // Extract company ID (little endian)
        uint16_t companyId = (data[1] << 8) | data[0];

        // Check for Teltonika company ID
        if (companyId == TELTONIKA_COMPANY_ID) {
            std::string mac = advertisedDevice.getAddress().toString();
            
            BeaconData beacon;
            beacon.mac = mac;
            beacon.rssi = advertisedDevice.getRSSI();
            beacon.rawPayload.assign(data, data + length);
            
            collectedBeacons.push_back(beacon);
            
            Serial.printf("[%s] ✅Teltonika Beacon Found: MAC: %s, RSSI: %d\n", 
                         getCurrentTimeString().c_str(), mac.c_str(), beacon.rssi);
            
            // Print raw manufacturer data for debugging
            Serial.print("Manufacturer Data: ");
            for (size_t i = 0; i < length; i++) {
                Serial.printf("%02X ", data[i]);
            }
            Serial.println();
        }
    }
};

void initBLEScanner() {
    BLEDevice::init("");
    BLEDevice::setPower(ESP_PWR_LVL_P9); // Max power

    // Configure WiFi/BLE coexistence
    esp_ble_scan_params_t scan_params = {
        .scan_type = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval = 0x50,  // 50ms
        .scan_window = 0x50      // 50ms
    };
    esp_ble_gap_set_scan_params(&scan_params);
    
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new TeltonikaAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(50);
    pBLEScan->setWindow(50);
   // pBLEScan->setDuplicateFilter(false); // Important for catching all advertisements
}

void scanForBeacons() {
    collectedBeacons.clear();
    
    Serial.printf("[%s] Starting BLE scan...\n", getCurrentTimeString().c_str());
    BLEScanResults results = pBLEScan->start(SCAN_DURATION, false);
    Serial.printf("[%s] Scan complete. Devices found: %d, Teltonika beacons: %d\n", 
                 getCurrentTimeString().c_str(), results.getCount(), collectedBeacons.size());
    
    pBLEScan->clearResults();
}

const std::vector<BeaconData>& getCollectedBeacons() {
    return collectedBeacons;
}

void clearCollectedBeacons() {
    collectedBeacons.clear();
}