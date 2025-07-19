#include "bluetooth_handler.h"
#include "wifi_manager.h"
#include "state_controller.h"
#include "cmd_processor.h"   // 👈 Include your new command processor
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

static String ssid = "";
static String password = "";

static bool gotSSID = false;
static bool gotPass = false;

#define SERVICE_UUID        "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define SSID_CHAR_UUID      "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define PASS_CHAR_UUID      "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
#define CMD_CHAR_UUID       "6e400004-b5a3-f393-e0a9-e50e24dcca9e"

class BluetoothCharCallbacks : public BLECharacteristicCallbacks {
public:
    void onWrite(BLECharacteristic *pCharacteristic) override {
        std::string value = pCharacteristic->getValue();
        if (value.empty()) return;

        String input = String(value.c_str());
        input.trim();

        if (!isWaitingForWifiInput() && pCharacteristic->getUUID().toString() != CMD_CHAR_UUID) return;

        if (pCharacteristic->getUUID().toString() == SSID_CHAR_UUID) {
            ssid = input;
            gotSSID = true;
            Serial.println("✅ SSID received.");
        } else if (pCharacteristic->getUUID().toString() == PASS_CHAR_UUID) {
            password = input;
            gotPass = true;
            Serial.println("🔐 Password received.");

            if (gotSSID && gotPass) {
                Serial.println("🔗 Connecting to Wi-Fi...");
                if (connectToWifi(ssid, password)) {
                    saveWifiCredentials(ssid, password);
                    Serial.println("✅ Connected! Credentials saved.");
                    setWaitingForWifiInput(false);
                } else {
                    Serial.println("❌ Failed to connect. Try again.");
                    gotSSID = gotPass = false;
                    ssid = password = "";
                }
            }
        } else if (pCharacteristic->getUUID().toString() == CMD_CHAR_UUID) {
            Serial.println("🛠️ Command received: " + input);
            processCommand(input); // 👈 Call command processor
        }
    }
};

void initBluetooth() {
    BLEDevice::init("ESP32_BLE_SETUP");

    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *ssidChar = pService->createCharacteristic(SSID_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE);
    BLECharacteristic *passChar = pService->createCharacteristic(PASS_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE);
    BLECharacteristic *cmdChar = pService->createCharacteristic(CMD_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE);

    ssidChar->setCallbacks(new BluetoothCharCallbacks());
    passChar->setCallbacks(new BluetoothCharCallbacks());
    cmdChar->setCallbacks(new BluetoothCharCallbacks());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);                    // 👈 Allows it to be seen in mobile BLE scanners
    pAdvertising->setMinPreferred(0x06);                    // Optional: improves Android visibility
    pAdvertising->setMinPreferred(0x12);                    // Optional: improves iOS visibility
    pAdvertising->start();

    Serial.println("🟦 BLE GATT Server started. Use a BLE app to send data.");
}

bool handleBluetoothInput() {
    return false;
}