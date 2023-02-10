#include <Arduino.h>
#include <Bounce2.h>
#include <WiFi.h>
#include "GET.h"
#define BLUE 5
#define RED 26
#define BUTTON 27
#define samp_siz 4
#define rise_threshold 4
int sensorPin = 33;
Bounce b = Bounce();
int id;
int cnt = 0;
int res = 1000;
int timer = 0;
int beat;
float print_value;
TaskHandle_t TaskA = NULL;
TaskHandle_t TaskB = NULL;
TaskHandle_t TaskC = NULL;

void sec(void *param);

void post_b(void *param);

void HeartBeat(void *param);

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
    b.attach(BUTTON,INPUT_PULLUP);
    b.interval(25); 
    ledcSetup(0, 5000, 8);
    ledcAttachPin(BLUE, 0);
    pinMode(RED, OUTPUT);
    digitalWrite(RED,0);
    Connect_Wifi();
    xTaskCreatePinnedToCore(post_b, "post_beat5s", 10000, NULL, 2, &TaskA, 1);
    xTaskCreatePinnedToCore(HeartBeat, "HeartBeat", 1000, NULL, 1, &TaskB, 0);
    xTaskCreatePinnedToCore(sec, "5sec_LED", 1000, NULL, 1, &TaskC, 1);
}

void loop(){
}

void sec(void *param) {
  while(1) {
    b.update();
    if ( b.fell() ) { 
      cnt++;
    }
    if(cnt%2==0) {
      digitalWrite(RED,0);
    }
    else if(cnt%2==1 && digitalRead(RED) == 0) {
      digitalWrite(RED,1);
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

void post_b(void *param){
    while(1){
      beat = int(print_value);
      POST_beat(beat);
      vTaskDelay(5000/portTICK_PERIOD_MS);
    }
}

void HeartBeat(void *param){
    while(1){
      float reads[samp_siz], sum;
        long int now, ptr;
        float   last, reader, start;
        float first, second, third, before;
        bool rising;
        int rise_count;
        int n;
        long int last_beat;

        for (int i = 0; i < samp_siz; i++)
        reads[i] = 0;
        sum = 0;
        ptr = 0;

        while(1)
        {
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
            
            Serial.print(print_value);
            Serial.println("");
            
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
    }
}