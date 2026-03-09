///////////////////////////////////////////////////////////////////////////////////////////////////
// gps_tiny.ino
//
// Minimal sketch for testing a GPS module connected via UART (Serial2)
// to ESP32 or RP2040.
//
// Based on
// https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/DeviceExample/DeviceExample.ino

// Note:
// This sketch can be used for testing the pinning, baudrate, power supply and load switch.
// Additionally, it allows to test the GPS module's startup time and if reception and decoding
// of the required data works correctly.
//
// Warning:
// Check the GPS module's datasheet for the correct power supply voltage and
// I/O voltage levels!
//
// The following are common GPS module configurations:
// 1. 3.3V power supply and 3.3V I/O levels
// 2. 5V power supply and 3.3V I/O levels (internal voltage regulator)
// 3. 5V power supply and 5V I/O levels (LEVEL SHIFTER REQUIRED!)
//
// Using the wrong voltage can damage the GPS module or the microcontroller.
//
// Sequence:
// 1. Wait (with GPS_TIMEOUT) for GPS data with valid time
//   - If timeout, check if any data was received from GPS module at all. Stop.
// 2. Receive NUM_UPDATES GPS time updates
// 3. Disable GPS module
// 4. Wait for TIME_DISABLED if any data can still be received from GPS module
// 5. Print if disabling was successful
// 6. Enable GPS
// 7. Continue at step 2.
//
// created: 03/2026
//
//
// MIT License
//
// Copyright (c) 2026 Matthias Prinke
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
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <TinyGPSPlus.h>

static const uint32_t SLEEP_TIME = 30;

// Number of time updates before sleep
#define NUM_UPDATES 20

// GPS baud rate
//#define GPS_BAUDRATE 9600 // M5Stack Unit GPS V1.0
#define GPS_BAUDRATE 115200 // M5Stack Unit GPS V1.1

// GPS timeout [s] - here: max. time from power on to valid time info
#define GPS_TIMEOUT 60

// GPS module disable time [s] - time to wait after disabling GPS
// During this time, it is checked if GPS data is still available.
#define TIME_DISABLED 10

// Board/load switch specific pin configuration for GPS power control and RX pin
// Add your GPS pin configuration here.
#if defined(ARDUINO_DFROBOT_FIREBEETLE_ESP32)

// GPS power enable pin (set to -1 if not used)
#define GPS_PWR_EN_PIN 4 // DO
// GPS power enable polarity: 1 = active high, 0 = active low
#define GPS_PWR_EN_ACTIVE 1
// GPS RX pin (set to -1 if not used)
#define GPS_RX_PIN 13 // D7

#elif defined(LORAWAN_NODE)

// GPS power enable pin (set to -1 if not used)
#define GPS_PWR_EN_PIN 27 // D4
// GPS power enable polarity: 1 = active high, 0 = active low
#define GPS_PWR_EN_ACTIVE 1
// GPS RX pin (set to -1 if not used)
#define GPS_RX_PIN 9 // D5

#else
#error "No board-specific GPS configuration defined!"

// GPS power enable pin (set to -1 if not used)
#define GPS_PWR_EN_PIN -1
// GPS power enable polarity: 1 = active high, 0 = active low
#define GPS_PWR_EN_ACTIVE 1
// GPS RX pin (set to -1 if not used)
#define GPS_RX_PIN -1

#endif // Board-specific GPS configuration

// The TinyGPSPlus object
TinyGPSPlus gps;

void gpsPowerControl(bool on)
{
  if (GPS_PWR_EN_PIN >= 0)
  {
    pinMode(GPS_PWR_EN_PIN, OUTPUT);
    digitalWrite(GPS_PWR_EN_PIN, (on == GPS_PWR_EN_ACTIVE) ? HIGH : LOW);
  }
}

void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN /* RX */, -1 /* TX */);
  delay(2000);

  Serial.println(F("\n=== gps_tiny.ino ==="));
  Serial.println(F("A simple demonstration of TinyGPSPlus with an attached GPS module"));
  Serial.print(F("Testing TinyGPSPlus library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();

  // Enable GPS module power
  gpsPowerControl(true);

  unsigned long start = millis();

  bool timeout = false;
  bool serialdata = false; 
  
  // CAUTION:
  // gps.time.isValid() and gps.date.isValid() return true as soon as a field has been parsed from an NMEA sentence,
  // regardless of whether the GPS module has actually acquired a fix. Therefore they can be true while day and month
  // are still 0 (e.g. 00/00/00 00:00:00). Non-zero day and month values indicate that we have received valid GPS
  // data with an actual fix.
  // see https://github.com/mikalhart/TinyGPSPlus/issues/107
  while (!gps.time.isValid()) {
    while (Serial2.available() > 0) {
      serialdata = true;
      if (gps.encode(Serial2.read()))
        displayInfo();
    }
    if (millis() - start > GPS_TIMEOUT * 1000UL) {
      timeout = true;
      break;
    }
  }

  if (!serialdata) {
    Serial.printf("Error: No GPS data received! Check GPS baudrate, power and data connection");
    // Stop
    while (1) {
      delay(10);
    }
  }
  if (timeout) {
    Serial.printf("Timeout: GPS could not acquire time within %u seconds!", GPS_TIMEOUT);
    Serial.printf("Check GPS signal reception and baudrate, maybe just increase GPS_TIMEOUT.");
    // Stop
    while (1) {
      delay(10);
    }
  }

  Serial.println("--- Time valid! ---");
  displayInfo();
}

void loop()
{
  static unsigned int count = 0;
  while (!gps.time.isUpdated()) {
    while (Serial2.available() > 0) 
      gps.encode(Serial2.read());
  }
  displayInfo();

  if (count++ > NUM_UPDATES) {
    Serial.println("Disabling GPS...");
    Serial.println("Waiting for data...");
    gpsPowerControl(false);
    delay(100);

    bool serialdata = false;
    unsigned long start = millis();

    // Wait for data
    do {
        if (Serial2.available()) {
           int c = Serial2.read();
           Serial.print((char)c);
           serialdata = true;
        }
        delay(10);
    } while (millis()-start < TIME_DISABLED * 1000UL);

    if (!serialdata) {
      Serial.println("No data received from GPS - good!");
    } else {
      Serial.println("\nData received from GPS - disabling failed!");
    }

    // Enable GPS again
    gpsPowerControl(true);
    count = 0;
  }
}
