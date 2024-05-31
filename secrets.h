#define SECRETS

//-------------------------------
// --> REPLACE BY YOUR VALUES <--
//-------------------------------

#if 0
// application identifier - pre-LoRaWAN 1.1.0, this was called appEUI
// when adding new end device in TTN, you will have to enter this number
// you can pick any number you want, but it has to be unique
uint64_t joinEUI = 0x12AD1011B0C0FFEE;

// device identifier - this number can be anything
// when adding new end device in TTN, you can generate this number,
// or you can set any value you want, provided it is also unique
uint64_t devEUI = 0x70B3D57ED005E120;

// select some encryption keys which will be used to secure the communication
// there are two of them - network key and application key
// because LoRaWAN uses AES-128, the key MUST be 16 bytes (or characters) long

// network key is the ASCII string "topSecretKey1234"
uint8_t nwkKey[] = { 0x74, 0x6F, 0x70, 0x53, 0x65, 0x63, 0x72, 0x65,
                     0x74, 0x4B, 0x65, 0x79, 0x31, 0x32, 0x33, 0x34 };

// application key is the ASCII string "aDifferentKeyABC"
uint8_t appKey[] = { 0x61, 0x44, 0x69, 0x66, 0x66, 0x65, 0x72, 0x65,
                     0x6E, 0x74, 0x4B, 0x65, 0x79, 0x41, 0x42, 0x43 };

// prior to LoRaWAN 1.1.0, only a single "nwkKey" is used
// when connecting to LoRaWAN 1.0 network, "appKey" will be disregarded
// and can be set to NULL
#endif


// Adafruit RFM9x LoRa Radio #1
//#define RADIO1
//#define RADIO4
#define RADIO5
//#define RADIO7
//#define RADIO9

#ifdef RADIO1
static const uint64_t devEUI = 0x9876B6000011C941;

static const uint64_t joinEUI = 0x0000000000000000;

static const std::uint8_t nwkKey[] = { 0x81, 0x83, 0xF4, 0xDD, 0x1B, 0x1F, 0xCF, 0x36, 0xC3, 0x73, 0x2A, 0xBE, 0x7C, 0xA2, 0x2F, 0x92 };

static const std::uint8_t appKey[] = { 0x24, 0xD4, 0x43, 0xCD, 0xB4, 0xD5, 0xDE, 0x3F, 0x9E, 0x17, 0xB7, 0x19, 0xE1, 0xD7, 0x16, 0xA2 };

#elif defined(RADIO4)
// Adafruit RFM9x LoRa Radio #4

static const uint64_t devEUI = 0x9876B6000011F87A;

static const uint64_t joinEUI = 0x0000000000000000;

static const std::uint8_t nwkKey[] = { 0x81, 0x83, 0xF4, 0xDD, 0x1B, 0x1F, 0xCF, 0x36, 0xC3, 0x73, 0x2A, 0xBE, 0x7C, 0xA2, 0x2F, 0x92 };

static const std::uint8_t appKey[] = { 0x39, 0x15, 0x2C, 0xBB, 0xDA, 0xEC, 0x25, 0xC6, 0x53, 0x4F, 0xA2, 0x20, 0x40, 0x04, 0xFA, 0xC8 };

#elif defined(RADIO5)

#define RADIOLIB_LORAWAN_DEV_EUI 0x0C8B95AAB05CFEFF

//static const uint64_t joinEUI = 0x0000000000000000;

#define RADIOLIB_LORAWAN_NWK_KEY 0x81, 0x83, 0xF4, 0xDD, 0x1B, 0x1F, 0xCF, 0x36, 0xC3, 0x73, 0x2A, 0xBE, 0x7C, 0xA2, 0x2F, 0x92

#define RADIOLIB_LORAWAN_APP_KEY 0x73, 0xF9, 0x96, 0x32, 0x29, 0x2A, 0xF9, 0x23, 0xC0, 0x7D, 0x26, 0x43, 0x1D, 0x0C, 0x9A, 0xBC

#elif defined(RADIO6)
static const uint64_t devEUI = 0x0C8B95AAAE6CFEFF;

static const uint64_t joinEUI = 0x0000000000000000;

static const std::uint8_t nwkKey[] = { 0x81, 0x83, 0xF4, 0xDD, 0x1B, 0x1F, 0xCF, 0x36, 0xC3, 0x73, 0x2A, 0xBE, 0x7C, 0xA2, 0x2F, 0x92 };

static const std::uint8_t appKey[] = { 0x59, 0xDC, 0x28, 0x58, 0xF6, 0x0C, 0x54, 0x70, 0x4E, 0x2E, 0xCB, 0x5E, 0x7B, 0x10, 0x79, 0x70 };

#elif defined(RADIO7)
// FireBeetleCover LoRa Radio #7
// pv-inverter-0

#define RADIOLIB_LORAWAN_DEV_EUI 0x70B3D57ED005CF56

// appeui, little-endian (lsb first)
///static const uint64_t joinEUI = 0x0000000000000000;

#define RADIOLIB_LORAWAN_NWK_KEY 0x2B, 0xA8, 0x2A, 0x2D, 0xCE, 0x8F, 0xC5, 0x01, 0x4A, 0xBB, 0xFA, 0x7A, 0xF7, 0x97, 0x1D, 0x3B

#define RADIOLIB_LORAWAN_APP_KEY 0xD6, 0xB3, 0x3F, 0xBB, 0x7B, 0x3B, 0x01, 0xC8, 0x04, 0x8A, 0xCF, 0x9E, 0x24, 0x12, 0x45, 0xC1

#elif defined(RADIO8)
// FireBeetleCover LoRa Radio #8
// pv-inverter-1

static const uint64_t devEUI[] = 0x70B3D57ED005D3B9;

static const uint64_t joinEUI = 0x0000000000000000;

static const std::uint8_t nwkKey[] = { 0x81, 0x83, 0xF4, 0xDD, 0x1B, 0x1F, 0xCF, 0x36, 0xC3, 0x73, 0x2A, 0xBE, 0x7C, 0xA2, 0x2F, 0x92 };

static const std::uint8_t appKey[] = { 0xB2, 0xA3, 0xD7, 0x16, 0xA6, 0x77, 0x8E, 0xEF, 0xBA, 0x57, 0xB7, 0xBE, 0x4F, 0xD7, 0xC4, 0x7E };

#elif defined(RADIO9)
// 98:76:B6:12:87:a1
static const uint64_t devEUI[] = 0x9876B600001287A1;

static const uint64_t joinEUI = 0x0000000000000000;

static const std::uint8_t nwkKey[] = { 0x81, 0x83, 0xF4, 0xDD, 0x1B, 0x1F, 0xCF, 0x36, 0xC3, 0x73, 0x2A, 0xBE, 0x7C, 0xA2, 0x2F, 0x92 };

static const std::uint8_t appKey[] = { 0x8F, 0xC5, 0x12, 0xE1, 0x22, 0xC8, 0xD0, 0xFC, 0xB7, 0x9F, 0x04, 0xC1, 0xAE, 0xC9, 0x51, 0x4C };

#endif
