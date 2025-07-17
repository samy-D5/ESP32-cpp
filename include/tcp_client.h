#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "ble_scanner.h"

void sendBeaconDataViaTCP(const std::vector<BeaconData>& beacons);

#endif
