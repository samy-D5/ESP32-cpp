#ifndef STATE_CONTROLLER_H
#define STATE_CONTROLLER_H
#pragma once

void setScanFlag();
void clearScanFlag();
bool shouldScanBeacons();

void setSendFlag();
void clearSendFlag();
bool shouldSendBeacons();

void setWaitingForWifiInput(bool waiting);
bool isWaitingForWifiInput();

#endif
