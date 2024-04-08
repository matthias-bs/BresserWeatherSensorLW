# BresserWeatherSensorLW

[![CI](https://github.com/matthias-bs/BresserWeatherSensorLW/actions/workflows/CI.yml/badge.svg)](https://github.com/matthias-bs/BresserWeatherSensorLW/actions/workflows/CI.yml)
[![GitHub release](https://img.shields.io/github/release/matthias-bs/BresserWeatherSensorLW?maxAge=3600)](https://github.com/matthias-bs/BresserWeatherSensorLW/releases)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](https://github.com/matthias-bs/BresserWeatherSensorLW/blob/main/LICENSE)

Bresser 868 MHz Weather Sensor Radio Receiver based on ESP32/RP2040 and SX1262/SX1276 &mdash; sends data to a LoRaWAN Network

This is a remake of [BresserWeatherSensorTTN](https://github.com/matthias-bs/BresserWeatherSensorTTN) based on [RadioLib](https://github.com/jgromes/RadioLib) instead of [MCCI Arduino LoRaWAN Library](https://github.com/mcci-catena/arduino-lorawan) for LoRaWAN communication.

This is early stage of development - stay tuned.

* [x] Weather sensor data reception
* [x] BLE sensor data reception
* [x] LoRaWAN join
* [x] LoRaWAN data uplink
* [x] LoRaWAN network time request & RTC setting
* [x] Energy saving / battery deep-discharge protection
* [x] Wakeup time adjustment
* [x] LoRaWAN control downlink
* [x] LoRaWAN status uplink
* [x] Periodic LinkCheck
* [x] Node/network status/debug information
* [x] setDeviceStatus(battLevel)
* [ ] Log messages
* [ ] Supported ESP32 boards
* [ ] RP2040 specific implementation

## Features
* Single 868 MHz Radio Transceiver for both Sensor Data Reception and LoRaWAN Connection
* Compatible to LoRaWAN Specification 1.1.0 / RP001 Regional Parameters 1.1 revision A
* Tested with [The Things Network](https://www.thethingsnetwork.org/)
* Supports multiple 868 MHz Sensors (e.g. Weather Sensor and Soil Moisture Sensor or Indoor Thermometer/Hygrometer)
* See [BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver) for supported sensors
* Low Power Design (using ESP32 Deep Sleep Mode / RP2040 Sleep State)
* Fast LoRaWAN Joining after Deep Sleep (using ESP32 RTC RAM / RP2040 Flash)
* [ATC MiThermometer](https://github.com/pvvx/ATC_MiThermometer) Bluetooth Low Energy Thermometer/Hygrometer Integration (optional)
* [Theengs Decoder](https://github.com/theengs/decoder) Bluetooth Low Energy Sensors Integration (optional)
* OneWire Temperature Sensor Integration (optional)
* ESP32/RP2040 Analog Digital Converter Integration (optional)
* [A02YYUW / DFRobot SEN0311 Ultrasonic Distance Sensor](https://wiki.dfrobot.com/_A02YYUW_Waterproof_Ultrasonic_Sensor_SKU_SEN0311) (30...4500mm) (optional)
* [Remote Configuration via LoRaWAN Downlink](https://github.com/matthias-bs/BresserWeatherSensorTTN/blob/main/README.md#remote-configuration-via-lorawan-downlink)