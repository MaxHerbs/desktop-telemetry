#include "WeatherMonitor.h"
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "ConfigManager.h"

float temperature;
float humidity;
String conditions;

#define max_str_len 4096
#define mile_conversion 1609.34

int update_period = 60000;
int prev_update = -update_period;

// Constructor
WeatherMonitor::WeatherMonitor(ConfigManager &_configManager, String jsonBaseKey) : configManager(_configManager), jsonBaseKey(jsonBaseKey)
{
}

void WeatherMonitor::begin()
{
}

// Updating the details
int WeatherMonitor::updateWeatherInfo()
{
    if (millis() - prev_update > update_period)
    {
        Serial.println("Enough time passed since last update, updating weather info");
        String response = sendRequest();
        populateVariables(response);
        prev_update = millis();
        return 1;
    }
    Serial.println("Not enough time passed since last update, not updating weather info");
    return 0;
}

void WeatherMonitor::populateVariables(String response)
{
    DynamicJsonDocument responseJsonObj(2048);
    DeserializationError error = deserializeJson(responseJsonObj, response);

    if (error)
    {
        Serial.print(F("Deserialisation of response failed: "));
        Serial.println(error.f_str());
        return;
    }
    Serial.println(response);
    temperature = responseJsonObj["main"]["temp"].as<float>();
    humidity = responseJsonObj["main"]["humidity"].as<float>();
    conditions = responseJsonObj["weather"][0]["main"].as<String>();
}
String WeatherMonitor::sendRequest()
{
    String response = "";
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Retrieve the endpoint URL from the configuration
        String endpoint = configManager.json[jsonBaseKey]["endpoint"].as<String>();
        Serial.println(endpoint);

        endpoint = configManager.buildTemplateFromString(endpoint, "getParams", jsonBaseKey);

        // Initialize the HTTP GET request
        http.begin(endpoint);
        http.addHeader("Content-Type", "application/json");

        // Sending the GET request
        int httpResponseCode = http.GET();

        // Check the response code
        if (httpResponseCode > 0)
        {
            response = http.getString(); // Get the response content
            Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        }
        else
        {
            Serial.printf("Error on sending GET: %s\n", http.errorToString(httpResponseCode).c_str());
        }

        // Free resources
        http.end();
    }
    else
    {
        Serial.println("WiFi not connected");
    }

    return response;
}

// GETTERS

float WeatherMonitor::getTemperature()
{
    return temperature;
}

float WeatherMonitor::getHumidity()
{
    return humidity;
}

String WeatherMonitor::getConditions()
{
    return conditions;
}
