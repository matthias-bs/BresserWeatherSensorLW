///////////////////////////////////////////////////////////////////////////////////////////////////
// gps_adafruit.ino
//
// Minimal sketch for testing a GPS module connected via UART (Serial2)
// to ESP32 or RP2040.
//
// Based on
// https://github.com/adafruit/Adafruit_GPS/blob/master/examples/GPS_HardwareSerial_Parsing/GPS_HardwareSerial_Parsing.ino
//
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
#include <Adafruit_GPS.h>

// Name of the hardware serial port
#define GPSSerial Serial2

// GPS baud rate
//#define GPS_BAUDRATE 9600 // M5Stack Unit GPS V1.0
#define GPS_BAUDRATE 115200 // M5Stack Unit GPS V1.1

// Sleep time in seconds
#define SLEEP_TIME 30

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

#define GPSECHO false

void gpsPowerControl(bool on)
{
  if (GPS_PWR_EN_PIN >= 0)
  {
    pinMode(GPS_PWR_EN_PIN, OUTPUT);
    digitalWrite(GPS_PWR_EN_PIN, (on == GPS_PWR_EN_ACTIVE) ? HIGH : LOW);
  }
}

void setup()
{
    Serial.begin(115200);
    delay(2000);
    gpsPowerControl(true);
    GPSSerial.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX_PIN /* RX */, -1 /* TX */);
    Adafruit_GPS GPS(&GPSSerial);
    uint32_t timer = millis();
    GPSSerial.print("\n\n\n");
    GPSSerial.flush();

    do
    {
        // read data from the GPS in the 'main loop'
        int c = GPS.read();
        // if you want to debug, this is a good time to do it!
        if (GPSECHO)
            if (c) {
                Serial.print((char)c);
            }
        // if a sentence is received, we can check the checksum, parse it...
        if (GPS.newNMEAreceived())
        {
            // a tricky thing here is if we print the NMEA sentence, or data
            // we end up not listening and catching other sentences!
            // so be very wary if using OUTPUT_ALLDATA and trying to print out data
            Serial.print(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
            if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
                continue;                     // we can fail to parse a sentence in which case we should just wait for another
        }

        // approximately every 2 seconds or so, print out the current stats
        if (millis() - timer > 2000)
        {
            timer = millis(); // reset the timer
            Serial.print("\nTime: ");
            if (GPS.hour < 10)
            {
                Serial.print('0');
            }
            Serial.print(GPS.hour, DEC);
            Serial.print(':');
            if (GPS.minute < 10)
            {
                Serial.print('0');
            }
            Serial.print(GPS.minute, DEC);
            Serial.print(':');
            if (GPS.seconds < 10)
            {
                Serial.print('0');
            }
            Serial.print(GPS.seconds, DEC);
            Serial.print('.');
            if (GPS.milliseconds < 10)
            {
                Serial.print("00");
            }
            else if (GPS.milliseconds > 9 && GPS.milliseconds < 100)
            {
                Serial.print("0");
            }
            Serial.println(GPS.milliseconds);
            Serial.print("Date: ");
            Serial.print(GPS.day, DEC);
            Serial.print('/');
            Serial.print(GPS.month, DEC);
            Serial.print("/20");
            Serial.println(GPS.year, DEC);
            Serial.print("Fix: ");
            Serial.print((int)GPS.fix);
            Serial.print(" quality: ");
            Serial.println((int)GPS.fixquality);
        }
    } while (!GPS.fix);

    Serial.println("\n--- GPS fix acquired! ---");
    Serial.print("Fix quality: ");
    Serial.println((int)GPS.fixquality);
    Serial.println("Disabling GPS...");
    gpsPowerControl(false);
    Serial.println("Sleeping...");
    Serial.flush();
    esp_sleep_enable_timer_wakeup(SLEEP_TIME * 1000UL * 1000UL); // function uses uS
    esp_deep_sleep_start();
}

void loop()
{
    delay(100);
}
