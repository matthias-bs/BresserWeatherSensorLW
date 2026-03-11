# Board Pin Definitions

This file groups radio, sensor, ADC and GPS pin defines per board (sourced from `config.h` and `BresserWeatherSensorLWCfg.h`). Each section is a per-board reference table.

## ARDUINO_FEATHER_ESP32 / ARDUINO_THINGPULSE_EPULSE_FEATHER

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 14 |
| Radio - RESET | `PIN_LORA_RST` | 27 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 32 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 33 |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |
| ADC input (battery) | `PIN_ADC_IN` | A13 |
| OneWire bus | `PIN_ONEWIRE_BUS` | 15 |

> **Note:** Required wiring: A to RST, B to DIO1, D to DIO0, E to CS

## ARDUINO_ADAFRUIT_FEATHER_ESP32S2

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 6 |
| Radio - RESET | `PIN_LORA_RST` | 9 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 5 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 11 |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |
| OneWire bus | `PIN_ONEWIRE_BUS` | 15 |
| ADC input (battery) | `PIN_ADC_IN` | -1 (external divider required) |
> **Note:** Required wiring: A to RST, B to DIO1, D to DIO0, E to CS

## ARDUINO_ADAFRUIT_FEATHER_ESP32_V2

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 14 |
| Radio - RESET | `PIN_LORA_RST` | 27 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 32 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 33 |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |

## ARDUINO_ESP32S3_POWERFEATHER

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 15 |
| Radio - RESET | `PIN_LORA_RST` | 45 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 16 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 18 |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |
| ADC input (battery) | `PIN_ADC_IN` | -1 (unused) |
| OneWire bus | `PIN_ONEWIRE_BUS` | 17 |
| A02YYUW TX | `A02YYUW_TX` | TX (symbol, pin 44) |
| A02YYUW RX | `A02YYUW_RX` | RX (symbol, pin 42) |
| A02YYUW PWR | `A02YYUW_PWR` | 1 |
| DYP-R01CW SDA | `DYP_R01CW_SDA` | SDA (35) |
| DYP-R01CW SCL | `DYP_R01CW_SCL` | SCL (36) |

> **Note:** Required wiring: A to RST, B to DIO1, D to DIO0, E to CS

## ARDUINO_ADAFRUIT_FEATHER_RP2040

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 7 |
| Radio - RESET | `PIN_LORA_RST` | 11 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 8 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 10 |
| Radio chip | `USE_SX1276` | SX1276 selected |
| ADC input (battery) | `PIN_ADC_IN` | A0 |
| OneWire bus | `PIN_ONEWIRE_BUS` | 6 |
| A02YYUW TX | `A02YYUW_TX` | 0 |
| A02YYUW RX | `A02YYUW_RX` | 1 |
| A02YYUW PWR | `A02YYUW_PWR` | 7 |
| DYP-R01CW SDA | `DYP_R01CW_SDA` | 2 |
| DYP-R01CW SCL | `DYP_R01CW_SCL` | 3 |

> **Note:** Required wiring: A to RST, B to DIO1, D to DIO0, E to CS

## ARDUINO_TTGO_LORA32_V1 / ARDUINO_TTGO_LoRa32_V1

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | `LORA_CS` (board symbol) |
| Radio - RESET | `PIN_LORA_RST` | `LORA_RST` |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | `LORA_IRQ` |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 33 |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |
| ADC input (battery) | `PIN_ADC_IN` | 35 |
| OneWire bus | `PIN_ONEWIRE_BUS` | 21 |

## ARDUINO_TTGO_LORA32_V2 / ARDUINO_TTGO_LoRa32_V2

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | `LORA_CS` |
| Radio - RESET | `PIN_LORA_RST` | `LORA_RST` |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | `LORA_IRQ` |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | `RADIOLIB_NC` |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |

## ARDUINO_TTGO_LoRa32_v21new

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | `LORA_CS` |
| Radio - RESET | `PIN_LORA_RST` | `LORA_RST` |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | `LORA_IRQ` |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | `LORA_D1` |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |

## ARDUINO_TBEAM_USE_RADIO_SX1262 / ARDUINO_TBEAM_USE_RADIO_SX1276

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio | `USE_SX1262` / `USE_SX1276` | Pin map awaiting implementation |

## ARDUINO_D1_MINI32

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 27 |
| Radio - RESET | `PIN_LORA_RST` | 32 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 21 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 33 |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |

## ARDUINO_HELTEC_WIFI_LORA_32

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio | (awaiting pin map) | Pin map not provided in this file |

## ARDUINO_HELTEC_WIFI_LORA_32_V2

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | `SS` (board symbol) |
| Radio - RESET | `PIN_LORA_RST` | `RST_LoRa` (board symbol) |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | `DIO0` (board symbol) |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | `DIO1` |
| Radio - DIO2 | `PIN_LORA_DIO2` | `DIO2` |
| Radio chip | `USE_SX1276` | SX1276 selected |

## ARDUINO_HELTEC_WIFI_LORA_32_V3

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | `SS` |
| Radio - RESET | `PIN_LORA_RST` | `RST_LoRa` |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | `DIO0` |
| Radio - GPIO / BUSY | `PIN_LORA_GPIO` | `BUSY_LoRa` |
| Radio chip | `USE_SX1262` | SX1262 selected |
| ADC input (V3) | `PIN_ADC_IN` | A0 |
| ADC control (V3) | `ADC_CTRL` | 37 |
| UBATT divisor (V3) | `UBATT_DIV` | 0.2041 |

## ARDUINO_HELTEC_WIRELESS_STICK / ARDUINO_HELTEC_WIRELESS_STICK_V3

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | `SS` |
| Radio - RESET | `PIN_LORA_RST` | `RST_LoRa` |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | `DIO0` |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | `DIO1` |
| Radio - DIO2 | `PIN_LORA_DIO2` | `DIO2` |
| Radio chip | `USE_SX1276` / `USE_SX1262` | V3 uses SX1262 |

## ARDUINO_CUBECELL_BOARD / ARDUINO_CUBECELL_BOARD_V2

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio | (awaiting pin map) | Not provided in file |

## ARDUINO_M5STACK_CORE2 / ARDUINO_M5STACK_Core2

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 33 |
| Radio - RESET | `PIN_LORA_RST` | 26 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 36 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |

## ARDUINO_DFROBOT_FIREBEETLE_ESP32 (LORAWAN_NODE)

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 14 |
| Radio - RESET | `PIN_LORA_RST` | 12 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 4 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 16 |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |
| ADC input (battery) | `PIN_ADC_IN` | A3 |
| Supply ADC | `PIN_SUPPLY_IN` | A0 |
| OneWire bus | `PIN_ONEWIRE_BUS` | 5 |
| A02YYUW TX | `A02YYUW_TX` | 0 |
| A02YYUW RX | `A02YYUW_RX` | 26 |
| A02YYUW PWR | `A02YYUW_PWR` | 25 |
| DYP-R01CW SDA | `DYP_R01CW_SDA` | 21 |
| DYP-R01CW SCL | `DYP_R01CW_SCL` | 22 |
| GPS power enable | `GPS_PWR_EN_PIN` | 27 (D4) |
| GPS RX | `GPS_RX_PIN` | 9 (D5) |

## ARDUINO_DFROBOT_FIREBEETLE_ESP32 (FIREBEETLE_ESP32_COVER_LORA)

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 27 (D4) |
| Radio - RESET | `PIN_LORA_RST` | 25 (D2) |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 26 (D3) |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 9 (D5) |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1276` | SX1276 selected |
| ADC input (battery) | `PIN_ADC_IN` | A0 |
| OneWire bus | `PIN_ONEWIRE_BUS` | 5 |
| GPS power enable | `GPS_PWR_EN_PIN` | 4 (DO) |
| GPS RX | `GPS_RX_PIN` | 13 (D7) |

> **Note:** Required wiring: D2 to RESET, D3 to DIO0, D4 to CS, D5 to DIO1

## ARDUINO_XIAO_ESP32S3

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 41 |
| Radio - RESET | `PIN_LORA_RST` | 42 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 39 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 40 |
| Radio - DIO2 | `PIN_LORA_DIO2` | `RADIOLIB_NC` |
| Radio chip | `USE_SX1262` | SX1262 selected |

## Default / Unknown board

| Interface | Define | Value / Notes |
|---|---:|---|
| Radio - CS (NSS) | `PIN_LORA_NSS` | 14 |
| Radio - RESET | `PIN_LORA_RST` | 12 |
| Radio - IRQ / DIO0 | `PIN_LORA_IRQ` | 4 |
| Radio - GPIO / DIO1 | `PIN_LORA_GPIO` | 16 |
| Radio chip | `LORA_CHIP` | `SX1276` (default selection in file) |

---

Notes:
- Wherever the original define uses board-symbolic names (e.g., `LORA_CS`, `LORA_RST`, `LORA_IRQ`, `SS`, `DIO0`), the table lists the symbolic name rather than a numeric GPIO because the numeric mapping is board/package-specific and defined elsewhere.
- `RADIOLIB_NC` indicates a not-connected / not-used pin for that radio mapping.
