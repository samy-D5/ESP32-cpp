#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

bool loadWifiCredentials(String &ssid, String &password);
void saveWifiCredentials(const String &ssid, const String &password);
bool connectToWifi(const String &ssid, const String &password);
void clearWifiCredentials();
void handleInitialWifiSetup();


#endif