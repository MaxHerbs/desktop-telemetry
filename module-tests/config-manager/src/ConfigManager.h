#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"



class ConfigManager {
public:
    ConfigManager(const char* _fileName, int _sdCs);

    int begin();
    char* readFile(fs::FS &fs, const char *path);
    
    JsonDocument json;

private:
    const char* fileName;
    int sdCs;

    void loadConfig(char* configText);

};

#endif // CONFIG_MANAGER_H




