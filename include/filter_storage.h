#pragma once
#include <Arduino.h>
#include <vector>
#include "ble_scanner.h" // defines BeaconData

void loadFiltersFromFlash();
void saveFiltersToFlash();
void addFilter(const String &mac);
void removeFilter(const String &mac);
bool isFiltered(const String &mac);
std::vector<String> getFilters();
void clearFiltersFromFlash();
std::vector<BeaconData> getFilteredBeaconList();
