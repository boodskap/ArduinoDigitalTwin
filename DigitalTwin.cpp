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
//#include <FS.h>
//#include <SPIFFS.h>

WiFiClient _espClient;
PubSubClient _mqttClient(_espClient);

DigitalTwin::DigitalTwin()
  : mqttHost(DEFAULT_MQTT_BROKER), mqttPort(DEFAULT_MQTT_PORT), apiKey(""), topic(""), ssid(""), psk(""), hasSetup(false){

}

void DigitalTwin::setup() {

  if (hasSetup) return;

  // if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
  //   DEBUG_PRINTLN("SPIFFS Mount Failed");
  // }

  // Open file for reading
  // File file = SPIFFS.open(CONFIG_FILE);

  StaticJsonDocument<1024> doc;
  //File file;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, "{}");
  if (error) {
    DEBUG_PRINTLN(F("Failed to read /config.json file."));
  } else {

    String wifiMacString = WiFi.macAddress();

    strlcpy(this->mqttHost, doc["mqtt_host"] | DEFAULT_MQTT_BROKER, sizeof(this->mqttHost));
    strlcpy(this->ssid, doc["ssid"] | "boodskap", sizeof(this->ssid));
    strlcpy(this->psk, doc["psk"] | "boodskap", sizeof(this->psk));
    strlcpy(this->apiKey, doc["api_key"] | wifiMacString.c_str(), sizeof(this->apiKey));
    this->mqttPort = doc["mqtt_port"] | DEFAULT_MQTT_PORT;
  }

  //file.close();

  sprintf(this->topic, "/%s/pub/0", this->apiKey);

  _mqttClient.setServer(this->mqttHost, this->mqttPort);

  hasSetup = true;
}

boolean DigitalTwin::checkAndConnectWiFi() {

  if (WiFi.status() == WL_CONNECTED) return true;

  DEBUG_PRINT("Connecting to WiFi ");
  DEBUG_PRINT(this->ssid);
  DEBUG_PRINT(". ");

  WiFi.mode(WIFI_STA);
  WiFi.begin(this->ssid, this->psk);

  for (int i = 0; i < 30; i++) {
    if (WiFi.status() != WL_CONNECTED) {
      DEBUG_PRINT('.');
      delay(1000);
    }
  }

  DEBUG_PRINTLN("");

  if (WiFi.status() == WL_CONNECTED) {
    DEBUG_PRINT("WiFi Connected, IP: ");
    DEBUG_PRINTLN(WiFi.localIP());
  } else {
    listen();
  }

  return (WiFi.status() == WL_CONNECTED);
}

boolean DigitalTwin::checkAndConnect() {

  if (!checkAndConnectWiFi()) {
    return false;
  }

  if (!_mqttClient.connected()) {
    if (_mqttClient.connect(this->apiKey)) {
      DEBUG_PRINT("connected with ");
      DEBUG_PRINTLN(this->mqttHost);
    } else {
      DEBUG_PRINT("connection failed with state ");
      DEBUG_PRINTLN(_mqttClient.state());
    }
  }

  return connected();
}

DigitalTwin& DigitalTwin::setCallback(MQTT_CALLBACK_SIGNATURE) {
  _mqttClient.setCallback(callback);
  return *this;
}

void DigitalTwin::listen() {
}

boolean DigitalTwin::loop() {

  if (!hasSetup) {
    setup();
  }

  if (!_mqttClient.connected()) {
    checkAndConnect();
  }

  return _mqttClient.loop();
}

boolean DigitalTwin::connected() {
  return _mqttClient.connected();
}

int DigitalTwin::state() {
  return _mqttClient.state();
}

boolean DigitalTwin::publish(JsonDocument* json) {

  if (!connected()) return false;

  String doc;
  serializeJson(*json, doc);

  boolean success = _mqttClient.publish(this->topic, doc.c_str());

  DEBUG_PRINT("Published: ");
  DEBUG_PRINT(doc.c_str());
  DEBUG_PRINT(" ");
  DEBUG_PRINTLN(success ? "yes" : "no");

  return success;
}
