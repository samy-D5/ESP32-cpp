#ifndef BLUETOOTH_HANDLER_H
#define BLUETOOTH_HANDLER_H

#include <BluetoothSerial.h>
#include <vector>
#include "ble_scanner.h"

extern BluetoothSerial SerialBT;

void initBluetooth();
bool handleBluetoothInput(const String &input);
bool isWaitingForWifiInput();

#endif
