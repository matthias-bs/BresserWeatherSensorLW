///////////////////////////////////////////////////////////////////////////////
// LoadSecrets.cpp
//
// Load LoRaWAN secrets from file 'secrets.json' on LittleFS, if available
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
// 20240723 Created from BresserWeatherSensorLW.ino
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "LoadSecrets.h"

// Load LoRaWAN secrets from file 'secrets.json' on LittleFS, if available
void loadSecrets(uint64_t &joinEUI, uint64_t &devEUI, uint8_t *nwkKey, uint8_t *appKey)
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
    File file = LittleFS.open("/secrets.json", "r");

    if (!file)
    {
      log_i("File 'secrets.json' not found.");
    }
    else
    {
      log_d("Reading 'secrets.json'");
      JsonDocument doc;

      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, file);
      if (error)
      {
        log_d("Failed to read JSON file, using defaults.");
      }
      else
      {
        const char *joinEUIStr = doc["joinEUI"];
        if (joinEUIStr == nullptr)
        {
          log_e("Missing joinEUI.");
          file.close();
          return;
        }
        uint64_t _joinEUI = 0;
        for (int i = 2; i < 18; i += 2)
        {
          char tmpStr[3] = "";
          unsigned int tmpByte;
          strncpy(tmpStr, &joinEUIStr[i], 2);
          sscanf(tmpStr, "%x", &tmpByte);
          _joinEUI = (_joinEUI << 8) | tmpByte;
        }
        // printf() cannot print 64-bit hex numbers (sic!), so we split it in two 32-bit numbers...
        log_d("joinEUI: 0x%08X%08X", static_cast<uint32_t>(_joinEUI >> 32), static_cast<uint32_t>(_joinEUI & 0xFFFFFFFF));

        const char *devEUIStr = doc["devEUI"];
        if (devEUIStr == nullptr)
        {
          log_e("Missing devEUI.");
          file.close();
          return;
        }
        uint64_t _devEUI = 0;
        for (int i = 2; i < 18; i += 2)
        {
          char tmpStr[3] = "";
          unsigned int tmpByte;
          strncpy(tmpStr, &devEUIStr[i], 2);
          sscanf(tmpStr, "%x", &tmpByte);
          _devEUI = (_devEUI << 8) | tmpByte;
        }
        if (_devEUI == 0)
        {
          log_e("devEUI is zero.");
          file.close();
          return;
        }
        // printf() cannot print 64-bit hex numbers (sic!), so we split it in two 32-bit numbers...
        log_d("devEUI: 0x%08X%08X", static_cast<uint32_t>(_devEUI >> 32), static_cast<uint32_t>(_devEUI & 0xFFFFFFFF));

        uint8_t check = 0;
        bool fail = false;

        log_d("nwkKey:");
        uint8_t _nwkKey[16];
        for (size_t i = 0; i < 16; i++)
        {
          const char *buf = doc["nwkKey"][i];
          if (buf == nullptr)
          {
            fail = true;
            break;
          }
          unsigned int tmp;
          sscanf(buf, "%x", &tmp);
          _nwkKey[i] = static_cast<uint8_t>(tmp);
          check |= _nwkKey[i];
#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
          printf("0x%02X", _nwkKey[i]);
          if (i < 15)
          {
            printf(", ");
          }
#endif
        } // for all nwk_key bytes
#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
        printf("\n");
#endif
        if (fail || (check == 0))
        {
          log_e("nwkKey parse error");
          file.close();
          return;
        }

        check = 0;
        log_i("appKey:");
        uint8_t _appKey[16];
        for (size_t i = 0; i < 16; i++)
        {
          const char *buf = doc["appKey"][i];
          if (buf == nullptr)
          {
            fail = true;
            break;
          }
          unsigned int tmp;
          sscanf(buf, "%x", &tmp);
          _appKey[i] = static_cast<uint8_t>(tmp);
          check |= _appKey[i];
#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
          printf("0x%02X", _appKey[i]);
          if (i < 15)
          {
            printf(", ");
          }
#endif
        } // for all app_key bytes
#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
        printf("\n");
#endif
        if (fail || (check == 0))
        {
          log_e("appKey parse error");
          file.close();
          return;
        }

        // Every check passed, copy intermediate values as result
        joinEUI = _joinEUI;
        devEUI = _devEUI;
        memcpy(nwkKey, _nwkKey, 16);
        memcpy(appKey, _appKey, 16);
      } // deserializeJson o.k.
    } // file read o.k.
    file.close();
  } // LittleFS o.k.
}
