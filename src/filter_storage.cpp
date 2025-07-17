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
    prefs.begin("beacon-filters", false);
    prefs.putUInt("count", filters.size());

    for (int i = 0; i < filters.size(); ++i) {
        String key = "f" + String(i);
        prefs.putString(key.c_str(), filters[i].c_str());
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
        if (f == mac) return true;
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
        if (isFiltered(b.mac.c_str())) {
            filtered.push_back(b);
        }
    }
    return filtered;
}
