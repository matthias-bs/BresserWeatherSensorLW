///////////////////////////////////////////////////////////////////////////////
// gps_test.ino
//
// Minimal sketch for testing a GPS module connected via UART (Serial2)
// to ESP32 or RP2040.
//
// Note:
// This sketch only reads and prints raw NMEA sentences from the GPS module.
// It can be used for testing the pinning, baudrate, power supply and 
// load switch.
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
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>


// GPS baud rate
//#define GPS_BAUDRATE        9600 // M5Stack Unit GPS V1.0
#define GPS_BAUDRATE      115200 // M5Stack Unit GPS V1.1

// Board/load switch specific pin configuration for GPS power control and RX pin
// Add your GPS pin configuration here.
#if defined(ARDUINO_DFROBOT_FIREBEETLE_ESP32)

// GPS power enable pin (set to -1 if not used)
#define GPS_PWR_EN_PIN      4 // DO
// GPS power enable polarity: 1 = active high, 0 = active low
#define GPS_PWR_EN_ACTIVE   1
// GPS RX pin (set to -1 if not used)
#define GPS_RX_PIN          13 // D7

#elif defined(LORAWAN_NODE)

// GPS power enable pin (set to -1 if not used)
#define GPS_PWR_EN_PIN      27 // D4
// GPS power enable polarity: 1 = active high, 0 = active low
#define GPS_PWR_EN_ACTIVE   1
// GPS RX pin (set to -1 if not used)
#define GPS_RX_PIN          9 // D5

#else
#error "No board-specific GPS configuration defined!"

// GPS power enable pin (set to -1 if not used)
#define GPS_PWR_EN_PIN      -1
// GPS power enable polarity: 1 = active high, 0 = active low
#define GPS_PWR_EN_ACTIVE   1
// GPS RX pin (set to -1 if not used)
#define GPS_RX_PIN          -1

#endif // Board-specific GPS configuration

void setup()
{
    Serial.begin(115200);
    Serial2.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN, -1);

    delay(2000);
    Serial.println("\n=== GPS Test ===");
    int c;

    // Enable GPS power if configured
    if (GPS_PWR_EN_PIN >= 0) {
        pinMode(GPS_PWR_EN_PIN, OUTPUT);
        digitalWrite(GPS_PWR_EN_PIN, GPS_PWR_EN_ACTIVE ? HIGH : LOW);
    }

    while (1)
    {
        // read data from the GPS in the 'main loop'
        if (Serial2.available()) {
           c = Serial2.read();
        }
        else {
          c = 0;
        }
        if (c) {
            Serial.print((char)c);
        }
        delay(10);
    }
}

void loop() {
    delay(100);
}