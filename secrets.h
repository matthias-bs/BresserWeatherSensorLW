// JoinEUI - previous versions of LoRaWAN called this AppEUI
// for development purposes you can use all zeros - see wiki for details
#define RADIOLIB_LORAWAN_JOIN_EUI  0x0000000000000000

// The Device EUI & two keys can be generated on the TTN console (or any other LoRaWAN Network Service Provider's console) 
#ifndef RADIOLIB_LORAWAN_DEV_EUI   // Replace with your Device EUI
#define RADIOLIB_LORAWAN_DEV_EUI   0x---------------
#endif
#ifndef RADIOLIB_LORAWAN_APP_KEY   // Replace with your App Key 
#define RADIOLIB_LORAWAN_APP_KEY   0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- 
#endif
#ifndef RADIOLIB_LORAWAN_NWK_KEY   // Put your Nwk Key here
#define RADIOLIB_LORAWAN_NWK_KEY   0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- 
#endif
