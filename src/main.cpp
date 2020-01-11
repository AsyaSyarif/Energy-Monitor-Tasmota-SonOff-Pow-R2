// Written by Arif 2K20
// asyarif.net


#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>


WiFiClient wifi;
MQTTClient mqtt(512); // 512 means maximum size of mqtt Payload 

#define _BAUDRATE       115200
#define _WIFI_SSID      "ARIF"
#define _WIFI_PASSWORD  ""
#define _MQTT_SERVER    "asyarif.net"
#define _MQTT_PORT      1883
#define _MQTT_USER      "arif"
#define _MQTT_PASSWORD  "kmzwa8awaa"
#define _MQTT_PUB_QOs   2                           
#define _MQTT_SUB_QOs   2


//Topic Publish 
#define _MQTT_PUB_TOPIC_REQUEST     "cmnd/powerr2/status8"
#define _MQTT_TOPIC_PAYLOAD_REQUEST  "8"

//Topic Subscribe
#define _MQTT_SUB_TOPIC_RESULT  "stat/powerr2/STATUS8"

unsigned long lastMillis = 0;
#define intervalPublish  3000 //3 senconds


void messageReceived(String &topic, String &payload) {
  // Serial.println("incoming: " + topic + " - " + payload);

  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(10) + 180;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, payload);

  const char* StatusSNS_Time = doc["StatusSNS"]["Time"]; // "1970-01-02T15:46:17"

  JsonObject StatusSNS_ENERGY = doc["StatusSNS"]["ENERGY"];
  const char* StatusSNS_ENERGY_TotalStartTime = StatusSNS_ENERGY["TotalStartTime"]; // "1970-01-01T00:00:00"
  float StatusSNS_ENERGY_Total = StatusSNS_ENERGY["Total"]; // 5.239
  int StatusSNS_ENERGY_Yesterday = StatusSNS_ENERGY["Yesterday"]; // 0
  float StatusSNS_ENERGY_Today = StatusSNS_ENERGY["Today"]; // 5.239
  int StatusSNS_ENERGY_Power = StatusSNS_ENERGY["Power"]; // 142
  int StatusSNS_ENERGY_ApparentPower = StatusSNS_ENERGY["ApparentPower"]; // 155
  int StatusSNS_ENERGY_ReactivePower = StatusSNS_ENERGY["ReactivePower"]; // 62
  float StatusSNS_ENERGY_Factor = StatusSNS_ENERGY["Factor"]; // 0.92
  int StatusSNS_ENERGY_Voltage = StatusSNS_ENERGY["Voltage"]; // 214
  float StatusSNS_ENERGY_Current = StatusSNS_ENERGY["Current"]; // 0.724

  Serial.println("------------------------");

  Serial.print("Time: ");
  Serial.println(StatusSNS_Time);


  Serial.print("Power: ");
  Serial.print(StatusSNS_ENERGY_Power);
  Serial.println(" W");

  Serial.print("Voltage: ");
  Serial.print(StatusSNS_ENERGY_Voltage);
  Serial.println(" V");

  Serial.print("Factor: ");
  Serial.println(StatusSNS_ENERGY_Factor);

  Serial.print("Total Current: ");
  Serial.print(StatusSNS_ENERGY_Current);
  Serial.println(" A");

  Serial.print("Apparent Power: ");
  Serial.print(StatusSNS_ENERGY_ApparentPower);
  Serial.println(" VA");

  Serial.print("Reactive Power: ");
  Serial.print(StatusSNS_ENERGY_ReactivePower);
  Serial.println("  VAr");

  Serial.print("Total Energy: ");
  Serial.print(StatusSNS_ENERGY_Total);
  Serial.println(" kWh");

  Serial.print("Total Energy Today: ");
  Serial.print(StatusSNS_ENERGY_Today);
  Serial.println(" kWh");
  
}

void setup() {
  
  Serial.begin(_BAUDRATE);
  WiFi.begin(_WIFI_SSID, _WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to the WiFi network");
  mqtt.begin(_MQTT_SERVER, _MQTT_PORT, wifi);
  Serial.print("Connecting MQTT Server ");
  while (!mqtt.connect("esp8266Client", _MQTT_USER, _MQTT_PASSWORD)) {
      Serial.print(".");
      delay(500);
  }
  
  Serial.println("\n");
  Serial.println("Connected");
  mqtt.onMessage(messageReceived);
  mqtt.subscribe(_MQTT_SUB_TOPIC_RESULT);
}

void loop() {
  mqtt.loop();
  if(WiFi.status() == WL_CONNECTED){
    if(millis() - lastMillis > intervalPublish){
      lastMillis = millis();
      mqtt.publish(_MQTT_PUB_TOPIC_REQUEST, _MQTT_TOPIC_PAYLOAD_REQUEST);
    }
  }
}