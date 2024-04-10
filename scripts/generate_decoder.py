#!/usr/bin/env python3

#########################################################################################
# Generate LoRaWAN Javascript payload decoder function from Arduino C++ header file
#
# Usage: 
# g++ -dM -E BresserWeatherSensorTTNCfg.h | ~/generate_decoder.py
# or
# g++ -dM -D<ARDUINO_BOARD> -E BresserWeatherSensorTTNCfg.h | ~/generate_decoder.py
#
# The C++ preprocessor is used to evaluate the defines in the header file. 
# Its output is then passed to this script, which creates the decoder function call
# to be copied into the Javascript decoder in the LoRaWAN Network provider's 
# web interface.
#
# created: 02/2023
#
# MIT License
# Copyright (C) 02/2023 Matthias Prinke (https://github.com/matthias-bs)
#
# History:
#
# 20230221 Created
# 20230716 Added lightning sensor data, split status bitmap in status_node and status
#
# To Do:
# - 
#
#########################################################################################
import sys
import re

# List of known features
features = [
    'SENSORID_EN',
    'ONEWIRE_EN',
    'SLEEP_EN',
    'THEENGSDECODER_EN',
    'RAINDATA_EN',
    'SOILSENSOR_EN',
    'MITHERMOMETER_EN',
    'DISTANCESENSOR_EN',
    'LIGHTNINGSENSOR_EN',
    'ADC_EN',
    'PIN_ADC_IN',
    'PIN_ADC0_IN',
    'PIN_ADC1_IN',
    'PIN_ADC2_IN',
    'PIN_ADC3_IN',
]

# Javascript decoder generator
# 
# The position in the dictionary below must match the order of assignments to the
# encoder object in doUplink() (BresserWeatherSensorTTN.ino).
# 
# The position in the dictionary then defines the order in the output!  
#
# Format:
#   <key> : {'cond': <condition>, 'type': <datatype>}
#
#   <key>       : JSON output key name
#   <condition> : feature which must be present in input or '' if unconditional
#   <datatype>  : decoder datatype (or rather name of decoding function)
generator = {
    'id' : {'cond': 'SENSORID_EN', 'type': 'uint32'},
    'status_node' : {'cond': '', 'type': 'bitmap'},
    'status' : {'cond': '', 'type': 'bitmap'},
    'air_temp_c': {'cond': '', 'type': 'temperature'},
    'humidity': {'cond': '', 'type': 'uint8'},
    'wind_gust_meter_sec': {'cond': '', 'type': 'uint16fp1'},
    'wind_avg_meter_sec': {'cond': '', 'type': 'uint16fp1'}, 
    'wind_direction_deg': {'cond': '', 'type': 'uint16fp1'},
    'rain_mm': {'cond': '', 'type': 'rawfloat'},
    'supply_v': {'cond': 'ADC_EN', 'type': 'uint16'},
    'battery_v': {'cond': 'PIN_ADC3_IN', 'type': 'uint16'},
    'water_temp_c': {'cond': 'ONEWIRE_EN', 'type': 'temperature'},
    'indoor_temp_c': {'cond': 'THEENGSDECODER_EN', 'type': 'temperature'},
    'indoor_humidity': {'cond': 'THEENGSDECODER_EN', 'type': 'uint8'},
    'indoor_temp_c': {'cond': 'MITHERMOMETER_EN', 'type': 'temperature'},
    'indoor_humidity': {'cond': 'MITHERMOMETER_EN', 'type': 'uint8'},
    'soil_temp_c': {'cond': 'SOILSENSOR_EN', 'type': 'temperature'}, 
    'soil_moisture': {'cond': 'SOILSENSOR_EN', 'type': 'uint8'},
    'rain_hr': {'cond': 'RAINDATA_EN', 'type': 'rawfloat'},
    'rain_day': {'cond': 'RAINDATA_EN', 'type': 'rawfloat'},
    'rain_week': {'cond': 'RAINDATA_EN', 'type': 'rawfloat'},
    'rain_mon': {'cond': 'RAINDATA_EN', 'type': 'rawfloat'},
    'adc0_v': {'cond': 'PIN_ADC0_IN', 'type': 'uint16'},
    'adc1_v': {'cond': 'PIN_ADC1_IN', 'type': 'uint16'},
    'adc2_v': {'cond': 'PIN_ADC2_IN', 'type': 'uint16'},
    'distance_mm': {'cond': 'DISTANCESENSOR_EN', 'type': 'uint16'},
    'lightning_time': {'cond': 'LIGHTNINGSENSOR_EN', 'type': 'unixtime'},
    'lightning_count': {'cond': 'LIGHTNINGSENSOR_EN', 'type': 'uint16'},
    'lightning_distance_km': {'cond': 'LIGHTNINGSENSOR_EN', 'type': 'uint8'},
}

header = '''
    return decode(
        bytes,
'''[1:-1]

types = ' ' * 8 + '[\n'

keys  = ' ' * 8 + '[\n'

footer = '''
    );
'''[1:-1]

# Process input from C-preprocessor 
list=[]
for line in sys.stdin:
    line = line.removeprefix('#define')
    
    # remove leading whitespaces
    line = line.lstrip()
    
    # remove trailing whitespaces
    line = line.strip()
    
    # remove whitespaces followed by non-whitespaces at end of line
    # e.g. "PIN_ADC0_IN A3" -> PIN_ADC0_IN
    line = re.sub(r'\s+\S+$', '', line)
    
    # Add all known features from input to the list
    if line in features:
        list.append(line)

# Print the list for debugging
#print(list)

# Generate the output
for k,v in generator.items():
    if v['cond'] == '' or v['cond'] in list:
        types += ' ' * 12 + v['type'] + ',\n'
        keys  += ' ' * 12 + k + ',\n'

# Print the result
print(header)
print(types)
print(' ' * 8 + '],')
print(keys)
print(footer)
