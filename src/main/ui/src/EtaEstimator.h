#ifndef ETA_ESTIMATOR_H // Include guard
#define ETA_ESTIMATOR_H

#include <Arduino.h> // Include the Arduino library for Arduino-specific functions
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "ConfigManager.h"

class EtaEstimator
{
public:
    // Constructor
    EtaEstimator(ConfigManager &_configManager, String jsonBaseKey);
    void begin();

    int updateCommuteDetails();

    float getCommuteDistance();
    int getCommuteTime();
    // String getConfigParam(String key);

private:
    String postBody;
    String postHeaders;
    String jsonBaseKey;

    float commute_distance;
    int commute_time;

    ConfigManager &configManager;

    String sendPostRequest();
    void populateVariables(String response);

};

#endif