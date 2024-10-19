#include "ApiInterface.h"
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



// Constructor
ApiInterface::ApiInterface(int sd_card_pin) {
  _sd_card_pin = sd_card_pin;
}


void ApiInterface::begin(){
  if (!SD.begin(_sd_card_pin)) {
    Serial.println("Card Mount Failed");
    return;
  }

  // Handle the SD card
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);


  // Get the config from the JSON file
  String configText = readFile(SD, "/config.json");
  Serial.println("Read this from config file:");
  Serial.println(configText);



  Serial.println("Deserializing JSON to replace WiFi credentials...");

  DeserializationError error = deserializeJson(configDoc, configText);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  
  // build the json for the post request
  postBody = buildTemplate("/postBody.txt", "postParams");
  postHeaders = buildTemplate("/headers.txt", "headerParams");
  Serial.println("Post Body");
  Serial.println(postBody);
  Serial.println("Post Headers");
  Serial.println(postHeaders);

  SD.end();

}



// Updating the details
int ApiInterface::updateCommuteDetails(){
    String response = sendPostRequest();
    populateVariables(response);
    return 1;
}


void ApiInterface::populateVariables(String response) {
  DynamicJsonDocument responseJsonObj(2048);
  DeserializationError error = deserializeJson(responseJsonObj, response);

  if (error) {
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



String ApiInterface::sendPostRequest() {
  String response = "";
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String endpoint = getConfigParam("endpoint");
    http.begin(endpoint);


    DynamicJsonDocument postJsonObj(2048); // Adjust size as needed
    DeserializationError error = deserializeJson(postJsonObj, postHeaders);

    if (!error) {
      // Iterate over each key-value pair in the JSON
      for (JsonPair kv : postJsonObj.as<JsonObject>()) {
        const char* key = kv.key().c_str();
        const char* value = kv.value().as<const char*>();
        http.addHeader(key, value);
      }
    } else {
      Serial.printf("Failed to parse headers: %s\n", error.c_str());
    }


    http.addHeader("Content-Type", "application/json");




    int httpResponseCode = http.POST(postBody);

    // Check for the response
    if (httpResponseCode > 0) {
      response = http.getString();
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    } else {
      Serial.printf("Error on sending POST: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  return response;
}








// GETTERS
String ApiInterface::getConfigParam(String key){
    return configDoc[key];
}

int ApiInterface::getCommuteTime(){
    return commute_time; // TODO: make this work
}

float ApiInterface::getCommuteDistance(){
    return commute_distance; // TODO: make this work
}







// TOOLS
String ApiInterface::readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "";
  }

  char* fileStream = (char*)malloc(max_str_len + 1);
  if (!fileStream) {
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

String ApiInterface::templateString(const String& templateStr, const String& placeholder, const String& value) {
  String result = templateStr;
  result.replace(placeholder, value);
  return result;
}


String ApiInterface::buildTemplate(String templateFileName, String paramKey) {
  String thisTemplate = readFile(SD, templateFileName.c_str());

  for (JsonPair pair : configDoc[paramKey].as<JsonObject>()) {
    //    Serial.print("Key: ");
    //    Serial.print(pair.key().c_str());
    //    Serial.print(", Value: ");
    //    Serial.println(pair.value().as<String>());
    thisTemplate = templateString(thisTemplate, "{{ " + String(pair.key().c_str()) + " }}", pair.value().as<String>());

  }
  return thisTemplate;
}
