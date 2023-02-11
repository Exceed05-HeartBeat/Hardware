#include <Arduino.h>
#include <Bounce2.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "GET.h"
#define BUTTON 27
#define GREENRGB 32
#define REDRGB 26
#define BLUERGB 25
#define TWORED 13
#define TWOGREEN 12
#define samp_siz 4
#define rise_threshold 4
int sensorPin = 34;
Bounce b = Bounce();
int id;
int ch1  = 0;
int cnt = 0;
int res = 1000;
int timer = 0;
int beat;
int on = 0;
int state = 0;
int color;
float print_value;
float reads[samp_siz], sum;
long int now, ptr;
float   last, reader, start;
float first, second, third, before;
bool rising;
int rise_count;
int n;
long int last_beat;
TaskHandle_t TaskA = NULL;
TaskHandle_t TaskB = NULL;
TaskHandle_t TaskC = NULL;
TaskHandle_t TaskD = NULL;
TaskHandle_t TaskE = NULL;

void press_b(void *param);

void HeartBeat(void *param);

void LED_SHOW(void *param);

void mode_change(void *param);

void Connect_Wifi()
{
  const char *ssid = "Galaxy A30C9C8";
  const char *password = "himl9080";
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("OK! IP=");
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------");
}


void setup(){
    Serial.begin(115200);
    for (int i = 0; i < samp_siz; i++)
      reads[i] = 0;
    sum = 0;
    ptr = 0;
    b.attach(BUTTON,INPUT_PULLUP);
    b.interval(25); 
    pinMode(GREENRGB, OUTPUT);
    pinMode(REDRGB, OUTPUT);
    pinMode(BLUERGB, OUTPUT);
    pinMode(GREENRGB, OUTPUT);
    pinMode(TWORED, OUTPUT);
    pinMode(TWOGREEN, OUTPUT);
    Connect_Wifi();
    xTaskCreatePinnedToCore(HeartBeat, "HeartBeat", 1024*10, NULL, 1, &TaskB, 1);
    xTaskCreatePinnedToCore(press_b, "5sec_LED", 1024*10, NULL, 2, &TaskC, 1);
    xTaskCreatePinnedToCore(LED_SHOW, "LED_SHOW", 1024*10, NULL, 1, &TaskD, 1);
    xTaskCreatePinnedToCore(mode_change, "MODE_LED", 1024*20, NULL, 1, &TaskE, 0);
}

void loop(){
}

void LED_SHOW(void *param){
  while(1){
    if(on == 1) {
      color = GET_level();
      POST_beat(beat);
      //color = 0;
      if(color == 0){
        digitalWrite(GREENRGB, HIGH);
        digitalWrite(BLUERGB, LOW);
        digitalWrite(REDRGB, LOW);
      }
      else if(color == 2){
        digitalWrite(REDRGB, HIGH);
        digitalWrite(BLUERGB, LOW);
        digitalWrite(GREENRGB, LOW);
      }
      else if(color == 1){
        digitalWrite(BLUERGB, LOW);
        digitalWrite(GREENRGB, HIGH);
        digitalWrite(REDRGB, HIGH);
      }
      else{

      }
    } else {
        digitalWrite(BLUERGB, LOW);
        digitalWrite(GREENRGB, LOW);
        digitalWrite(REDRGB, LOW);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void mode_change(void *param){
    while(1){
      if(on == 1) {
        b.update();
        if(b.fell()){
          //Serial.println("1212121212");
          if(state == 0){
            state = 1;
            digitalWrite(TWORED, HIGH);
            digitalWrite(TWOGREEN, LOW);
            POST_mode(1);
            //Serial.println("1000000");
          }
          else{
            state = 0;
            digitalWrite(TWOGREEN, HIGH);
            digitalWrite(TWORED, LOW);
            POST_mode(0);
            //Serial.println("19999");
          }
          vTaskDelay(1000/portTICK_PERIOD_MS);
        } 
      } else {
          digitalWrite(TWOGREEN, LOW);
          digitalWrite(TWORED, LOW);
        }
      vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void press_b(void *param) {
  while(1) {
    //Serial.println(touchRead(2));
    if(touchRead(2) <=24){
      if(ch1==0){
        on = 1;
        POST_onoff(1);
        Serial.println(on);
        ch1 = 1;
      }
      else{
        on = 0;
        POST_onoff(0);
        Serial.println(on);
        ch1 = 0;
      }
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void HeartBeat(void *param){
    while(1){
      if(on == 1) {
          // calculate an average of the   sensor
          // during a 20 ms period (this will eliminate
          // the 50   Hz noise caused by electric light
          n = 0;
          start = millis();
          reader = 0.;
          do
          {
              reader += analogRead (sensorPin);
              n++;
              now = millis();
          }
          while (now < start +   20);  
          reader /= n;  // we got an average
          
          // Add the   newest measurement to an array
          // and subtract the oldest measurement from   the array
          // to maintain a sum of last measurements
          sum -= reads[ptr];
          sum += reader;
          reads[ptr] = reader;
          last = sum / samp_siz;
          // now last holds the average of the values in the array

          // check   for a rising curve (= a heart beat)
          if (last > before)
          {
              rise_count++;
              if (!rising && rise_count > rise_threshold)
              {
              //   Ok, we have detected a rising curve, which implies a heartbeat.
              //   Record the time since last beat, keep track of the two previous
              //   times (first, second, third) to get a weighed average.
              // The rising   flag prevents us from detecting the same rise more than once.
              rising   = true;
              first = millis() - last_beat;
              last_beat = millis();

              // Calculate the weighed average of heartbeat rate
              // according   to the three last beats
              print_value = 60000. / (0.4 * first + 0.3 *   second + 0.3 * third);
              if(print_value < 200 && print_value > 20) {
                beat = int(print_value);
              }
              if(print_value < 200 && print_value > 20) {
                Serial.print(print_value);
                Serial.println("");
              }

              
              third = second;
              second   = first;
              
              }
          }
          else
          {
              //   Ok, the curve is falling
              rising = false;
              rise_count = 0;
          }
          before = last;
          
          
          ptr++;
          ptr   %= samp_siz;
          
      }
      vTaskDelay(3/portTICK_PERIOD_MS);
    }
}