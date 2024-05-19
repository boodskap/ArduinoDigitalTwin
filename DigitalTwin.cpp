#include "DigitalTwin.h"
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiSTA.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

DigitalTwin::DigitalTwin()
  : mqttBroker(DEFAULT_MQTT_BROKER), mqttPort(DEFAULT_MQTT_PORT), apiKey(""), topic("") {
  mqttClient.setServer(this->mqttBroker, this->mqttPort);
}

void DigitalTwin::checkAndConnect() {

  if (!mqttClient.connected()) {
    if (mqttClient.connect(this->apiKey)) {
      DEBUG_PRINT("connected with ");
      DEBUG_PRINTLN(this->mqttBroker);
    } else {
      DEBUG_PRINT("connection failed with state ");
      DEBUG_PRINTLN(mqttClient.state());
    }
  }
}

DigitalTwin& DigitalTwin::setMqttBroker(const char* mqttBroker) {
  setMqttBroker(mqttBroker, DEFAULT_MQTT_PORT);
  return *this;
}

DigitalTwin& DigitalTwin::setMqttBroker(const char* mqttBroker, int mqttPort) {
  strcpy(this->mqttBroker, mqttBroker);
  this->mqttPort = mqttPort;
  mqttClient.setServer(this->mqttBroker, this->mqttPort);
  return *this;
}

DigitalTwin& DigitalTwin::setApiKey(const char* apiKey) {
  strcpy(this->apiKey, apiKey);
  sprintf(this->topic, "/%s/pub/0", apiKey);
  return *this;
}

DigitalTwin& DigitalTwin::setCallback(MQTT_CALLBACK_SIGNATURE) {
  mqttClient.setCallback(callback);
  return *this;
}

boolean DigitalTwin::loop() {
  if(!mqttClient.connected()) {
    checkAndConnect();
  }
  return mqttClient.loop();
}

boolean DigitalTwin::connected() {
  return mqttClient.connected();
}

int DigitalTwin::state() {
  return mqttClient.state();
}

boolean DigitalTwin::publish(JsonDocument* json) {

  if(!mqttClient.connected()) return false;

  String doc;
  serializeJson(*json, doc);

  boolean success = mqttClient.publish(this->topic, doc.c_str());

  DEBUG_PRINT("Published: ");  
  DEBUG_PRINT(doc.c_str());
  DEBUG_PRINT(" ");
  DEBUG_PRINTLN(success ? "yes" : "no");  

  return success;
}
