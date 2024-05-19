#include <DigitalTwin.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#define BAUD_RATE 115200

#define SSID_NAME "YOUR SSID"
#define SSID_PSK "YOUR SSID PASSWORD"
#define API_KEY "YOUR DEVICE API KEY"
#define MQTT_BROKER "twinned.digital"
#define MQTT_PORT 1883
#define SLEEP_INTERVAL 60000

void checkAndConnectWiFi();
void readAndPublish();

DigitalTwin twin;


void setup() {

  Serial.begin(BAUD_RATE);

  twin.setMqttBroker(MQTT_BROKER, MQTT_PORT);
  twin.setApiKey(API_KEY);

}

void loop() {

  checkAndConnectWiFi();

  twin.loop();

  readAndPublish();

  delay(SLEEP_INTERVAL);
}

/**
* Connect with your sensor(s) and send the data to the platform
*/
void readAndPublish(){

  StaticJsonDocument<200> doc;

  doc["humidity"] = 38;
  doc["temperature"] = 33.45;

  twin.publish(&doc);

}



void checkAndConnectWiFi() {

  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_NAME, SSID_PSK);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("");
  Serial.println(WiFi.localIP());
}
