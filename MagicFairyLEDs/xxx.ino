
//const char* ssid = "Wir haben kein WLAN";
//const char* password = "MarieMoritzGabi";

/*
 * 
 * 
E (1631) camera: Camera probe failed with error 0x105(ESP_ERR_NOT_FOUND)
Camera init failed with error 0x105 
 */


 /*
  * 
  *void locateLEDs()
{
  Serial.println("locate LEDS");
  setAllLEDs(false);
  
  vTaskDelay(100);

  //printFBInfo(fb_old);

  for (int i = 8; i < 15; i++)
  {
    Serial.printf("---------LED %d-----------\n", i);
    takePicture(&fb_old);
    applyGaussianBlur(fb_old);
    printFB(fb_old);
    
    setLED(i, true);
    vTaskDelay(100);
    
    takePicture(&fb_new);
    applyGaussianBlur(fb_new);
    printFB(fb_new);
    
    setLED(i, false);
    vTaskDelay(100);

    int delta_max = subtractFB(fb_old, fb_new);
    printFB(fb_old);
    Serial.printf("Substraction, delta_max: %d\n", delta_max);
    
    led_location[i] = estimateLocation(fb_old, delta_max/2);
    printf("Location LED %d: (%d/%d)\n", i,led_location[i].x,led_location[i].y);
    //applyThreshold(fb_old,delta_max/2);
    //upscaleFbBrightness(fb_old, delta_max);
  }
}

    
  */
