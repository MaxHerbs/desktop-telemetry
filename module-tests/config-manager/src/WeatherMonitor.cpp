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

#define max_str_len 4096
#define mile_conversion 1609.34

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
    String response = sendRequest();
    populateVariables(response);
    return 1;
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
    return temperature; // TODO: make this work
}

float WeatherMonitor::getHumidity()
{
    return humidity; // TODO: make this work
}
