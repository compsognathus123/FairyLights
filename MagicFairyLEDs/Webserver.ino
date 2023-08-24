#include "Website.h"

#define CONFIG_ASYNC_TCP_USE_WDT 0

// Create an instance of the web server
AsyncWebServer server(80);

uint8_t *jpeg_buf = NULL;
size_t jpeg_len = 0;

// HTML page served by the web server
/*const char htmlPage[] PROGMEM = R"rawliteral(
 <html>
     <head>
       <title>Magic Fairy LEDs</title>
       <script>
         function locateLEDs() {
           var xhr = new XMLHttpRequest();
           xhr.open('GET', '/locate', true);
           xhr.send();
           location.reload();
         }
         function updatePreview()
         {       
          var image = document.getElementById("previewImage"); 
          var timestamp = new Date().getTime();
          image.src = "/cam_preview?timestamp=" + timestamp; 
         }
         setInterval(function() {
           var previewImage = document.getElementById('previewImage');
           previewImage.src = '/cam_preview?t=' + new Date().getTime();
         }, 1000);
       </script>
     </head>
     <body>
       <h2>Magic Fairy LEDs</h2>
       <img id="previewImage" onclick="updatePreview()" src="/cam_preview">
       <br>Click image to update.<br>
       <button type="button" onclick="locateLEDs()">Locate LEDs</button>
     </body>
   </html>)rawliteral";*/

void startWebserver()
{
  //Location
  server.on("/cam_preview", HTTP_GET, handleCameraPreview);
  server.on("/locate", HTTP_GET, handleLocateLEDs);
  //Effects
  server.on("/set_settings", HTTP_GET, handleSetSettings);
  
  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(200, "text/html", htmlPage);
  });

  server.begin();
  Serial.println("Web server started");
}

void handleSetSettings(AsyncWebServerRequest *request)
{  
  if(request->hasParam("effect"))
  {
    String effect = request->getParam("effect")->value();
    setEffect(effect);
    Serial.print("GET effect ");
    Serial.println(effect);
    request->send(200);
  }
  else if(request->hasParam("speed"))
  {
    int effect_speed = request->getParam("speed")->value().toInt();
    setEffectSpeed(effect_speed);
    Serial.printf("GET effect_speed %d\n", effect_speed);
    request->send(200);
  }  
  else if(request->hasParam("size"))
  {
    int effect_size = request->getParam("size")->value().toInt();
    setEffectSize(effect_size);
    Serial.printf("GET effect_size %d\n", effect_size);
    request->send(200);
  } 
  else if(request->hasParam("overall_brightness"))
  {
    int overall_brightness = request->getParam("overall_brightness")->value().toInt();
    setOverallBrightness(overall_brightness);
    Serial.printf("GET overall_brightness %d\n", overall_brightness);
    request->send(200);
  }  
  else if(request->hasParam("crop"))
  {
    String crop = request->getParam("crop")->value();
    setCrop(crop == "true");
    Serial.printf("GET crop %d\n", (crop == "true"));
    request->send(200);
  } 
  request->send(404);
}

void handleCameraPreview(AsyncWebServerRequest *request)
{
  if(isLocating())
  {
    request->send(500, "text/plain", "Locating...");    
    return;
  }
  
  camera_fb_t *fb_gray = NULL;
  takePicture(&fb_gray);
  Serial.println("Took preview picture.");

  if (fb_gray) 
  {
    free(jpeg_buf);
    bool success = frame2jpg(fb_gray, 80, &jpeg_buf, &jpeg_len);
    esp_camera_fb_return(fb_gray);
    Serial.printf("Converted to JPEG: %d len: %zu\n", success);
  }
  if(jpeg_buf != NULL)
  {
    Serial.printf("Send JPEG response.\n");
    AsyncWebServerResponse *response = request->beginChunkedResponse("image/jpg", [](uint8_t *buffer, size_t maxLen, size_t alreadySent) -> size_t 
    {
      //if(isLocating()) return 0;
      if (jpeg_len - alreadySent >= maxLen) {
        memcpy(buffer, jpeg_buf + alreadySent, maxLen);
        return maxLen;
      } else {  // last chunk and then 0
        memcpy(buffer, jpeg_buf + alreadySent, jpeg_len - alreadySent);
        return jpeg_len - alreadySent;
      }
      //yield();
    });
    request->send(response);
    //Serial.println("sent image request");
  } else {
    request->send(500, "text/plain", "Camera capture failed");
  }

}



// Function to handle HTTP GET requests for capturing a picture
void handleLocateLEDs(AsyncWebServerRequest *request) {
  request->send(200);
  if(!isLocating()) locate_request = true;  
}
