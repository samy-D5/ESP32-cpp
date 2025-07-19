#include "ble_scanner.h"
#include  "filter_storage.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <map>

// === Constants ===
#define SCAN_DURATION 2 // in seconds
#define TELTONIKA_COMPANY_ID 0x089A

// === Globals ===
static BLEScan* bleScanner = nullptr;
static std::map<std::string, bool> seenBeacons;
static std::vector<BeaconData> collectedBeacons;
//static std::vector<String> macFilterList;

// === Beacon Callback Class ===
class BeaconScanCallback : public BLEAdvertisedDeviceCallbacks {
public:
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        std::string mac = advertisedDevice.getAddress().toString();

        // Skip duplicates
        if (seenBeacons.count(mac)) return;

        const uint8_t* payload = advertisedDevice.getPayload();
        int length = advertisedDevice.getPayloadLength();

        // Parse advertisement fields
        int index = 0;
        while (index < length) {
            uint8_t fieldLen = payload[index];
            if (fieldLen == 0 || (index + fieldLen+1) > length) break;

            uint8_t adType = payload[index + 1];

            // Check for manufacturer-specific data (0xFF)
            if (adType == 0xFF && fieldLen >= 4) {
                uint16_t companyId = payload[index + 3] << 8 | payload[index + 2];

                if (companyId == TELTONIKA_COMPANY_ID) {
                    seenBeacons[mac] = true;

                    BeaconData beacon;
                    beacon.mac = mac;
                    beacon.rssi = advertisedDevice.getRSSI();
                    beacon.rawPayload.assign(payload, payload + length);

                    collectedBeacons.push_back(beacon);

                    Serial.println("✅ Teltonika EYE Beacon Found!");
                    Serial.printf("MAC: %s, RSSI: %d\n", mac.c_str(), beacon.rssi);
                }
                break;
            }

            index += fieldLen + 1;
        }
    }
};

// === Initialization ===
void initBLEScanner() {
    BLEDevice::init("");
    bleScanner = BLEDevice::getScan();
    bleScanner->setAdvertisedDeviceCallbacks(new BeaconScanCallback(), false);
    bleScanner->setActiveScan(false);
    bleScanner->setInterval(100);
    bleScanner->setWindow(99);
}

// === Start Scan ===
void scanForBeacons() {
    seenBeacons.clear();
    collectedBeacons.clear();

    Serial.println("🔍 Starting BLE scan...");
    BLEScanResults results = bleScanner->start(SCAN_DURATION, false);
    Serial.printf("🔎 Scan complete. Devices found: %d\n", results.getCount());

    bleScanner->clearResults();
    bleScanner->stop();
}

// === Accessor ===
const std::vector<BeaconData>& getCollectedBeacons() {
    return collectedBeacons;
}

