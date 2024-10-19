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



    int getCommuteDistance();
    float getCommuteTime();
    String getConfigParam(String key);


private:
    String postBody;
    String postHeaders;

    float commute_distance;
    int commute_time;
};

#endif // APIINTERFACE_H