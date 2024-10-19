#include "src/ApiInterface.h"

#define sdCsPin 5
ApiInterface myInterface(sdCsPin);

void setup() {
  Serial.begin(115200);
  myInterface.begin();

  String ssid = myInterface.getConfigParam("ssid");
  String password = myInterface.getConfigParam("password");
 
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  int status = myInterface.updateCommuteDetails();

  Serial.println(myInterface.getCommuteDistance());
  Serial.println(myInterface.getCommuteTime());
  
  
  


}

void loop() {
  // put your main code here, to run repeatedly:

}
