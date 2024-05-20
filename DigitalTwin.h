#ifndef _DIGITAL_TWIN_H
#define _DIGITAL_TWIN_H
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#define TWIN_DEBUG
#define SERIAL Serial

#define DEFAULT_MQTT_BROKER "twinned.digital"
#define DEFAULT_MQTT_PORT 1883
#define FORMAT_SPIFFS_IF_FAILED true
#define CONFIG_FILE "/config.json"

/* Setup debug printing macros. */
#ifdef TWIN_DEBUG
#define DEBUG_PRINT(...) \
  { SERIAL.print(__VA_ARGS__); }
#define DEBUG_PRINTLN(...) \
  { SERIAL.println(__VA_ARGS__); }
#else
#define DEBUG_PRINT(...) \
  {} /**< Debug Print Placeholder if Debug is disabled */
#define DEBUG_PRINTLN(...) \
  {} /**< Debug Print Line Placeholder if Debug is disabled */
#endif

class DigitalTwin {

public:
  DigitalTwin();

  DigitalTwin& setCallback(MQTT_CALLBACK_SIGNATURE);
  void setup();
  boolean loop();
  boolean connected();
  int state();
  boolean publish(JsonDocument* json);


private:

  boolean checkAndConnectWiFi();
  boolean checkAndConnect();
  void listen();

  char mqttHost[120];
  int mqttPort;
  char apiKey[40];
  char topic[50];
  char ssid[80];
  char psk[40];
  bool hasSetup;
};

#endif  //_DIGITAL_TWIN_H