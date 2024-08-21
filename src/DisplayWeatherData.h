
#if !defined(_DISPLAY_WEATHER_DATA_H)
#define _DISPLAY_WEATHER_DATA_H
#include <time.h>
#include <WeatherSensor.h>

void initDisplay(void);
void displayWeatherData(WeatherSensor::sensor_t sensor, RainGauge &rainGauge, time_t time);

#endif // _DISPLAY_WEATHER_DATA_H
