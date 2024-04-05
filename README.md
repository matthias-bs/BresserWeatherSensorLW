# BresserWeatherSensorLW

Bresser 868 MHz Weather Sensor Radio Receiver based on ESP32/RP2040 and SX1262/SX1276 - sends data to a LoRaWAN Network

This is a remake of [BresserWeatherSensorTTN](https://github.com/matthias-bs/BresserWeatherSensorTTN) based on [RadioLib](https://github.com/jgromes/RadioLib) instead of [MCCI Arduino LoRaWAN Library](https://github.com/mcci-catena/arduino-lorawan) for LoRaWAN communication.

This is early stage of development - stay tuned.

* [x] Weather sensor data reception
* [x] BLE sensor data reception
* [x] LoRaWAN join
* [x] LoRaWAN data uplink
* [ ] LoRaWAN network time request & RTC setting
* [ ] Wakeup time adjustment
* [ ] LoRaWAN control downlink
* [ ] LoRaWAN status uplink
* [ ] RP2040 specific implementation
