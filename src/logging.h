///////////////////////////////////////////////////////////////////////////////
// logging.h
// 
//  Replacement for
//  https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-log.h
//  on RP2040
//
// - DEBUG_RP2040_PORT is set in Arduino IDE:
//   Tools->Debug port: "<Disabled>|<Serial>|<Serial1>|<Serial2>"
// - CORE_DEBUG_LEVEL has to be set manually below
//
// created: 09/2023
//
//
// MIT License
//
// Copyright (c) 2023 Matthias Prinke
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
// 20230927 Created from BresserWeatherSensorReceiver
// 20231004 Added function names and line numbers to ESP8266/RP2040 debug logging
// 20231005 Allowed re-definition of CORE_DEBUG_LEVEL and log_* macros
// 20251031 Added option for logging via Serial2
//
// ToDo:
// - 
//
///////////////////////////////////////////////////////////////////////////////

/*! \file logging.h
 *  \brief Replacement for esp32-hal-log.h on RP20240
 *
 * Replacement for
 * https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-log.h
 * on RP2040
 * 
 * - DEBUG_RP2040_PORT is set in Arduino IDE:
 *   Tools->Debug port: "<Disabled>|<Serial>|<Serial1>|<Serial2>"
 * - CORE_DEBUG_LEVEL has to be set manually below
 */

#ifndef LOGGING_H
#define LOGGING_H

// Enable to use Serial2 as debug output port
// This is useful to debug power management without powering/charging via USB
// #define SERIAL2_LOG_ENABLE

#if CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_NONE
const char *pathToFileName(const char *path);

#if defined(SERIAL2_LOG_ENABLE)

// Use Serial2 as debug port
#define DEBUG_PORT Serial2
#undef log_i
#define log_i(...) { DEBUG_PORT.printf("[I][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
#undef log_d
#define log_d(...) { DEBUG_PORT.printf("[D][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
#undef log_w
#define log_w(...) { DEBUG_PORT.printf("[W][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
#undef log_e
#define log_e(...) { DEBUG_PORT.printf("[E][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
#undef log_v
#define log_v(...) { DEBUG_PORT.printf("[V][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
#endif // SERIAL2_LOG_ENABLE
#endif // CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_NONE

#if defined(ARDUINO_ARCH_RP2040)

    #if defined(DEBUG_RP2040_PORT)
        #define DEBUG_PORT DEBUG_RP2040_PORT
    #endif

    #define ARDUHAL_LOG_LEVEL_NONE      0
    #define ARDUHAL_LOG_LEVEL_ERROR     1
    #define ARDUHAL_LOG_LEVEL_WARN      2
    #define ARDUHAL_LOG_LEVEL_INFO      3
    #define ARDUHAL_LOG_LEVEL_DEBUG     4
    #define ARDUHAL_LOG_LEVEL_VERBOSE   5

    // '#undef' allows to change a previous definition from WeatherSensor.h
    #undef log_e
    #undef log_w
    #undef log_i
    #undef log_d
    #undef log_v

    // Set desired level here!
    #undef CORE_DEBUG_LEVEL
    #define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_DEBUG

    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_NONE
        #define log_e(...) { DEBUG_PORT.printf("[E][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.println(); }
     #else
        #define log_e(...) {}
     #endif
    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_ERROR
        #define log_w(...) { DEBUG_PORT.printf("[W][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_w(...) {}
     #endif
    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_WARN
        #define log_i(...) { DEBUG_PORT.printf("[I][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_i(...) {}
     #endif
    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_INFO
        #define log_d(...) { DEBUG_PORT.printf("[D][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_d(...) {}
     #endif
    #if defined(DEBUG_PORT) && CORE_DEBUG_LEVEL > ARDUHAL_LOG_LEVEL_DEBUG
        #define log_v(...) { DEBUG_PORT.printf("[V][%s:%d] %s(): ", pathToFileName(__FILE__), __LINE__, __func__); DEBUG_PORT.printf(__VA_ARGS__); DEBUG_PORT.println(); }
     #else
        #define log_v(...) {}
     #endif

#endif // defined(ARDUINO_ARCH_RP2040)
#endif // LOGGING_H