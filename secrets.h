// JoinEUI (LoRaWAN  v1.1.0) or AppEUI (LoRaWAN v1.0.4)
// for development purposes you can use all zeros - see RadioLib wiki for details
#define RADIOLIB_LORAWAN_JOIN_EUI  0x0000000000000000

// The Device EUI & two keys can be generated on the TTN console (or any other LoRaWAN Network Service Provider's console)
#pragma message("Replace the dummy values for RADIOLIB_LORAWAN_DEV_EUI, RADIOLIB_LORAWAN_APP_KEY and RADIOLIB_LORAWAN_NWK_KEY by your own credentials.")
#ifndef RADIOLIB_LORAWAN_DEV_EUI   // Replace with your Device EUI
#define RADIOLIB_LORAWAN_DEV_EUI   0x0000000000000000
#endif
#ifndef RADIOLIB_LORAWAN_APP_KEY   // Replace with your App Key 
#define RADIOLIB_LORAWAN_APP_KEY   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
#endif
#ifndef RADIOLIB_LORAWAN_NWK_KEY   // Put your Nwk Key here
#define RADIOLIB_LORAWAN_NWK_KEY   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
#endif
