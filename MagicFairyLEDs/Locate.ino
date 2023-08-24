
//Gaussian Blur
#define FIXED_POINT_SHIFT 8 // Number of fractional bits for fixed-point arithmetic
int gaussianKernel[3][3] = {
  {16, 32, 16},
  {32, 64, 32},
  {16, 32, 16}
};

camera_fb_t *fb_old;
camera_fb_t *fb_new;

#define SCALE_WIDTH 320
#define SCALE_HEIGHT 240

bool locating;
bool crop;
Location dimension = {0,0};

Location getDimension()
{
  if(dimension.x == 0 && dimension.y == 0) 
     preferences.getBytes("dimension", &dimension, sizeof(Location));
  return dimension;
}
void setDimension(Location l)
{
  preferences.putBytes("dimension", &l, sizeof(Location));
  dimension=l;
}

bool isLocating(){
  return locating;
}

void setCrop(bool new_crop)
{
  crop=new_crop;
}

void locateLEDs()
{
  Serial.println("locate LEDS");
  locating = true;
  setAllLEDs(false);
  
  vTaskDelay(100);

  //printFBInfo(fb_old);
    takePicture(&fb_old);
    applyGaussianBlur(fb_old);

    int delta_max[NUM_LEDS] = {0};

  for (int i = 0; i < NUM_LEDS; i++)
  {
    //Serial.printf("---------LED %d-----------\n", i);
    
    setLED(i, true);
    vTaskDelay(10);
    
    takePicture(&fb_new);
    applyGaussianBlur(fb_new);
    //printFB(fb_new);
    
    setLED(i, false);
    vTaskDelay(10);

    delta_max[i] = subtractFB(fb_old, fb_new);
    //printFB(fb_old);
    //Serial.printf("Substraction, delta_max: %d\n", delta_max[i]);
    
    led_location[i] = estimateLocation(fb_old, delta_max[i]/2);
    printf("Location LED %d: (%d/%d) delta_max %d\n", i,led_location[i].x,led_location[i].y, delta_max[i]);
    //applyThreshold(fb_old,delta_max/2);
    //upscaleFbBrightness(fb_old, delta_max);
    camera_fb_t *fb_switch = fb_old;
    fb_old = fb_new;
    fb_new = fb_switch;
    
    yield();
  }
  returnFB(fb_old);
  returnFB(fb_new);
  
  markMissingLEDs(delta_max);
  if(crop)
  {
    Location l = cropLEDLocations();//Crop and Scale to QVGA --> mind the aspect ratio when capturing in different resolution!
    Serial.printf("dimension %d/%d\n", l.x,l.y);
    setDimension(l);
  }else
  {
    setDimension({SCALE_WIDTH,SCALE_HEIGHT});
    Serial.printf("dimension %d/%d\n", SCALE_WIDTH,SCALE_HEIGHT);
  }
  storeLEDLocations();
  
  locating = false;  
}

void markMissingLEDs(int *delta_max)
{
  int delta_max_mean = 0;
  for(int i = 0; i < NUM_LEDS; i++)
  {
    delta_max_mean +=delta_max[i];
  }
  delta_max_mean /= NUM_LEDS;
  Serial.printf("Delta_max_mean %d\n", delta_max_mean);
  for(int i = 0; i < NUM_LEDS;i++)
  {
    if(delta_max[i] < delta_max_mean/2)
    {
      led_location[i] = {-1,-1};
      Serial.printf("LED %d is missing\n", i);
    }
  }
}

Location cropLEDLocations()
{
  int x_min=9999,x_max=0;
  int y_min=9999,y_max=0;
  
  for(int i = 0; i < NUM_LEDS;i++)
  {
    if(led_location[i].x == -1) continue;
    
    if(led_location[i].x < x_min)
    {
      x_min = led_location[i].x;
    }else if(led_location[i].x > x_max)
    {
      x_max = led_location[i].x;      
    }
    if(led_location[i].y < y_min)
    {
      y_min = led_location[i].y;
    }else if(led_location[i].y > y_max)
    {
      y_max = led_location[i].y;      
    }
  }
  Serial.printf("x_min: %d x_max %d, y_min %d, y_max %d\n", x_min,x_max,y_min,y_max);

  int delta_x = x_max-x_min;
  int delta_y = y_max-y_min;
  
  //Vertically or horizontally dominant LED arrangement
  float x_scale = SCALE_WIDTH/delta_x;
  float y_scale = SCALE_HEIGHT/delta_y;
  float scale = x_scale;
  if(y_scale>x_scale) scale=y_scale;

  for(int i = 0; i < NUM_LEDS; i++)
  {
    if(led_location[i].x == -1) continue;
    led_location[i].x = (led_location[i].x-x_min) * scale;
    led_location[i].y = (led_location[i].y-y_min) * scale;
  }

  return {delta_x*scale, delta_y*scale};
}

//Subtracts fb_2-fb_1. Result is stored in fb_1. Returns max_vaule for treshold
int subtractFB(camera_fb_t *fb_1, camera_fb_t *fb_2)
{
  if (fb_1 == NULL || fb_2 == NULL) {
    Serial.printf("Cannot subtract, FB is NULL: %d %d\n", fb_1, fb_2);
    return 0;
  } else if (fb_1->len != fb_2->len) {
    Serial.printf("FB Length doesnt match: %d %d\n", fb_1->len, fb_2->len);
    return 0;
  }

  byte delta_max = 0;
  byte *val1;
  byte *val2;
  for (int i = 0; i < fb_1->len; i++)
  {
    /*//If fb_2 > fb_1, substract. if not, result is zero
      int val1 = *(fb_1->buf + i);
      int val2 = *(fb_2->buf + i);
      int result = *(fb_2->buf + i) - *(fb_1->buf + i);
      if(i < 300) Serial.printf("%d - %d = %d\n", val2, val1, result);

      (fb_1->buf + i) = *(fb_2->buf + i) >  *(fb_1->buf + i) ? *(fb_2->buf + i) - *(fb_1->buf + i) : 0;
      if(*(fb_1->buf + i) > max_value) max_value=*(fb_1->buf + i);*/

    val1 = fb_1->buf + i;
    val2 = fb_2->buf + i;

    *val1 = *val2 >  *val1 ? *val2 - *val1 : 0;
    if (*val1 > delta_max) delta_max = *val1;
  }
  return delta_max;
}

void applyGaussianBlur(camera_fb_t *fb)
{
  int width = fb->width;
  int height = fb->height;
  int row, col, kRow, kCol;
  uint8_t *buf_blurred = (uint8_t *)calloc(width * height, sizeof(uint8_t));
  //Serial.printf("malloc success: %d\n", buf_blurred);

  for (row = 1; row < height - 1; row++) {
    for (col = 1; col < width - 1; col++) {
      int sum = 0;
      int pixelIndex = (row * width + col);

      for (kRow = -1; kRow <= 1; kRow++) {
        for (kCol = -1; kCol <= 1; kCol++) {
          int kernelIndex = (kRow + 1) * 3 + (kCol + 1);
          int neighborIndex = ((row + kRow) * width + (col + kCol));

          sum += fb->buf[neighborIndex] * gaussianKernel[kRow + 1][kCol + 1];
        }
      }

      buf_blurred[pixelIndex] = (uint8_t)(sum >> FIXED_POINT_SHIFT);
    }
  }
  free(fb->buf);
  fb->buf = buf_blurred;
}

void applyThreshold(camera_fb_t *fb, int threshold)
{
  for(int i = 0; i < fb->len; i++)
  {
    if(fb->buf[i] < threshold) fb->buf[i] = 0;
  }
}

Location estimateLocation(camera_fb_t *fb, int threshold)
{
  int pixel_x,pixel_y;
  int pixel_weight;
  int led_x = 0;
  int led_y = 0;
  int weights=0;
  
  for(int i = 0; i < fb->len; i++)
  {
    if(fb->buf[i] > threshold)
    {
      pixel_y = i/fb->width;
      pixel_x = i - pixel_y*fb->width;
      //Serial.printf("Potential LED Pixel at %d/%d\n", pixel_x, pixel_y);

      pixel_weight = fb->buf[i]-threshold;
      float combined_weights = (weights+pixel_weight);
      //Serial.printf("%f = %d + %d\n", combined_weights,weights,pixel_weight);
      
      led_x = led_x *(weights/combined_weights) + pixel_x * (pixel_weight/combined_weights);
      led_y = led_y *(weights/combined_weights) + pixel_y * (pixel_weight/combined_weights);
      weights += pixel_weight;
      //Serial.printf("Estimated Location %d/%d\n", led_x, led_y);

    }else
    {
      fb->buf[i] = 0;
    }
  }
  
  //Return Estimated LED Location
  return {led_x,led_y};
}

void printFB(camera_fb_t *fb)
{
  for (int y = 50; y < 51; y++)
  {
    for (int x = 0; x < fb->width; x++)
    {
      Serial.printf("%d ", *(fb->buf + y * fb->width + x));
    }
    Serial.printf("\n");
  }
}

void upscaleFbBrightness(camera_fb_t *fb, int max_value)
{
  float factor = 255.0/max_value;
  for(int i = 0; i < fb->len; i++)
  {
    fb->buf[i] *= factor;
  }
}

camera_fb_t *getFbDebug()
{
  return fb_old;
}
