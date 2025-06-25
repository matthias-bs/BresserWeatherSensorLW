#include <Arduino.h>
#include <DallasTemperature.h>

#define PIN_ONEWIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
static OneWire oneWire(PIN_ONEWIRE_BUS); //!< OneWire bus

// Pass our oneWire reference to Dallas Temperature.
static DallasTemperature owTempSensors(&oneWire); //!< Dallas temperature sensors connected to OneWire bus

// Get temperature from Maxim OneWire Sensor
float getOneWireTemperature(uint8_t index)
{
    // Call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    owTempSensors.requestTemperatures();

    // Get temperature by index
    float tempC = owTempSensors.getTempCByIndex(index);

    // Check if reading was successful
    if (tempC != DEVICE_DISCONNECTED_C)
    {
        log_i("Temperature = %.2f°C", tempC);
    }
    else
    {
        log_i("Error: Could not read temperature data");
    }

    return tempC;
};

void setup()
{
    // Initialize the Dallas Temperature library
    owTempSensors.begin();
}

void loop()
{
    // Get temperature from the first sensor (index 0)
    float temperature = getOneWireTemperature(0);

    // Do something with the temperature value
    log_i("Temperature from OneWire sensor: %.2f°C", temperature);

    // Wait for a while before the next reading
    delay(5000);
}