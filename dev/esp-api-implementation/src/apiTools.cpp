#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

float commute_distance;
int commute_time;

#define max_str_len 4096
#define mile_conversion 1609.34

StaticJsonDocument<max_str_len> configDoc;
String postBody;
String postHeaders;

String getConfigParam(String key)
{
    return configDoc[key];
}

int setupApi()
{
    String configText = readFile(SD, "/config.json");
    Serial.println(configText);

    DeserializationError error = deserializeJson(configDoc, configText);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    const char *ssid = getConfigParam("ssid").c_str();         // Use const char*
    const char *password = configDoc("password").c_str(); // Use const char*

    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);

    WiFi.begin(ssid, password);
}

void populateGlobalVariables(String response)
{
    DynamicJsonDocument responseJsonObj(2048);
    DeserializationError error = deserializeJson(responseJsonObj, response);

    if (error)
    {
        Serial.print(F("Deserialisation of response failed: "));
        Serial.println(error.f_str());
        return;
    }

    commute_distance = responseJsonObj["routes"][0]["distanceMeters"].as<float>() / mile_conversion;

    String durationStr = responseJsonObj["routes"][0]["duration"];
    durationStr = durationStr.substring(0, durationStr.length() - 1); // Remove the last character
    commute_time = round(durationStr.toFloat() / 60.0);
}

String templateString(const String &templateStr, const String &placeholder, const String &value)
{
    String result = templateStr;
    result.replace(placeholder, value);
    return result;
}

String buildTemplate(String templateFileName, String paramKey)
{
    String thisTemplate = readFile(SD, templateFileName.c_str());

    for (JsonPair pair : configDoc[paramKey].as<JsonObject>())
    {
        //    Serial.print("Key: ");
        //    Serial.print(pair.key().c_str());
        //    Serial.print(", Value: ");
        //    Serial.println(pair.value().as<String>());
        thisTemplate = templateString(thisTemplate, "{{ " + String(pair.key().c_str()) + " }}", pair.value().as<String>());
    }
    return thisTemplate;
}

String readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return "";
    }

    char *fileStream = (char *)malloc(max_str_len + 1);
    if (!fileStream)
    {
        Serial.println("Failed to allocate memory");
        file.close();
        return "";
    }

    // Initialize fileStream with null characters
    memset(fileStream, 0, max_str_len + 1);

    // Read the file content into fileStream
    size_t bytesRead = file.readBytes(fileStream, max_str_len);
    fileStream[bytesRead] = '\0'; // Null-terminate the string

    file.close();

    String return_text = String(fileStream);
    free(fileStream);

    return return_text;
}