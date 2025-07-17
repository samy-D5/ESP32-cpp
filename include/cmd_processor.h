#include <WString.h>
#ifndef CMD_PROCESSOR_H
#define CMD_PROCESSOR_H

bool processBluetoothCommand(const String& input);
void performBeaconScan();
void sendFilteredBeacons();

#endif
