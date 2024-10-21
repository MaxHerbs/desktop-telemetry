#include "EtaEstimator.h"
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "ConfigManager.h"

float commute_distance;
int commute_time;

#define max_str_len 4096
#define mile_conversion 1609.34

// Constructor
EtaEstimator::EtaEstimator(ConfigManager &_configManager, String jsonBaseKey) : configManager(_configManager), jsonBaseKey(jsonBaseKey)
{
}

void EtaEstimator::begin()
{
    // build the json for the post request
    postBody = buildTemplate("/" + jsonBaseKey + "/postBody.txt", "postParams");
    Serial.println("Post Body");
    Serial.println(postBody);
    postHeaders = buildTemplate("/" + jsonBaseKey + "/headers.txt", "headerParams");
    Serial.println(postHeaders);
    SD.end();
}

// Updating the details
int EtaEstimator::updateCommuteDetails()
{
    String response = sendPostRequest();
    populateVariables(response);
    return 1;
}

void EtaEstimator::populateVariables(String response)
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
    commute_distance = responseJsonObj["routes"][0]["distanceMeters"].as<float>() / mile_conversion;

    String durationStr = responseJsonObj["routes"][0]["duration"];
    durationStr = durationStr.substring(0, durationStr.length() - 1); // Remove the last character
    commute_time = round(durationStr.toFloat() / 60.0);
}

String EtaEstimator::sendPostRequest()
{
    String response = "";
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        String endpoint = configManager.json[jsonBaseKey]["endpoint"].as<String>();
        Serial.println(endpoint);
        http.begin(endpoint);

        DynamicJsonDocument postJsonObj(2048);
        DeserializationError error = deserializeJson(postJsonObj, postHeaders);
        if (!error)
        {
            for (JsonPair kv : postJsonObj.as<JsonObject>())
            {
                const char *key = kv.key().c_str();
                const char *value = kv.value().as<const char *>();
                http.addHeader(key, value);
            }
        }
        else
        {
            Serial.printf("Failed to parse headers: %s\n", error.c_str());
        }

        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.POST(postBody);

        // Check for the response
        if (httpResponseCode > 0)
        {
            response = http.getString();
            Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        }
        else
        {
            Serial.printf("Error on sending POST: %s\n", http.errorToString(httpResponseCode).c_str());
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

int EtaEstimator::getCommuteTime()
{
    return commute_time; // TODO: make this work
}

float EtaEstimator::getCommuteDistance()
{
    return commute_distance; // TODO: make this work
}

String EtaEstimator::templateString(const String &templateStr, const String &placeholder, const String &value)
{
    String result = templateStr;
    result.replace(placeholder, value);
    return result;
}

String EtaEstimator::buildTemplate(String templateFileName, String paramKey)
{
    String thisTemplate = String(configManager.readFile(SD, templateFileName.c_str()));

    for (JsonPair pair : configManager.json[jsonBaseKey][paramKey].as<JsonObject>())
    {
        //    Serial.print("Key: ");
        //    Serial.print(pair.key().c_str());
        //    Serial.print(", Value: ");
        //    Serial.println(pair.value().as<String>());
        thisTemplate = templateString(thisTemplate, "{{ " + String(pair.key().c_str()) + " }}", pair.value().as<String>());
    }
    return thisTemplate;
}
