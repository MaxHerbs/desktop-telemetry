#ifndef WEATHER_MONITOR_H // Include guard
#define WEATHER_MONITOR_H

#include <Arduino.h> // Include the Arduino library for Arduino-specific functions
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "ConfigManager.h"

class WeatherMonitor
{
public:
    // Constructor
    WeatherMonitor(ConfigManager &_configManager, String jsonBaseKey);
    void begin();

    int updateWeatherInfo();

    float getTemperature();
    float getHumidity();

    String getConfigParam(String key);

private:
    String getBody;
    String jsonBaseKey;

    ConfigManager &configManager;

    String sendRequest();
    void populateVariables(String response);

};

#endif