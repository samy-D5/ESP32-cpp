#include <Arduino.h>
#include "filter_storage.h"
#include <Preferences.h>
#include "ble_scanner.h"

Preferences prefs;
std::vector<String> filters;

void loadFiltersFromFlash() {
    filters.clear();
    prefs.begin("beacon-filters", true);
    int count = prefs.getUInt("count", 0);

    for (int i = 0; i < count; ++i) {
        String key = "f" + String(i);
        String mac = prefs.getString(key.c_str(), "");
        if (mac.length() > 0) {
            filters.push_back(mac.c_str());
        }
    }
    prefs.end();
}

void saveFiltersToFlash() {
    prefs.begin("beacon-filters", false); //write-mode

    int oldCount = prefs.getUInt("count", 0); //get previous count to clean up leftovers
    prefs.putUInt("count", filters.size()); // store the number of filters

    for (int i = 0; i < filters.size(); ++i) {
        String key = "f" + String(i);
        prefs.putString(key.c_str(), filters[i].c_str());
    }
    //delete leftover keys from old data
    for (int i = filters.size(); i < oldCount; ++i) {
        String key = "f" + String(i);
        prefs.remove(key.c_str());
    }
    prefs.end();
}

void addFilter(const String& mac) {
    for (const auto& f : filters) {
        if (f == mac) return;
    }
    filters.push_back(mac);
    saveFiltersToFlash();
}

void removeFilter(const String& mac) {
    filters.erase(std::remove(filters.begin(), filters.end(), mac), filters.end());
    saveFiltersToFlash();
}

bool isFiltered(const String& mac) {
    for (const auto& f : filters) {
        if (f.equalsIgnoreCase(mac)) return true;
    }
    return false;
}

std::vector<String> getFilters() {
    return filters;
}

void clearFiltersFromFlash() {
    prefs.begin("beacon-filters", false);
    prefs.clear();
    prefs.end();
    filters.clear();
}

std::vector<BeaconData> getFilteredBeaconList() {
    std::vector<BeaconData> filtered;
for (const auto& b : getCollectedBeacons()) {
    Serial.printf("🔍 Checking MAC %s against register list\n", b.mac.c_str());
    if (isFiltered(b.mac.c_str())) {
        Serial.printf("✅ Match found for MAC: %s\n", b.mac.c_str());
        filtered.push_back(b);
    }
}
    return filtered;
}
