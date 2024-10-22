#include "ConfigManager.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define max_str_len 4096

ConfigManager::ConfigManager(const char *_fileName, int _sdCs)
{
    fileName = _fileName;
    sdCs = _sdCs;
    Serial.println(fileName);

    if (!SD.begin(_sdCs))
    {
        Serial.println("Card Mount Failed");
        return;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }
    Serial.print("SD card mounted with size: ");
    Serial.println(SD.cardSize() / (1024 * 1024));
}

int ConfigManager::begin()
{
    char *configString = readFile(SD, fileName);

    if (configString == nullptr)
    {
        Serial.println("Failed to read master config file");
        return -1;
    }
    loadConfig(configString);
    free(configString);

    return 0;
}

void ConfigManager::loadConfig(char *configText)
{
    DeserializationError error = deserializeJson(json, configText);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return; // Exit if deserialization fails
    }
}

// TOOLS
char *ConfigManager::readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return nullptr;
    }

    char *fileStream = (char *)malloc(max_str_len);
    if (!fileStream)
    {
        Serial.println("Failed to allocate memory");
        file.close();
        return nullptr;
    }

    size_t bytesRead = file.readBytes(fileStream, max_str_len);
    fileStream[bytesRead] = '\0'; // Null-terminate the string

    file.close();

    return fileStream;
}




String ConfigManager::templater(const String &templateStr, const String &placeholder, const String &value)
{
    String result = templateStr;
    result.replace(placeholder, value);
    return result;
}

String ConfigManager::buildTemplateFromFile(String templateFileName, String paramKey, String jsonBaseKey)
{
    String thisTemplate = String(readFile(SD, templateFileName.c_str()));

    return buildTemplateFromString(thisTemplate, paramKey, jsonBaseKey); 
}


String ConfigManager::buildTemplateFromString(String text, String paramKey, String jsonBaseKey)
{
    for (JsonPair pair : json[jsonBaseKey][paramKey].as<JsonObject>())
    {
        //    Serial.print("Key: ");
        //    Serial.print(pair.key().c_str());
        //    Serial.print(", Value: ");
        //    Serial.println(pair.value().as<String>());
        text = templater(text, "{{ " + String(pair.key().c_str()) + " }}", pair.value().as<String>());
    }
    return text;
}