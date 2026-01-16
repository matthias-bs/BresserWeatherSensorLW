///////////////////////////////////////////////////////////////////////////////
// BresserWeatherSensorLW.ino
//
// Bresser 868 MHz Weather Sensor Radio Receiver
// based on ESP32 and RFM95W/SX1276/SX1262/LR1121 -
// sends data to a LoRaWAN network (e.g. The Things Network)
//
// The radio transceiver is used with RadioLib
// in FSK mode to receive weather sensor data
// and
// in LoRaWAN mode to connect to a LoRaWAN network
//
// Based on:
// ---------
// RadioLib by Jan Gromeš (https://github.com/jgromes/RadioLib)
// Persistence support for RadioLib (https://github.com/radiolib-org/radiolib-persistence)
// Bresser5in1-CC1101 by Sean Siford (https://github.com/seaniefs/Bresser5in1-CC1101)
// rtl_433 (https://github.com/merbanan/rtl_433)
// BresserWeatherSensorTTN by Matthias Prinke (https://github.com/matthias-bs/BresserWeatherSensorTTN)
// Lora-Serialization by Joscha Feth (https://github.com/thesolarnomad/lora-serialization)
// OneWireNg by Piotr Stolarz (https://github.com/pstolarz/OneWireNg)
// DallasTemperature / Arduino-Temperature-Control-Library by Miles Burton (https://github.com/milesburton/Arduino-Temperature-Control-Library)
//
// Library dependencies (tested versions):
// ---------------------------------------
// (install via normal Arduino Library installer:)
// RadioLib                             7.4.0
// LoRa Serialization                   3.3.1
// BresserWeatherSensorReceiver         0.37.0
// OneWireNg                            0.14.1 (optional)
// DallasTemperature                    4.0.5 (optional)
// NimBLE-Arduino                       2.3.7 (optional)
// ATC MiThermometer                    0.5.0 (optional)
// Theengs Decoder                      2.0.0 (optional)
// RTClib (Adafruit)                    2.1.4 (optional)
//
// (installed from ZIP file:)
// DistanceSensor_A02YYUW               1.0.2 (optional)
//
// created: 04/2024
//
//
// MIT License
//
// Copyright (c) 2025 Matthias Prinke
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
// 20240407 Created from
//          https://github.com/matthias-bs/BresserWeatherSensorTTN (v0.12.2)
//          https://github.com/jgromes/RadioLib/blob/master/examples/LoRaWAN/LoRaWAN_Reference/LoRaWAN_Reference.ino
//          https://github.com/radiolib-org/radiolib-persistence/blob/main/examples/LoRaWAN_ESP32/LoRaWAN_ESP32.ino
// 20240410 Added RP2040 specific implementation
//          Added minimum sleep interval (and thus uplink interval)
//          Added M5Stack Core2 initialization
// 20240414 Added separation between LoRaWAN and application layer
//          Fixed battLevel calculation
// 20240415 Added ESP32-S3 PowerFeather
// 20240416 Added enabling of 3.3V power supply for FeatherWing on ESP32-S3 PowerFeather
// 20240423 Removed rtcSyncReq & runtimeExpired, added rtcTimeSource
// 20240424 Added appLayer.begin()
// 20240504 PowerFeather: added BATTERY_CAPACITY_MAH to init()
//          Added BresserWeatherSensorLWCmd.h
// 20240505 Implemented loading of LoRaWAN secrets from file on LittleFS (if available)
// 20230524 Modified PAYLOAD_SIZE: Moved define to header file, added small reserve
//          to uplinkPayload[], modified actual size in sendReceive()
// 20240528 Disabled uplink transmission of LoRaWAN node status flags
// 20242529 Fixed payload size calculation
// 20240530 Updated to RadioLib v6.6.0
// 20240603 Added AppLayer status uplink
// 20240606 Changed appStatusUplinkInterval from const to variable
// 20240608 Added LoRaWAN device status uplink
// 20240630 Switched to lwActivate() from radiolib-persistence/examples/LoRaWAN_ESP32
// 20240716 Modified port to allow modifications by appLayer.getPayloadStage<1|2>()
// 20240722 Added periodic uplink of LoRaWAN node status messages
// 20240723 Moved loadSecrets() to LoadSecrets.cpp/.h
//          Moved decodeDownlink() & sendCfgUplink() to BresserWeatherSensorLWCmd.cpp/.h
// 20240725 Added reading of hardware/deployment specific configuration node_config.json
//          from LittleFS (optional)
// 20240729 PowerFeather: Enabled battery temperature measurement, added specific configuration
// 20240730 PowerFeather: modified setSupplyMaintainVoltage()
// 20240804 PowerFeather: Added configuration of maximum battery charging current
// 20240818 Fixed bootCount
// 20240920 Fixed handling of downlink after any kind of uplink
// 20240912 Bumped to RadioLib v7.0.0
// 20240928 Modified for LoRaWAN v1.0.4 (requires no nwkKey)
// 20241123 PowerFeather: Fixed inadverted sleep if battery low & supply o.k.
// 20241203 Added supply voltage measurement if PIN_SUPPLY_IN is defined
//          Moved start of sensor reception after battery voltage check
//          Modified sleep duration if battery is low but external power is available
// 20241227 Moved uplinkDelay() from BresserWeatherSensorLWCmd.cpp
//          Changed to use radio object from BresserWeatherSensorReceiver
// 20250317 Removed ARDUINO_M5STACK_Core2 (now all uppercase)
// 20250318 Renamed PAYLOAD_SIZE to MAX_UPLINK_SIZE, payloadSize to uplinkSize
// 20250622 Updated to RadioLib v7.2.0, added custom delay (ESP32 light sleep)
// 20250803 Added support for external RTC chips
// 20250806 Refactored by adding SystemContext class,
//          moved system code from BresserWeatherSensorLW.ino
// 20250820 Added initial sysCtx.getVoltages() call
//          Moved battery level calculation to SystemContext
// 20251031 Added logging via Serial2
// 20260116 Fixed radio module initialization for LilyGo T3S3 boards using RadioLib 7.5.0
//          if the radio object is created here (instead of in BresserWeatherSensorReceiver)
//          Added configuration for Seeed Studio XIAO ESP32S3 with Wio-SX1262
//
// ToDo:
// -
//
// Notes:
// - Set "Core Debug Level: Debug" for initial testing
// - The lines with "#pragma message()" in the compiler output are not errors, but useful hints!
// - The default LoRaWAN credentials are read at compile time from secrets.h (included in config.h),
//   they can be overridden by the JSON file secrets.json placed in LittleFS
//   (Use https://github.com/earlephilhower/arduino-littlefs-upload for uploading.)
// - Pin mapping of radio transceiver module is done in two places:
//   - BresserWeatherSensorLW:       config.h
//   - BresserWeatherSensorReceiver: WeatherSensorCfg.h
// - After a successful transmission, the controller can go into deep sleep
// - If joining the network or transmitting uplink data fails,
//   the controller will go into deep sleep
// - For LoRaWAN Specification 1.1.0, a small set of data (the "nonces") have to be stored persistently -
//   this implementation uses Flash (via Preferences library)
// - Storing LoRaWAN network session information speeds up the connection (join) after a restart -
//   this implementation uses the ESP32's RTC RAM or a variable located in the RP2040's RAM, respectively.
//   In the latter case, an uninitialized linker section is used for this purpose.
// - The ESP32's Bluetooth LE interface is used to access sensor data (option)
// - settimeofday()/gettimeofday() must be used to access the ESP32's RTC time
// - Arduino ESP32 package has built-in time zone handling, see
//   https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32/blob/master/NTP_Example/NTP_Example.ino
//
///////////////////////////////////////////////////////////////////////////////
/*! \file BresserWeatherSensorLW.ino */

#if !defined(ESP32) && !defined(ARDUINO_ARCH_RP2040)
#pragma error("This is not the example your device is looking for - ESP32 & RP2040 only")
#endif

// LoRa_Serialization
#include <LoraMessage.h>

// ESP32/RP2040 Preferences
#include <Preferences.h>
static Preferences store;

// Logging macros for RP2040
#include "src/logging.h"

// LoRaWAN config, credentials & pinmap
#include "config.h"

#include <RadioLib.h>
#include "BresserWeatherSensorLWCfg.h"
#include "BresserWeatherSensorLWCmd.h"

#if defined(EXT_RTC)
// Adafruit RTClib - https://github.com/adafruit/RTClib
#include <RTClib.h>
#endif

#include "src/LoadSecrets.h"
#include "src/AppLayer.h"
#include "src/SystemContext.h"

#if defined(RADIO_CHIP)
// Use radio object from BresserWeatherSensorReceiver
extern RADIO_CHIP radio;

#else

#if defined(ARDUINO_LILYGO_T3S3_SX1262) || defined(ARDUINO_LILYGO_T3S3_SX1276) || defined(ARDUINO_LILYGO_T3S3_LR1121)
static SPIClass *spi = new SPIClass(SPI);

// Create radio object with custom SPI configuration
LORA_CHIP radio = new Module(PIN_LORA_NSS, PIN_LORA_IRQ, PIN_LORA_RST, PIN_LORA_GPIO, *spi);

#else
// Create radio object
LORA_CHIP radio = new Module(PIN_LORA_NSS, PIN_LORA_IRQ, PIN_LORA_RST, PIN_LORA_GPIO);

#endif
#endif

#if defined(ARDUINO_LILYGO_T3S3_LR1121)
static const uint32_t rfswitch_dio_pins[] = {
    RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6,
    RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};

static const Module::RfSwitchMode_t rfswitch_table[] = {
    // mode                  DIO5  DIO6
    {LR11x0::MODE_STBY, {LOW, LOW}},
    {LR11x0::MODE_RX, {HIGH, LOW}},
    {LR11x0::MODE_TX, {LOW, HIGH}},
    {LR11x0::MODE_TX_HP, {LOW, HIGH}},
    {LR11x0::MODE_TX_HF, {LOW, LOW}},
    {LR11x0::MODE_GNSS, {LOW, LOW}},
    {LR11x0::MODE_WIFI, {LOW, LOW}},
    END_OF_MODE_TABLE,
};
#endif // ARDUINO_LILYGO_T3S3_LR1121

/// System context
SystemContext sysCtx;

/// Application layer
AppLayer appLayer(&sysCtx);

// LoRaWAN specific variables which must retain their values after deep sleep
#if defined(ESP32)
// Stored in RTC RAM
RTC_DATA_ATTR bool appStatusUplinkPending = false;
RTC_DATA_ATTR bool lwStatusUplinkPending = false;
RTC_DATA_ATTR uint8_t LWsession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];

#else
/// RP2040 RAM is preserved during sleep; we just have to ensure that it is not initialized at startup (after reset)
uint8_t LWsession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE] __attribute__((section(".uninitialized_data")));

/// AppLayer status uplink pending
bool appStatusUplinkPending __attribute__((section(".uninitialized_data")));

/// LoRaWAN Node status uplink pending
bool lwStatusUplinkPending __attribute__((section(".uninitialized_data")));
#endif

/*!
 * \brief Activate node by restoring session or otherwise joining the network
 *
 * \return RADIOLIB_LORAWAN_NEW_SESSION or RADIOLIB_LORAWAN_SESSION_RESTORED
 */
int16_t lwActivate(LoRaWANNode &node)
{
  // setup the OTAA session information
#if defined(LORAWAN_VERSION_1_1)
  int16_t state = node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);
#elif defined(LORAWAN_VERSION_1_0_4)
  int16_t state = node.beginOTAA(joinEUI, devEUI, nullptr, appKey);
#else
#error "LoRaWAN version not defined"
#endif

  debug(state != RADIOLIB_ERR_NONE, "Initialise node failed", state, true);

  log_d("Recalling LoRaWAN nonces & session");
  // ##### setup the flash storage
  store.begin("radiolib");
  // ##### if we have previously saved nonces, restore them and try to restore session as well
  if (store.isKey("nonces"))
  {
    uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];                   // create somewhere to store nonces
    store.getBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // get them from the store
    state = node.setBufferNonces(buffer);                               // send them to LoRaWAN
    debug(state != RADIOLIB_ERR_NONE, "Restoring nonces buffer failed", state, false);

    // recall session from RTC deep-sleep preserved variable
    state = node.setBufferSession(LWsession); // send them to LoRaWAN stack

    // if we have booted more than once we should have a session to restore, so report any failure
    // otherwise no point saying there's been a failure when it was bound to fail with an empty LWsession var.
    debug((state != RADIOLIB_ERR_NONE) && !sysCtx.isFirstBoot(), "Restoring session buffer failed", state, false);

    // if Nonces and Session restored successfully, activation is just a formality
    // moreover, Nonces didn't change so no need to re-save them
    if (state == RADIOLIB_ERR_NONE)
    {
      log_d("Successfully restored session - now activating");
      state = node.activateOTAA();
      debug((state != RADIOLIB_LORAWAN_SESSION_RESTORED), "Failed to activate restored session", state, true);

      // ##### close the store before returning
      store.end();
      return (state);
    }
  }
  else
  { // store has no key "nonces"
    log_d("No Nonces saved - starting fresh.");
  }

  // if we got here, there was no session to restore, so start trying to join
  state = RADIOLIB_ERR_NETWORK_NOT_JOINED;
  while (state != RADIOLIB_LORAWAN_NEW_SESSION)
  {
    log_i("Join ('login') to the LoRaWAN Network");
    state = node.activateOTAA();

    // ##### save the join counters (nonces) to permanent store
    log_d("Saving nonces to flash");
    uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];                   // create somewhere to store nonces
    uint8_t *persist = node.getBufferNonces();                          // get pointer to nonces
    memcpy(buffer, persist, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);          // copy in to buffer
    store.putBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // send them to the store

    // we'll save the session after an uplink

    if (state != RADIOLIB_LORAWAN_NEW_SESSION)
    {
      log_i("Join failed: %d", state);
      sysCtx.sleepAfterFailedJoin();

    } // if activateOTAA state
  } // while join

  log_i("Joined");

  // reset the failed join count
  sysCtx.resetFailedJoinCount();

  delay(1000); // hold off off hitting the airwaves again too soon - an issue in the US

  // ##### close the store
  store.end();
  return (state);
}

// setup & execute all device functions ...
void setup()
{
#if defined(ARDUINO_M5STACK_CORE2)
  sysCtx.setupM5StackCore2();
#endif

#if CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_NONE
#if !defined(SERIAL2_LOG_ENABLE)
  Serial.begin(115200);
  Serial.setDebugOutput(true);
#else
#if defined(ARDUINO_ESP32S3_POWERFEATHER)
  // PowerFeather
  Serial2.begin(115200, SERIAL_8N1, 44 /* TX */, 42 /* RX */);
  Serial2.setDebugOutput(true);
#elif defined(ARDUINO_M5STACK_CORE2)
  // M5Stack Core2
  // Port C
  // GND - black
  // 5V - red
  // G13 (RX2) - yellow
  // G14 (TX2) - white
  Serial2.begin(115200, SERIAL_8N1, TX2, RX2);
  Serial2.setDebugOutput(true);
#else
#error "SERIAL2_LOG_ENABLE defined but no Serial2 configuration for this board"
#endif
#endif // SERIAL2_LOG_ENABLE

  delay(2000); // give time to switch to the serial monitor
#endif         // CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_NONE
  log_i("Setup");

  sysCtx.begin();

  if (sysCtx.isFirstBoot())
  {
    appStatusUplinkPending = false;
    lwStatusUplinkPending = false;
  }

// Try to load LoRaWAN secrets from LittleFS file, if available
#ifdef LORAWAN_VERSION_1_1
  bool requireNwkKey = true;
#else
  bool requireNwkKey = false;
#endif
  loadSecrets(requireNwkKey, joinEUI, devEUI, nwkKey, appKey);

  sysCtx.getVoltages();
  sysCtx.sleepIfSupplyLow();

  // Initialize Application Layer - starts sensor reception
  appLayer.begin();

  // build payload byte array (+ reserve to prevent overflow with configuration at run-time)
  uint8_t uplinkPayload[MAX_UPLINK_SIZE + 8];

  LoraEncoder encoder(uplinkPayload);

  uint8_t fPort = 1;
  appLayer.getPayloadStage1(fPort, encoder);

  int16_t state = 0; // return value for calls to RadioLib

#if !defined(RADIO_CHIP)
#if defined(ARDUINO_LILYGO_T3S3_SX1262) || defined(ARDUINO_LILYGO_T3S3_SX1276) || defined(ARDUINO_LILYGO_T3S3_LR1121)
  // Use local radio object with custom SPI configuration
  spi->begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
#endif
#endif

  radio.reset();
  LoRaWANNode node(&radio, &Region, subBand);

  // setup the radio based on the pinmap (connections) in config.h
  log_v("Initialise radio");

  state = radio.begin();
  debug(state != RADIOLIB_ERR_NONE, "Initialise radio failed", state, true);

// Using local radio object
#if defined(ARDUINO_LILYGO_T3S3_LR1121)
  radio.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table);

  // LR1121 TCXO Voltage 2.85~3.15V
  radio.setTCXO(3.0);
#endif

#if defined(ARDUINO_XIAO_ESP32S3)
    // set RF switch control configuration
    radio.setRfSwitchPins(38, RADIOLIB_NC);

    // TCXO Voltage according to
    // https://files.seeedstudio.com/products/SenseCAP/Wio_SX1262/Wio-SX1262_Module_Datasheet.pdf
    // 1.7~3.3V
    //
    // Set to 3.0V as in code example
    // https://github.com/Seeed-Studio/one_channel_hub/blob/4cc771ac02da1bd18be67509f6b52d21bb0feabd/components/smtc_ral/bsp/sx126x/seeed_xiao_esp32s3_devkit_sx1262.c#L351
    radio.setTCXO(3.0);
#endif

#if defined(ESP32)
  // Optionally provide a custom sleep function - see config.h
  node.setSleepFunction(customDelay);
#endif

  // activate node by restoring session or otherwise joining the network
  state = lwActivate(node);
  // state is one of RADIOLIB_LORAWAN_NEW_SESSION or RADIOLIB_LORAWAN_SESSION_RESTORED

  uint8_t battLevel = sysCtx.getBattlevel();
  log_d("Battery level: %u", battLevel);
  node.setDeviceStatus(battLevel);

  // Check if clock was never synchronized or sync interval has expired
  if (sysCtx.rtcNeedsSync())
  {
    log_i("RTC sync required");
    node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_DEVICE_TIME);
  }

  // get payload immediately before uplink - not used here
  appLayer.getPayloadStage2(fPort, encoder);

  uint8_t downlinkPayload[MAX_DOWNLINK_SIZE]; // Make sure this fits your plans!
  size_t downlinkSize;                        // To hold the actual payload size rec'd
  LoRaWANEvent_t downlinkDetails;

  uint8_t uplinkSize = encoder.getLength();
  uint8_t maxPayloadLen = node.getMaxPayloadLen();
  log_d("Max payload length: %u", maxPayloadLen);
  if (uplinkSize > maxPayloadLen)
  {
    log_w("Payload size exceeds maximum of %u bytes - truncating", maxPayloadLen);
    uplinkSize = maxPayloadLen;
  }

  // ----- and now for the main event -----

  enum class E_FSM_STAGE : uint8_t
  {
    E_SENSORDATA = 0x00,
    E_RESPONSE = 0x01,
    E_LWSTATUS = 0x02,
    E_APPSTATUS = 0x03,
    E_DONE = 0x04
  };

  E_FSM_STAGE fsmStage = E_FSM_STAGE::E_SENSORDATA;

  /// Uplink request - command received via downlink
  uint8_t uplinkReq = 0;

  do
  {
    // Retrieve the last uplink frame counter
    uint32_t fCntUp = node.getFCntUp();
    log_d("FcntUp: %u", node.getFCntUp());

    bool isConfirmed = false;

    // Send a confirmed uplink every 64th frame
    // and also request the LinkCheck command
    if (fCntUp && (fCntUp % 64 == 0))
    {
      log_i("[LoRaWAN] Requesting LinkCheck");
      node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_LINK_CHECK);
      isConfirmed = true;
    }

    // Set appStatusUplink flag if required
    uint8_t appStatusUplinkInterval = appLayer.getAppStatusUplinkInterval();
    if (appStatusUplinkInterval && (fCntUp % appStatusUplinkInterval == 0))
    {
      appStatusUplinkPending = true;
      log_i("App status uplink pending");
    }

    // Set lwStatusUplink flag if required
    if (sysCtx.lw_stat_interval && (fCntUp % sysCtx.lw_stat_interval == 0))
    {
      lwStatusUplinkPending = true;
      log_i("LoRaWAN node status uplink pending");
    }

    if (fsmStage == E_FSM_STAGE::E_RESPONSE)
    {
      log_d("Sending response uplink.");
      fPort = uplinkReq;
      encodeCfgUplink(fPort, uplinkPayload, uplinkSize);
      sysCtx.uplinkDelay(node.timeUntilUplink(), uplinkIntervalSeconds);
    }
    else if (fsmStage == E_FSM_STAGE::E_LWSTATUS)
    {
      log_d("Sending LoRaWAN status uplink.");
      fPort = CMD_GET_LW_STATUS;
      encodeCfgUplink(fPort, uplinkPayload, uplinkSize);
      sysCtx.uplinkDelay(node.timeUntilUplink(), uplinkIntervalSeconds);
      lwStatusUplinkPending = false;
    }
    else if (fsmStage == E_FSM_STAGE::E_APPSTATUS)
    {
      log_d("Sending application status uplink.");
      fPort = CMD_GET_SENSORS_STAT;
      encodeCfgUplink(fPort, uplinkPayload, uplinkSize);
      sysCtx.uplinkDelay(node.timeUntilUplink(), uplinkIntervalSeconds);
      appStatusUplinkPending = false;
    }

    log_i("Sending uplink; port %u, size %u", fPort, uplinkSize);

    state = node.sendReceive(
        uplinkPayload,
        uplinkSize,
        fPort,
        downlinkPayload,
        &downlinkSize,
        isConfirmed,
        nullptr,
        &downlinkDetails);
    debug(state < RADIOLIB_ERR_NONE, "Error in sendReceive", state, false);

    uplinkReq = 0;

    // Check if downlink was received
    // (state 0 = no downlink, state 1/2 = downlink in window Rx1/Rx2)
    if (state > 0)
    {
      // Did we get a downlink with data for us
      if (downlinkSize > 0)
      {
        log_i("Downlink port %u, data: ", downlinkDetails.fPort);
        arrayDump(downlinkPayload, downlinkSize);

        if (downlinkDetails.fPort > 0)
        {
          uplinkReq = decodeDownlink(downlinkDetails.fPort, downlinkPayload, downlinkSize);
        }
      }
      else
      {
        log_d("<MAC commands only>");
      }

      // print RSSI (Received Signal Strength Indicator)
      log_d("[LoRaWAN] RSSI:\t\t%f dBm", radio.getRSSI());

      // print SNR (Signal-to-Noise Ratio)
      log_d("[LoRaWAN] SNR:\t\t%f dB", radio.getSNR());

      // print frequency error
      log_d("[LoRaWAN] Frequency error:\t%f Hz", radio.getFrequencyError());

      // print extra information about the event
      log_d("[LoRaWAN] Event information:");
      log_d("[LoRaWAN] Confirmed:\t%d", downlinkDetails.confirmed);
      log_d("[LoRaWAN] Confirming:\t%d", downlinkDetails.confirming);
      log_d("[LoRaWAN] Datarate:\t%d", downlinkDetails.datarate);
      log_d("[LoRaWAN] Frequency:\t%7.3f MHz", downlinkDetails.freq);
      log_d("[LoRaWAN] Output power:\t%d dBm", downlinkDetails.power);
      log_d("[LoRaWAN] Frame count:\t%u", downlinkDetails.fCnt);
      log_d("[LoRaWAN] fPort:\t\t%u", downlinkDetails.fPort);
      log_d("[LoRaWAN] Time-on-air: \t%u ms", node.getLastToA());
      log_d("[LoRaWAN] Rx window: %d", state);
    }

    uint32_t networkTime = 0;
    uint16_t milliseconds = 0;
    if (node.getMacDeviceTimeAns(&networkTime, &milliseconds, true) == RADIOLIB_ERR_NONE)
      uint16_t milliseconds = 0;
    if (node.getMacDeviceTimeAns(&networkTime, &milliseconds, true) == RADIOLIB_ERR_NONE)
    {
      log_i("[LoRaWAN] DeviceTime Unix:\t %ld", networkTime);
      log_i("[LoRaWAN] DeviceTime frac:\t%u ms", milliseconds);

      sysCtx.setTime(networkTime, E_TIME_SOURCE::E_LORA);
      log_d("RTC sync completed");
      sysCtx.printDateTime();
    }

    uint8_t margin = 0;
    uint8_t gwCnt = 0;
    if (node.getMacLinkCheckAns(&margin, &gwCnt) == RADIOLIB_ERR_NONE)
    {
      log_d("[LoRaWAN] LinkCheck margin:\t%d", margin);
      log_d("[LoRaWAN] LinkCheck count:\t%u", gwCnt);
    }

    if (uplinkReq)
    {
      fsmStage = E_FSM_STAGE::E_RESPONSE;
    }
    else if (lwStatusUplinkPending)
    {
      fsmStage = E_FSM_STAGE::E_LWSTATUS;
    }
    else if (appStatusUplinkPending)
    {
      fsmStage = E_FSM_STAGE::E_APPSTATUS;
    }
    else
    {
      fsmStage = E_FSM_STAGE::E_DONE;
    }
  } while (fsmStage != E_FSM_STAGE::E_DONE);

  // now save session to RTC memory
  uint8_t *persist = node.getBufferSession();
  memcpy(LWsession, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

  // wait until next uplink - observing legal & TTN Fair Use Policy constraints
  sysCtx.gotoSleep(sysCtx.sleepDuration());
}

// The MCU wakes from deep-sleep and starts from the very beginning.
// It then goes back to sleep, so loop() is never called and which is
// why it is empty.

void loop() {}
