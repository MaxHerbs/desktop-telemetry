#ifndef APIINTERFACE_H // Include guard
#define APIINTERFACE_H

#include <Arduino.h> // Include the Arduino library for Arduino-specific functions
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>


class ApiInterface
{
public:
    // Constructor
    ApiInterface(int sd_card_pin);
    void begin();

    int updateCommuteDetails();

    float getCommuteDistance();
    int getCommuteTime();
    String getConfigParam(String key);



private:
    int _sd_card_pin;
    String postBody;
    String postHeaders;

    float commute_distance;
    int commute_time;


    String sendPostRequest();
    void populateVariables(String response);

    String readFile(fs::FS &fs, const char *path);
    String templateString(const String& templateStr, const String& placeholder, const String& value);
    String buildTemplate(String templateFileName, String paramKey);
};

#endif // APIINTERFACE_H