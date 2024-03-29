void portalBuild(){
  uint32_t timeleftAP = WiFiApTimer.timeLeft()/1000;
  
  GP.BUILD_BEGIN();
  if (data.params.theme == LIGHT_THEME ) GP.THEME(GP_LIGHT);
  else GP.THEME(GP_DARK);

  // Update components
  GP.UPDATE("temperature,signal,switch,mqttStatusLed,ipAddress,wifiAPTimer");


  // Configuration page
  if (portal.uri() == form.config) {
    GP.PAGE_TITLE("Configuration");
    GP.TITLE("Configuration");
    GP.HR();
    GP.BUTTON_LINK(form.preferences, "Preferences");
    GP.BUTTON_LINK(form.WiFiConfig, "Wi-Fi configuration");
    GP.BUTTON_LINK(form.mqttConfig, "MQTT configuration");
    GP.BUTTON_LINK(form.ESPNowPreferences, "ESP-NOW configuration");
    GP.BUTTON_LINK(form.factoryReset, "Factory reset");
    GP.BUTTON_LINK(form.firmwareUpgrade, "Firmware upgrade");
    GP.BUTTON("rebootButton", "Reboot");
    GP.HR();
    GP.BUTTON_LINK(form.root, "Back");

    //Log
  } else if (portal.uri() == form.log){
    GP.PAGE_TITLE("Log");
    GP.AREA_LOG(glog, 20);
    GP.BUTTON_LINK(form.root, "Back");

    //Preferences
  } else if (portal.uri() == form.preferences){
    GP.PAGE_TITLE("Preferences");
    GP.FORM_BEGIN(form.preferences);
      GP.TITLE("Preferences");
      GP.HR();
      GP.BLOCK_TAB_BEGIN("Device name");
        GP.LABEL("Temperature label"); GP.BREAK();
        GP.TEXT("tempLabel", "Temperature label", data.sensor.tempLabel);GP.BREAK();
        GP.LABEL("Device name"); GP.BREAK();
        GP.TEXT("device_name", "Device name", data.sensor.device_name);GP.BREAK();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("Settings");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Theme");   GP.SELECT("theme", "Light,Dark", data.params.theme);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Sensor refresh time"); GP.NUMBER("sensorRefreshTime", "Sensor refresh time", data.sensor.sensorRefreshTime); 
        GP.BOX_END(); 
      GP.BLOCK_END();
      
      GP.BLOCK_TAB_BEGIN("Information");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Firmware version: "); GP.LABEL(sw_version);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Release date: "); GP.LABEL(release_date);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("MAC: "); GP.LABEL(WiFi.macAddress());
        GP.BOX_END();
      GP.BLOCK_END();

      GP.HR();
      GP.FORM_SEND("Save", form.config);
    GP.FORM_END();
    GP.BUTTON_LINK(form.config, "Back");


    // WiFi configuration page 
  } else if (portal.uri() == form.WiFiConfig) {
      GP.PAGE_TITLE("Wi-Fi configuration");
      GP.FORM_BEGIN(form.WiFiConfig);
        GP.TITLE("Wi-Fi");
        GP.HR();

        GP.BLOCK_TAB_BEGIN("Information");
          if (WiFi.status() == WL_CONNECTED){
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("Wi-Fi status"); GP.LED_GREEN("WiFiLed", true);
            GP.BOX_END();
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("Signal"); GP.LABEL("","signal");
            GP.BOX_END();
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("IP address"); GP.LABEL(WiFi.localIP().toString(),"ipAddress");
            GP.BOX_END();}
          else {  
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("WiFi status"); GP.LED_GREEN("WiFiLed", false);
            GP.BOX_END();
          }
        GP.BLOCK_END();

        GP.BLOCK_TAB_BEGIN("Settings");
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("SSID"); GP.TEXT("ssid", "SSID", data.wifi.ssid, "200px");
          GP.BOX_END();
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Password"); GP.PASS("pass", "Password", data.wifi.password, "200px");
          GP.BOX_END();
        GP.BLOCK_END();
        
        GP.HR();
        GP.FORM_SEND("Save", form.config);
        GP.BUTTON_LINK(form.config, "Back");
      GP.FORM_END();

    // MQTT configuration page 
  } else if (portal.uri() == form.mqttConfig) {
    GP.PAGE_TITLE("MQTT configuration");
    GP.FORM_BEGIN(form.mqttConfig);
      GP.TITLE("MQTT");
      GP.HR();

      GP.BLOCK_TAB_BEGIN("Information");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Status"); GP.LED_GREEN("mqttStatusLed", mqttClient.isConnected());          
        GP.BOX_END();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("Server");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Address");
          GP.TEXT("mqttServerIp", "Address", data.mqtt.mqttServerIp);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Port");
          GP.NUMBER("mqttServerPort", "Port", data.mqtt.mqttServerPort, "100px");
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Username");
          GP.TEXT("mqttUsername", "Username", data.mqtt.mqttUsername);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Password");
          GP.PASS("mqttPassword", "Password", data.mqtt.mqttPassword);
        GP.BOX_END();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("MQTT Message periods");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Avaible");
          GP.NUMBER("avaible_period", "Avaible", data.mqtt.avaible_period, "100px");
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Message");
          GP.NUMBER("status_delay", "Message", data.mqtt.status_period, "100px");
        GP.BOX_END();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("MQTT topics");
        GP.LABEL("Discovery topic"); GP.BREAK();
        GP.TEXT("discoveryTopic", "Discovery topic", data.mqtt.discoveryTopic); GP.BREAK();
        GP.LABEL("Command topic"); GP.BREAK();
        GP.TEXT("commandTopic", "Command topic", data.mqtt.commandTopic); GP.BREAK();
        GP.LABEL("Avaible topic"); GP.BREAK();
        GP.TEXT("avaibleTopic", "Avaible topic", data.mqtt.avaibleTopic); GP.BREAK();
        GP.LABEL("State topic"); GP.BREAK();
        GP.TEXT("stateTopic", "State topic", data.mqtt.stateTopic); GP.BREAK();
      GP.BLOCK_END();

      GP.HR();
      GP.SUBMIT("Save and reboot");
      GP.BUTTON_LINK(form.config, "Back");;
    GP.FORM_END();   


    //ESP Now preferences page    
  } else if (portal.uri() == form.ESPNowPreferences){
  GP.PAGE_TITLE("ESP-NOW configuration");
  GP.FORM_BEGIN(form.ESPNowPreferences);
    GP.TITLE( "ESP-NOW configuration" );
    GP.HR();
    GP.BLOCK_TAB_BEGIN("MAC Receiver");
      GP.BOX_BEGIN(GP_EDGES);
        GP.LABEL("MAC");
        GP.TEXT("espNowBroadcastAddress", "MAC Receiver", data.EspNow.broadcastAddress, "", 12);
       GP.BOX_END();
    GP.BLOCK_END();
    GP.FORM_SEND("Save", form.config);
    GP.BUTTON_LINK(form.config, "Back");
  GP.FORM_END();

    //Factory reset page 
  } else if (portal.uri() == form.factoryReset) {
    GP.PAGE_TITLE("Factory reset");
    GP.FORM_BEGIN(form.factoryReset);
      GP.TITLE( "Factory reset" );
      GP.HR();
      GP.BOX_BEGIN(GP_EDGES);
        GP.LABEL("I'm really understand what I do");           
        GP.CHECK("resetAllow", resetAllow);  GP.BREAK(); 
      GP.BOX_END();
      
      GP.HR();      
      GP.SUBMIT("Factory reset");
      GP.BUTTON_LINK(form.config, "Back");;
    GP.FORM_END();
    
    // Root page, "/"
  } else {
    GP.PAGE_TITLE(data.sensor.tempLabel);
    GP.FORM_BEGIN(form.root);
       GP.BLOCK_TAB_BEGIN("Sensor");
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL(data.sensor.tempLabel); GP.LABEL("","temperature");
          GP.BOX_END();
      GP.BLOCK_END();
    
      GP.BLOCK_TAB_BEGIN("Wi-Fi");
        if (WiFi.status() == WL_CONNECTED){
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Status");GP.LED_GREEN("WiFiLed", true);
          GP.BOX_END();
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Signal"); GP.LABEL("","signal");
          GP.BOX_END();
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("IP address"); GP.LABEL(WiFi.localIP().toString(),"ipAddress");
          GP.BOX_END(); 
        }else{
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Status");GP.LED_GREEN("WiFiLed", false);
          GP.BOX_END(); 
        }
      GP.BLOCK_END();
      
      GP.BLOCK_TAB_BEGIN("MQTT");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Status"); GP.LED_GREEN("mqttStatusLed", mqttClient.isConnected());
        GP.BOX_END(); 
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("Information");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Firmware version");
          GP.LABEL(sw_version);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Release date");
          GP.LABEL(release_date);
        GP.BOX_END();
        if (WiFiApTimer.active()){
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Restart in");
            GP.LABEL(String(timeleftAP),"wifiAPTimer");
          GP.BOX_END();
        };
      GP.BLOCK_END();
      GP.HR();
      GP.BUTTON_LINK(form.config, "Configuration");
      GP.BUTTON_LINK(form.log, "Log");
    GP.FORM_END();
  }
  GP.BUILD_END();
}

void portalCheckForm(){
  if (portal.form()) {    
    //WiFi config
    if (portal.form(form.WiFiConfig)) {
      portal.copyStr("ssid", data.wifi.ssid);
      portal.copyStr("pass", data.wifi.password);
      data.params.factoryReset = false;
      data.params.wifiAP = false;
      memory.updateNow();
      wifiConnect();
    
    // Factory reset
    } else if(portal.form(form.factoryReset)){
      Serial.println("Factory reset"); 
      if(portal.getCheck("resetAllow"))
        factoryReset();
    
    // Preferences
    } else if(portal.form(form.preferences)){
      portal.copyStr("tempLabel", data.sensor.tempLabel);
      portal.copyStr("device_name", data.sensor.device_name);
      portal.copyInt("theme", data.params.theme);
      portal.copyInt("sensorRefreshTime", data.sensor.sensorRefreshTime);
      memory.updateNow();
      sensorSetTimer();
      
      //MQTT Config
    } else if(portal.form(form.mqttConfig)){
      portal.copyStr("mqttServerIp", data.mqtt.mqttServerIp);
      data.mqtt.mqttServerPort = portal.getInt("mqttServerPort");
      portal.copyStr("mqttUsername", data.mqtt.mqttUsername);
      portal.copyStr("mqttPassword", data.mqtt.mqttPassword);
      data.mqtt.avaible_period = portal.getInt("avaible_period");
      data.mqtt.status_period = portal.getInt("status_delay");
      portal.copyStr("discoveryTopic", data.mqtt.discoveryTopic);
      portal.copyStr("commandTopic", data.mqtt.commandTopic);
      portal.copyStr("avaibleTopic", data.mqtt.avaibleTopic);
      portal.copyStr("stateTopic", data.mqtt.stateTopic);
      memory.updateNow();
      restart();

      //ESP-NOW Config
    } else if(portal.form(form.ESPNowPreferences)){
      portal.copyStr("espNowBroadcastAddress", data.EspNow.broadcastAddress);
      memory.updateNow();
    }
    
  }
}

void portalUpdate(){
  if (portal.update()){
    long rssi = WiFi.RSSI();
    int strength = map(rssi, -80, -20, 0, 100);
    String wifiStrength = String(strength)+"%";
    portal.updateString("signal", wifiStrength);

    portal.updateFloat("temperature", temperature, 1 );

    portal.updateInt("mqttStatusLed",mqttClient.isConnected());
    String ipAdress = WiFi.localIP().toString();
    portal.updateString("ipAddress", ipAdress);    

    uint32_t timeleftAP = WiFiApTimer.timeLeft()/1000;
    portal.updateInt("wifiAPTimer", timeleftAP);

    portal.updateLog(glog);
  }
};

void portalClick(){
  if (portal.click()){
    Serial.println("Portal click");
    if (portal.click("rebootButton")){
      restart();
    }
  }
};

void portalAction(){
  portalCheckForm();
  portalUpdate();
  portalClick();
}

//Custom OTA page
void OTAbuild(bool UpdateEnd, const String& UpdateError) {
  GP.BUILD_BEGIN(400);
    #ifndef GP_OTA_LIGHT
      GP.THEME(GP_DARK);
    #else
      GP.THEME(GP_LIGHT);
    #endif

    GP.PAGE_TITLE(F("Firmware upgrade"));
    if (!UpdateEnd) {
      GP.BLOCK_TAB_BEGIN(F("Firmware upgrade"));
        GP.OTA_FIRMWARE(F("OTA firmware"), GP_GREEN, true);
      GP.BLOCK_END();
      GP.BUTTON_LINK(form.config, "Back");
    } else if (UpdateError.length()) {
      GP.BLOCK_TAB_BEGIN(F("Firmware upgrade"));
        GP.TITLE(String(F("Update error: ")) + UpdateError);
        GP.BUTTON_LINK(form.firmwareUpgrade, F("Refresh"));
      GP.BLOCK_END();
        
    } else {
      GP.BLOCK_TAB_BEGIN(F("Firmware upgrade"));
        GP.TITLE(F("Update Success!"));
        GP.TITLE(F("Rebooting..."));
      GP.BLOCK_END();
      GP.BUTTON_LINK(form.root, "Home");
    }
    
  GP.BUILD_END();
}