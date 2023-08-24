
#include "esp_camera.h"
#include "img_converters.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#include <Preferences.h>


#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems

#define DEBUG 


Preferences preferences;

// Replace with your network credentials
const char* ssid = "Wir haben kein WLAN";
const char* password = "MarieMoritzGabi";

typedef struct {
    int x;
    int y;
} Location;

long millis_loop;

bool locate_request;

void setup() 
{    
  
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);
 
  preferences.begin("magic-fairy", false);
  preferences.putInt("effect_speed", 10);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  printIPAddress();
  
   
  initCamera();
  startWebserver();
  initLEDs(); 
  setLED(11, true);
  
}

void printIPAddress() {
  IPAddress ip = WiFi.localIP();
  Serial.print("ESP32 IP Address: ");
  Serial.println(ip);
}

void loop() 
{
  millis_loop = millis();
  EVERY_N_MILLISECONDS(30)
  {
    if(!isLocating())
    {
      handleEffects(millis_loop)    ;
    }
  }

  //From Webserver
  if(locate_request) 
  {
    locate_request = false;
    locateLEDs();
  }
}
