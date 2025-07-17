#include "reset_handler.h"
#include <Arduino.h>
#include <esp_system.h>

void resetDevice()
{
    ESP.restart();
}
