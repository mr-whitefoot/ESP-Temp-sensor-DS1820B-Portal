#include <EspMQTTClient.h>
#include <ArduinoJson.h>
#include <TimerMs.h>
#include <GyverPortal.h>
#include <EEManager.h>
#include <DS18B20.h>
#include <ESP8266WiFi.h>
#include <espnow.h>



String sw_version = "0.3.7";
String release_date = "04.02.2024";
#define LIGHT_THEME 0
#define DARK_THEME 1
#define ONE_WIRE_BUS 2 //Для ESP01
//#define ONE_WIRE_BUS D2 //Для wemos mini

struct Mqtt{
  //MQTT
  char mqttServerIp[32];
  short mqttServerPort = 1883;
  char mqttUsername[32];
  char mqttPassword[32];
  //Period of sending message in seconds
  int status_period = 10;
  int avaible_period = 60;
  //MQTT Topic
  char discoveryTopic[100] = "homeassistant/sensor/temp_sensor/config";
  char commandTopic[100]   = "homeassistant/sensor/temp_sensor/set";
  char avaibleTopic[100]   = "homeassistant/sensor/temp_sensor/avaible";
  char stateTopic[100]     = "homeassistant/sensor/temp_sensor/state";
};

struct Wifi{
    // WiFi
  char ssid[32];
  char password[32];
};

struct Parameters{
  bool factoryReset = true;
  byte wifiConnectTry = 0;
  bool wifiAP;
  int  theme = DARK_THEME;
};

struct SensorData{
  char tempLabel[32] = "Temperature";
  char device_name[32] = "temp_sensor";
  int sensorRefreshTime = 10;
};

struct ESPNowData{
  bool enable = false;
  char broadcastAddress[13] = "FFFFFFFFFFFF";
};

struct Data {
  SensorData sensor;
  Parameters params;
  Wifi wifi;
  Mqtt mqtt;
  ESPNowData EspNow;
};

#define WIFIAPTIMER 300000
Data data;
GyverPortal portal;
EEManager memory(data);
GPlog glog("log");
WiFiEventHandler onSoftAPModeStationConnected, onSoftAPModeStationDisconnected, onStationModeConnected;
float temperature;
float temperature_array[9];
OneWire oneWire(ONE_WIRE_BUS);
DS18B20 sensor(&oneWire);


struct Form{
  const char* root = "/";
  const char* log = "/log";
  const char* config = "/config";
  const char* preferences = "/config/preferences";
  const char* WiFiConfig ="/config/wifi_config";
  const char* mqttConfig = "/config/mqtt_config";
  const char* ESPNowPreferences = "/config/ESPNowPreferences";
  const char* factoryReset = "/config/factory_reset";
  const char* firmwareUpgrade = "/ota_update";
};

Form form;
TimerMs MessageTimer, ServiceMessageTimer, WiFiApTimer, wifiApStaTimer, sensorTimer;
EspMQTTClient mqttClient;
bool resetAllow;

void publish();
void SendDiscoveryMessage();
void SendAvailableMessage(const String &mode );
void wifiAp();
void wifiConnect();
void mqttPublish();
void factoryReset();
void mqttStart();
void restart();
void sensorSetTimer();


#include <webface.h>
#include <function.h>
#include <mqtt.h>

void setup() {
  startup();
}

void loop(){
  ArduinoOTA.handle();
  mqttClient.loop();
  mqttPublish();
  portal.tick();
  WiFiApTimer.tick(); 
  wifiApStaTimer.tick();
  sensorTimer.tick();
}