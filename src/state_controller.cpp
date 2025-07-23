#include "state_controller.h"

static bool scanFlag = false;
static bool sendFlag = false;

void setScanFlag() { scanFlag = true; }
void clearScanFlag() { scanFlag = false; }
bool shouldScanBeacons() { return scanFlag; }

void setSendFlag() { sendFlag = true; }
void clearSendFlag() { sendFlag = false; }
bool shouldSendBeacons() { return sendFlag; }

static bool waitingForWifiInput = false;

void setWaitingForWifiInput(bool waiting) {
    waitingForWifiInput = waiting;
}

bool isWaitingForWifiInput() {
    return waitingForWifiInput;
}

static bool sendTriggered = false;

bool hasSendBeenTriggered() {
    return sendTriggered;
}

void setSendTriggered(bool val) {
    sendTriggered = val;
}

bool isFilteringEnabled = false;

void setFilteringEnabled(bool enabled) {
    isFilteringEnabled = enabled;
}

bool getFilteringEnabled() {
    return isFilteringEnabled;
}
