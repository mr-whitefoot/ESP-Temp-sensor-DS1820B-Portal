void println(const String& text){
  Serial.println(text);
  glog.println(text);
}
void println(int32_t text){
  Serial.println(text);
  glog.println(text);
}

void print(const String& text){
  Serial.print(text);
  glog.print(text);
}

void print(float text){
  Serial.print(text);
  glog.print(text);
}

void print(int32_t text){
  Serial.print(text);
  glog.print(text);
}

void wifiApStaTimerHandler(){
  WiFi.mode(WIFI_STA);
}

void mqttStart(){
  println("Starting MQTT"); 
  mqttClient.setMqttServer(data.mqtt.mqttServerIp, data.mqtt.mqttUsername, data.mqtt.mqttPassword, data.mqtt.mqttServerPort );
  mqttClient.setMqttClientName(data.sensor.device_name);
    //Setup max lingth of message MQTT
  mqttClient.setMaxPacketSize(1000);
}

void sensorRead(){
  sensor.requestTemperatures();
  while (!sensor.isConversionComplete()); // wait until sensor is ready
  float temp_now = sensor.getTempC();

  println(F("-----------------------------------------------------------------------"));
  println("Temperature now: "+String(temp_now));
  print("Sensor_resolution: ");
  println(sensor.getResolution());

  int array_size = sizeof(temperature_array)/sizeof(temperature_array[0]);

  print("Array[");
  for(int i = 0; i < array_size ; i++){
    if(i != 0){ print(", ");}
    print(String(temperature_array[i]));
  }
  println("]"); 

  if (temperature_array[array_size - 1] <= temp_now){
    for(int i = 1; i < array_size; i++){
      temperature_array[i-1] = temperature_array[i];
    }
    temperature_array[array_size - 1] = temp_now;
  }else{
    for(int i = 0; i < array_size; i++){
      if ( temperature_array[i] > temp_now ){
        for(int j = array_size - 1 ; j > i; j--){
          temperature_array[j] = temperature_array[j-1];
        }
        temperature_array[i] = temp_now;
        break;
      }
    }
  }

  print("Array after insert[");
  for(int i = 0; i < array_size ; i++){
    if(i != 0){ print(", ");}
    print(String(temperature_array[i]));
  }
  println("]"); 

  if (temperature_array[4]!= NULL){
    temperature = temperature_array[4];
    print(F("Temperature mean: "));
  } else {
    temperature = temp_now;
    print(F("Temperature now:"));
  }
  print(temperature);
  println(F("°C"));
  println(F("-----------------------------------------------------------------------"));
}

void sensorSetTimer(){
  sensorTimer.stop();
  sensorTimer.setTime(data.sensor.sensorRefreshTime*1000);
  sensorTimer.attach(sensorRead);
  sensorTimer.start();
}

void sensorInit(){
  println("Initialize DS18B20 Sensor");
  pinMode(ONE_WIRE_BUS, INPUT);
  print("DS18B20 Library version: ");
  println(DS18B20_LIB_VERSION);
  sensor.begin();
  sensor.setResolution(12);
  println("Sensor update timer "+data.sensor.sensorRefreshTime*1000);
  sensorSetTimer();
  println(F("-------------------------------"));
}

void convertStringToMAC( const char* stringMAC, uint8_t bytesMAC[6] ){
  int values[6];
  int i;

  if (6 == sscanf(stringMAC, "%x:%x:%x:%x:%x:%x%*c",
                  &values[0], &values[1], &values[2],
                  &values[3], &values[4], &values[5]))
  {
    /* convert to uint8_t */
    for (i = 0; i < 6; ++i)
      bytesMAC[i] = (uint8_t)values[i];
  }
  else
  {
    /* invalid mac */
  };
}

void startup(){
  Serial.begin(9600);
  //Log
  glog.start(1000);

  println("-------------------------------");
  println("Booting...");
  println("Initialize EEPROM");
  EEPROM.begin(sizeof(data) + 1); // +1 key
  memory.begin(0, 'k');

  //Sensor
  sensorInit();

  // Connecting WiFi
  println("Initialize WiFi");
  if (data.params.factoryReset == true || data.params.wifiAP == true ) {
    wifiAp();
  } else {
    wifiConnect();
  }

  // Enable OTA update
  println("Starting OTA updates");
  ArduinoOTA.begin();

  if (data.params.factoryReset==false){    
    //MQTT
    mqttStart();
  
    // MQTT timers
    println("Starting MQTT timers");
    MessageTimer.setTime(data.mqtt.status_period*1000);
    MessageTimer.start();
    ServiceMessageTimer.setTime(data.mqtt.avaible_period*1000);
    ServiceMessageTimer.start();
  }
  
  // WiFiAP timer
  println("Starting WiFiAP timer");
  wifiApStaTimer.setTime(WIFIAPTIMER);
  wifiApStaTimer.attach(wifiApStaTimerHandler);

  //ESP NOW
  println("Starting ESP NOW");
  if (esp_now_init() != 0 ) {
    println("Error initializing ESP-NOW");
    restart();
  };


  println("Boot complete");
  println("-------------------------------");
}

void portalStart(){
  println("Starting portal");
  portal.attachBuild(portalBuild);
  portal.disableAuth();
  portal.attach(portalAction);
  portal.OTA.attachUpdateBuild(OTAbuild);
  portal.start(data.sensor.device_name);
  portal.enableOTA();
}

void wifiRestart(){
  data.params.wifiAP = false;
  memory.updateNow();
  restart();
}

void wifiAp(){
  println("Create AP");

  String ssid = data.sensor.device_name;
  ssid += "_AP";
  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid);
  IPAddress ip = WiFi.softAPIP();
  print("AP IP address: ");
  println(ip.toString());

  data.params.wifiConnectTry = 0;
  memory.updateNow();

  WiFiApTimer.setTime(WIFIAPTIMER);
  WiFiApTimer.setTimerMode();
  WiFiApTimer.attach(wifiRestart);
  WiFiApTimer.start();

  onSoftAPModeStationConnected = WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected& event)
  {
    portalStart();
  });

  onSoftAPModeStationDisconnected = WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected& event){
    data.params.wifiAP = false;
    memory.updateNow();
    WiFiApTimer.stop();
  });
}

void wifiConnect(){
  WiFi.mode(WIFI_STA);
  WiFi.hostname(data.sensor.device_name);

  onStationModeConnected = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected& event)
  {
    portalStart();
    wifiApStaTimer.start();
  });

  println("Wifi connecting...");
  WiFi.begin(data.wifi.ssid, data.wifi.password);
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    notConnectedCounter++;
    if (notConnectedCounter > 150)
    { // Reset board if not connected after 5s
      println("Resetting due to Wifi not connecting...");

      data.params.wifiConnectTry += 1;
      if(data.params.wifiConnectTry >= 12){
        data.params.wifiAP = true;
        memory.updateNow();
        restart();
      }
    }
  }
  
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  print("Wifi connected, IP address: ");
  println(WiFi.localIP().toString());
}

void factoryReset(){
  println("Factory reset");
  memory.reset();
  data.params.factoryReset = true;
  memory.updateNow();
  restart();
}

void restart(){
  println("Rebooting...");
  println("-------------------------------");
  SendAvailableMessage("offline");
  mqttClient.loop();
  portal.tick();
  ESP.restart();
};

float getTemperature(){
  return temperature;
}
