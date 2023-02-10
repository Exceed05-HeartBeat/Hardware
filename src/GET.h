#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <string.h>
int x;
const String baseUrl = "https://ecourse.cpe.ku.ac.th/exceed05/hard/get_status";
//มี2get คือ ค่าของโหมด กับ ค่าของสีที่แสดง แดงเขียวเหลือง
//มี3post คือ ค่าของheartbeatทุกๆ5วิ กับ ค่าของโหมดเมื่อมีการเปลี่ยน และ on/off
int GET_mode()
{
  DynamicJsonDocument doc(65536);
  HTTPClient http;
  const String url = baseUrl;
  http.begin(url);

  int httpResponseCode = http.GET();
  if (httpResponseCode == 200)
  {
    String payload = http.getString();
    deserializeJson(doc, payload);
 
    Serial.println((const int)doc["result"]["current_status"]);
    return doc["result"]["current_status"];
  }
  else
  {
    Serial.print("Error ");
    Serial.println(httpResponseCode);
  }
}

int GET_level()
{
  DynamicJsonDocument doc(65536);
  HTTPClient http;
  const String url = baseUrl;
  http.begin(url);

  int httpResponseCode = http.GET();
  if (httpResponseCode == 200)
  {
    String payload = http.getString();
    deserializeJson(doc, payload);

    return doc["result"]["color"];
  }
  else
  {
    Serial.print("Error ");
    Serial.println(httpResponseCode);
  }
}

void POST_beat(int beat){
    String json;
    DynamicJsonDocument doc(65536);
    doc["current_heartbeat"] = beat;
    serializeJson(doc,json);

    const String url = baseUrl + "posts";
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type","application/json");

    int httpResponseCode = http.POST(json);
    if (httpResponseCode >= 200 && httpResponseCode < 300) {
        Serial.print("HTTP ");
        Serial.println(httpResponseCode);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
}

void POST_mode(int sta){
    String json;
    DynamicJsonDocument doc(65536);
    doc["current_status"] = sta;
    serializeJson(doc,json);

    const String url = baseUrl + "posts";
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type","application/json");

    int httpResponseCode = http.POST(json);
    if (httpResponseCode >= 200 && httpResponseCode < 300) {
        Serial.print("HTTP ");
        Serial.println(httpResponseCode);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
}

void POST_onoff(int on){
    String json;
    DynamicJsonDocument doc(65536);
    doc["is_on"] = on;
    serializeJson(doc,json);

    const String url = baseUrl + "posts";
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type","application/json");

    int httpResponseCode = http.POST(json);
    if (httpResponseCode >= 200 && httpResponseCode < 300) {
        Serial.print("HTTP ");
        Serial.println(httpResponseCode);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
}