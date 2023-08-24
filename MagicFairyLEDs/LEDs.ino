
#define NUM_LEDS 100
#define DATA_PIN 14




CRGB leds[NUM_LEDS];
Location led_location[NUM_LEDS];

String led_effect;
int effect_speed, effect_size;
int overall_brightness;


void initLEDs()
{  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  led_effect = preferences.getString("led_effect", "Vertical sinus");
  effect_speed = preferences.getInt("effect_speed", 128);
  effect_size = preferences.getInt("effect_size", 128);
  preferences.getBytes("led_locations", led_location, NUM_LEDS * 8);
  overall_brightness = preferences.getInt("overall_brightness", 255);
}

void setOverallBrightness(int brightness)
{
  overall_brightness = brightness;
  preferences.putInt("overall_brightness", overall_brightness);
  FastLED.setBrightness(overall_brightness);
}

void setEffect(String effect_new)
{
  led_effect = effect_new;
  preferences.putString("effect", effect_new);
}
void setEffectSpeed(int e_speed)
{
  effect_speed = 255-e_speed;
  preferences.putInt("effect_speed", effect_speed);
}
void setEffectSize(int e_size)
{
  effect_size = 255-e_size;
  preferences.putInt("effect_size", effect_size);
}

void handleEffects(long millis_loop)
{
  if(led_effect=="Vertical sinus")
  {
    effectSinus(millis_loop, false);
  }
  else if(led_effect=="Radial sinus")
  {
    effectSinus(millis_loop, true);
  }
  else if(led_effect=="2D Noise")
  {
    effect2DNoise(millis_loop);
  }
}

void effectSinus(long millis_loop, bool radial)
{
    byte ef_speed = effect_speed/5;
    if(ef_speed == 0) ef_speed =1;
    
  for(int i = 0; i < NUM_LEDS; i++)
  {
    if(led_location[i].x == -1) continue;
    
    Location l = led_location[i];
    /*byte val = sin8(l.y*1.2 + millis_loop/10);
    val = dim8_lin (val);
    byte hue = (map(l.x,0,320,0,255)+millis_loop/100)%255;*/

    byte hue = getHue(l, millis_loop);
    
    byte val;
    
    if(radial)
    {
      //QVGA 320x240 --> center@ 160x120
      int radius = sqrt(sq(l.x-getDimension().x/2)+sq(l.y-getDimension().y/2));
      //val = sin8(radius * effect_size/64.0 - sin8(192+(millis_loop/ef_speed)));      
      val = sin8(radius * effect_size/64.0 - millis_loop/ef_speed);      
    }else
    {
      val = sin8(l.y * effect_size/64.0 + millis_loop/ef_speed);      
    }
    if(val<10) val=0;
    
    /*byte val = inoise8(l.x/2,l.y/2,millis_loop/40);
    val = map(hue,45,195, 0,255);
    val = dim8_lin (val);*/
    
    leds[i] = CHSV(hue,255,val);
  }
  FastLED.show();
}


void effect2DNoise(long millis_loop)
{
    byte ef_speed = effect_speed/5;
    if(ef_speed == 0) ef_speed =1;
    
  for(int i = 0; i < NUM_LEDS; i++)
  {
    if(led_location[i].x == -1) continue;
    
    Location l = led_location[i];    
    byte hue = inoise8(l.x*(effect_size/64.0),l.y*(effect_size/64.0), millis_loop/(ef_speed));
    //val = map(hue,45,195, 0,255);
    //val = dim8_lin (val);
    
    leds[i] = CHSV(hue,255,180);
  }
  FastLED.show();
}

byte getHue(Location l, long millis_loop)
{ 
    //byte hue = inoise8(l.x,l.y,millis_loop/10);
    //hue = map(hue,45,195, 0,255);
    //byte hue = inoise8(millis_loop/20);
    
    int radius = sqrt(sq(l.x-getDimension().x/2)+sq(l.y-getDimension().y/2));
    byte hue = inoise8(radius * effect_size/32, millis_loop/20);
  return hue;
}

void animateLEDs()
{  
  
}

void setLEDLocation(int led_index, int x, int y)
{
  led_location[led_index]={x,y};
}

void storeLEDLocations()
{
  preferences.putBytes("led_locations", led_location, NUM_LEDS * 8);
}

void setLED(int index, bool lit)
{
  if(lit)
  {
    leds[index] = CRGB(255,255,255);    
  }else
  {
    leds[index] = CRGB(0,0,0);    
  }
  FastLED.show();
}

void setAllLEDs(bool lit)
{
  if(lit)
  {
    for(int i = 0; i < NUM_LEDS; i++) 
      leds[i] = CRGB(255,255,255);
  }else
  {    
    FastLED.clear();
  }
  FastLED.show();
}
