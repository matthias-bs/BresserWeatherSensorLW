///////////////////////////////////////////////////////////////////////////////
// LoadNodeCfg.cpp
//
// Load LoRaWAN node configuration 'node_config.json' from LittleFS, if available
//
// This configuration file is intended for hardware/deployment environment
// specific settings (e.g. battery voltage levels, timezone)
//
// created: 07/2024
//
//
// MIT License
//
// Copyright (c) 2024 Matthias Prinke
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// History:
//
// 20240725 Created
// 20240729 Added PowerFeather specific configuration
// 20240804 Added max_charge_current
// 20241110 Fixed ArduinoJson deprecation warning
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "LoadNodeCfg.h"

// Load LoRaWAN node configuration 'node_config.json' from LittleFS, if available
void loadNodeCfg(
    String &tzinfo,
    uint16_t &batt_weak,
    uint16_t &batt_low,
    uint16_t &batt_discharge_lim,
    uint16_t &batt_charge_lim,
    struct sPowerFeatherCfg &powerFeatherCfg)
{


    if (!LittleFS.begin(
#if defined(ESP32)
            // Format the LittleFS partition on error; parameter only available for ESP32
            true
#endif
            ))
    {
        log_d("Could not initialize LittleFS.");
    }
    else
    {
        File file = LittleFS.open("/node_config.json", "r");

        if (!file)
        {
            log_i("File 'node_config.json' not found.");
        }
        else
        {
            log_d("Reading 'node_config.json'");
            JsonDocument doc;

            // Deserialize the JSON document
            DeserializationError error = deserializeJson(doc, file);
            if (error)
            {
                log_d("Failed to read JSON file, using defaults.");
            }
            else
            {
                if (!doc["timezone"].isNull())
                    tzinfo = doc["timezone"].as<String>();
                if (!doc["battery_weak"].isNull())
                    batt_weak = doc["battery_weak"];
                if (!doc["battery_low"].isNull())
                    batt_low = doc["battery_low"];
                if (!doc["battery_discharge_lim"].isNull())
                    batt_discharge_lim = doc["battery_discharge_lim"];
                if (!doc["battery_charge_lim"].isNull())
                    batt_charge_lim = doc["battery_charge_lim"];
                if (!doc["powerfeather"].isNull()) {
                    JsonObject pf = doc["powerfeather"];
                    if (!pf["battery_capacity"].isNull()) {
                        powerFeatherCfg.battery_capacity = pf["battery_capacity"];
                    }
                    if (!pf["supply_maintain_voltage"].isNull()) {
                        powerFeatherCfg.supply_maintain_voltage = pf["supply_maintain_voltage"];
                    }
                    if (!pf["max_charge_current"].isNull()) {
                        powerFeatherCfg.max_charge_current = pf["max_charge_current"];
                    }
                    if (!pf["temperature_measurement"].isNull()) {
                        powerFeatherCfg.temperature_measurement = pf["temperature_measurement"];
                    }
                    if (!pf["battery_fuel_gauge"].isNull()) {
                        powerFeatherCfg.battery_fuel_gauge = pf["battery_fuel_gauge"];
                    }
                }
            } // deserializeJson o.k.
        } // file read o.k.
        file.close();
    } // LittleFS o.k.

    log_d("Timezone: %s", tzinfo.c_str());
    log_d("Battery weak:            %4d mV", batt_weak);
    log_d("Battery low:             %4d mV", batt_low);
    log_d("Battery discharge limit: %4d mV", batt_discharge_lim);
    log_d("Battery charge limit:    %4d mV", batt_charge_lim);
    log_d("PowerFeather");
    log_d("  Battery capacity:        %4d mAh", powerFeatherCfg.battery_capacity);
    log_d("  Supply maintain voltage: %4d mV", powerFeatherCfg.supply_maintain_voltage);
    log_d("  Max. charge current:     %4d mA", powerFeatherCfg.max_charge_current);
    log_d("  Temperature measurement: %s", powerFeatherCfg.temperature_measurement ? "true" : "false");
    log_d("  Battery fuel gauge:      %s", powerFeatherCfg.battery_fuel_gauge ? "true" : "false");
} // loadNodeCfg()