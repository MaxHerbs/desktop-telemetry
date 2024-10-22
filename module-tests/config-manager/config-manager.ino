#include "src/ConfigManager.h"
#include "src/EtaEstimator.h"
#include "src/WeatherMonitor.h"


ConfigManager myConfig("/config.json", 5);


void setup() {
  Serial.begin(115200);
  myConfig.begin();

  String ssid = myConfig.json["ssid"].as<String>();
  String password = myConfig.json["password"].as<String>();

  Serial.println(ssid);
  Serial.println(password);

  WiFi.begin(ssid, password);

  Serial.print("Connecting.");
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  EtaEstimator estimator(myConfig, "etaApi");
  estimator.begin();
  estimator.updateCommuteDetails();


  int commute_time = estimator.getCommuteTime();
  float commute_distance = estimator.getCommuteDistance();

  Serial.print("Commute time: ");
  Serial.println(commute_time);
  Serial.print("Commute distance: ");
  Serial.println(commute_distance);






  WeatherMonitor weatherMonitor(myConfig, "openWeatherApi");
  weatherMonitor.updateWeatherInfo();
  Serial.println("Weather...");
  Serial.println(weatherMonitor.getTemperature());
  Serial.println(weatherMonitor.getHumidity());

}

void loop() {
  // put your main code here, to run repeatedly:

}
