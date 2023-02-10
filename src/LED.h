#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Bounce2.h>
#include "GET.h"
#define GREENRGB 9
#define REDRGB 11
#define BLUERGB 10
#define TWORED 32
#define TWOGREEN 33
#define BUTTON 27
Bounce debouncer = Bounce();
int state = 0; //state 0 -> ปกติ state 1 -> ออกกำลังกาย

void setup() {
  // put your setup code here, to run once:
    pinMode(GREENRGB, OUTPUT);
   pinMode(REDRGB, OUTPUT);
   pinMode(BLUERGB, OUTPUT);
   pinMode(GREENRGB, OUTPUT);
   pinMode(TWORED, OUTPUT);
   pinMode(TWOGREEN, OUTPUT);
   digitalWrite(BLUERGB, HIGH);
   digitalWrite(TWOGREEN, HIGH);
   debouncer.attach(BUTTON, INPUT_PULLUP);
    debouncer.interval(25); 
    Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
}
void LED_SHOW(){
  int color;
  color = GET_level();
  if(color == 1){
    digitalWrite(GREENRGB, HIGH);
    digitalWrite(BLUERGB, LOW);
    digitalWrite(REDRGB, LOW);
  }
  else if(color == 2){
    digitalWrite(REDRGB, HIGH);
    digitalWrite(BLUERGB, LOW);
    digitalWrite(GREENRGB, LOW);
  }
  else{
    digitalWrite(BLUERGB, LOW);
    digitalWrite(GREENRGB, HIGH);
    digitalWrite(REDRGB, HIGH);
  }

}
void mode_change(){
    while(1){
        debouncer.update();
        if(debouncer.fell()){
            if(state == 0){
                state = 1;
                digitalWrite(TWORED, HIGH);
                digitalWrite(TWOGREEN, LOW);
                POST_mode(1);
            }
            else{
                state = 0;
                digitalWrite(TWOGREEN, HIGH);
                digitalWrite(TWORED, LOW);
                POST_mode(0);
            }
        }
    }
}