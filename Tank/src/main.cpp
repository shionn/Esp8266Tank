#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

WiFiServer wifiServer(2300);

void reconnectIfNeed() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("Connecting"));
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(WiFi.status());
    }
    Serial.println();
    Serial.print(F("IP : "));
    Serial.print(WiFi.localIP());
    Serial.print(F("\tEth : "));
    Serial.println(WiFi.macAddress());
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.disconnect();
  WiFi.setHostname("EspTank");
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(F("AsusHome"), F("aazzeerrttyy"));

  reconnectIfNeed();
  wifiServer.begin();
}

DynamicJsonDocument doc(1024);

void loop() {
  reconnectIfNeed();
  WiFiClient client = wifiServer.available();
  if (client) {
    while (!client.connected()) {
      delay(10);
      Serial.println("wait connection");
    }
    while (client.connected()) {
      deserializeJson(doc, client);
      Serial.println(doc["s"].as<int16_t>());
      Serial.println(doc["r"].as<int16_t>());
    }
    client.stop();
    Serial.println(F("Client disconnected"));
  }
}