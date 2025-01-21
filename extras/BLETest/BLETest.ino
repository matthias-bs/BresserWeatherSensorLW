#include <Arduino.h>
#include "BleSensors.h"

// List of known sensors' BLE addresses
#define KNOWN_BLE_ADDRESSES \
     {                       \
         "a4:c1:38:b8:1f:7f" \
     }

std::vector<std::string> knownBLEAddresses;

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("BLETest");

    knownBLEAddresses = KNOWN_BLE_ADDRESSES;
    BleSensors bleSensors = BleSensors(knownBLEAddresses);
    unsigned ble_scantime = 31;
    bool ble_active = false;

    for (const std::string &s : knownBLEAddresses)
    {
        (void)s;
        log_d("%s", s.c_str());
    }

    bleSensors.getData(ble_scantime, ble_active);
}

void loop()
{
    delay(100);
}
