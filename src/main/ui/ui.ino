#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include "src/ConfigManager.h"
#include "src/EtaEstimator.h"
#include "src/WeatherMonitor.h"


#define sdCsPin 5
ConfigManager myConfig("/config.json", sdCsPin);
EtaEstimator estimator(myConfig, "etaApi");
WeatherMonitor weatherMonitor(myConfig, "openWeatherApi");

int updateFrequency = 5000;
int prevUpdate = -updateFrequency;


static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */



int currScreen = 0;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  tft.startWrite();
  tft.setAddrWindow( area->x1, area->y1, w, h );
  tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  tft.endWrite();

  lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
  uint16_t touchX = 0, touchY = 0;

  bool touched = false;//tft.getTouch( &touchX, &touchY, 600 );

  if ( !touched )
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print( "Data x " );
    Serial.println( touchX );

    Serial.print( "Data y " );
    Serial.println( touchY );
  }
}

void setup()
{
  Serial.begin( 115200 ); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println( LVGL_Arduino );
  Serial.println( "I am LVGL_Arduino" );

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

  tft.begin();          /* TFT init */
  tft.setRotation( 0 ); /* Landscape orientation, flipped */

  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register( &indev_drv );


  ui_init();

  Serial.println( "Setup done" );
  Serial.println("Setting up api");

  myConfig.begin();

  String ssid = myConfig.json["ssid"].as<String>();
  String password = myConfig.json["password"].as<String>();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");



  estimator.begin();
  weatherMonitor.begin();
  weatherMonitor.updateWeatherInfo();
  estimator.updateCommuteDetails();





  Serial.println(weatherMonitor.getConditions());



}


float switchCounter = prevUpdate;
void loop()
{
  if (millis() - switchCounter > updateFrequency) {
    changeScreen();
    switchCounter = millis();
  }


  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}


void changeScreen() {
  if (currScreen > 1) {
    currScreen = 0;
  }

  if (currScreen == 0) {
    lv_scr_load(ui_commuteScreen);
    setupCommuteScreen();
  } else if (currScreen == 1) {
    lv_scr_load(ui_weatherScreen);
    setupWeatherScreen();
  }
  currScreen++;

}



void setupCommuteScreen() {
  Serial.println("Setting up commute screen...");
  char charTime[32];
  int commuteTime = estimator.getCommuteTime();

  if (commuteTime > 60) {
    int hr = 0;
    int mins = 0;
    while (commuteTime > 60) {
      hr += 1;
      commuteTime -= 60;
    }
    mins = commuteTime;
    sprintf(charTime, "%d hr %d mins", hr, mins);

  } else {
    sprintf(charTime, "%d minutes", estimator.getCommuteTime());
  }

  lv_label_set_text(ui_time, charTime);


  char charDist[32];
  sprintf(charDist, "%0.1f miles", estimator.getCommuteDistance());
  lv_label_set_text(ui_distance, charDist);
  Serial.println("Finished setting up commute screen");
}



void setupWeatherScreen() {
  Serial.println("Setting up weather screen...");
  char charTemp[32];
  float temperature = weatherMonitor.getTemperature();
  sprintf(charTemp, "%.02f c", temperature);
  lv_label_set_text(ui_temperature, charTemp);

    char charHumidity[32];
  float humidity = weatherMonitor.getHumidity();
  sprintf(charTemp, "%.02f %", humidity);
  lv_label_set_text(ui_humidity, charTemp);


  char charConditions[32];
  sprintf(charConditions, "%s", weatherMonitor.getConditions());
  lv_label_set_text(ui_conditions, charConditions);
  Serial.println("Finished setting up weather screen");
}
