bool ToBool( String value){
  if ( (value == "true" )||
       (value == "True" )||
       (value == "TRUE" )){
        return true;
       };

  if ( value == "false" ||
       value == "False" ||
       value == "FALSE" ){
        return false;
       };

  return false;
}

void onConnectionEstablished() {
  println("MQTT server is connected");
  SendDiscoveryMessage();
  SendAvailableMessage("online");

  mqttClient.subscribe(data.commandTopic, [] (const String &payload)  {
    println("MQTT received command topic");
    //.....
  });
}

void publish() {
  println("MQTT publish status");

  DynamicJsonDocument doc(256);
  char buffer[256];
  doc["temperature"] = getTemperature();
  doc["WiFiRSSI"] = WiFi.RSSI();
  serializeJson(doc, buffer);
  mqttClient.publish(data.stateTopic, buffer, false);
}

void SendDiscoveryMessage( ){
  println("MQTT publish discovery message");
  DynamicJsonDocument doc(1024);
  char buffer[1024];

  String device_name = data.device_name;

  doc["name"]         = data.device_name;
  doc["uniq_id"]      = "ESP_"+device_name;
  doc["object_id"]    = "ESP_"+device_name;
  doc["ip"]           = WiFi.localIP().toString();
  doc["mac"]          = WiFi.macAddress();
  doc["avty_t"]       = data.avaibleTopic;
  doc["pl_avail"]     = "online";
  doc["pl_not_avail"] = "offline";
  doc["stat_t"]       = data.stateTopic;;
  doc["dev_cla"]      = "temperature";
  doc["val_tpl"]      = "{{ value_json.temperature|default(0) }}";
  doc["unit_of_meas"] = "°C";

  JsonObject device = doc.createNestedObject("device");
  device["name"]         = data.tempLabel;
  device["model"]        = device_name;
  device["configuration_url"] = "http://"+WiFi.localIP().toString();
  device["manufacturer"] = "WhiteFootCompany";
  device["sw_version"]   = version;
  JsonArray identifiers = device.createNestedArray("identifiers");
  identifiers.add(WiFi.macAddress());

  serializeJson(doc, buffer);
  mqttClient.publish(data.discoveryTopic, buffer, true);
}

void SendAvailableMessage(const String &mode = "online"){
  println("MQTT publish avaible message");
  mqttClient.publish(data.avaibleTopic, mode, false);
}

void mqttPublish() {
  if (mqttClient.isConnected() && MessageTimer.tick()) {
    publish();
  }

  if (mqttClient.isConnected() && ServiceMessageTimer.tick()) {
    SendAvailableMessage();
  }
}
