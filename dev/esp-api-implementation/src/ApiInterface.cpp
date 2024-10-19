#include "MyClass.h"
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

  if (!SD.begin(sd_card_pin)) {
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





// GETTERS
String ApiInterface::getConfigParam(String key){
    return configDoc[key];
}

int ApiInterface::getCommuteTime(){
    return 10; // TODO: make this work
}

float ApiInterface::getCommuteDistance(){
    return 4.5; // TODO: make this work
}







// TOOLS
static String readFile(fs::FS &fs, const char *path) {
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

static String templateString(const String& templateStr, const String& placeholder, const String& value) {
  String result = templateStr;
  result.replace(placeholder, value);
  return result;
}


static String buildTemplate(String templateFileName, String paramKey) {
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
