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

  mqttClient.subscribe(data.mqtt.commandTopic, [] (const String &payload)  {
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
  mqttClient.publish(data.mqtt.stateTopic, buffer, false);
}

void SendDiscoveryMessage( ){
  println("MQTT publish discovery message");
  DynamicJsonDocument doc(1024);
  char buffer[1024];

  String device_name = data.sensor.device_name;

  doc["name"]         = data.sensor.device_name;
  doc["uniq_id"]      = "ESP_"+device_name;
  doc["object_id"]    = "ESP_"+device_name;
  doc["ip"]           = WiFi.localIP().toString();
  doc["mac"]          = WiFi.macAddress();
  doc["avty_t"]       = data.mqtt.avaibleTopic;
  doc["pl_avail"]     = "online";
  doc["pl_not_avail"] = "offline";
  doc["stat_t"]       = data.mqtt.stateTopic;;
  doc["dev_cla"]      = "temperature";
  doc["val_tpl"]      = "{{ value_json.temperature|default(0) }}";
  doc["unit_of_meas"] = "°C";

  JsonObject device = doc.createNestedObject("device");
  device["name"]         = data.sensor.tempLabel;
  device["model"]        = device_name;
  device["configuration_url"] = "http://"+WiFi.localIP().toString();
  device["manufacturer"] = "WhiteFootCompany";
  device["sw_version"]   = sw_version;
  JsonArray identifiers = device.createNestedArray("identifiers");
  identifiers.add(WiFi.macAddress());

  serializeJson(doc, buffer);
  mqttClient.publish(data.mqtt.discoveryTopic, buffer, true);
}

void SendAvailableMessage(const String &mode = "online"){
  println("MQTT publish avaible message");
  mqttClient.publish(data.mqtt.avaibleTopic, mode, true);
}

void mqttPublish() {
  if (mqttClient.isConnected() && MessageTimer.tick()) {
    publish();
  }
}
