#include <ESP8266WiFi.h>

// https://www.dfrobot.com/blog-994.html
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

void loop() {
  reconnectIfNeed();

  WiFiClient client = wifiServer.available();
  if (client) {
    while (!client.connected()) {
      delay(10);
      Serial.println("wait connection");
    }
    while (client.connected()) {
      Serial.println(client.readString());
      while (client.available() > 0) {
        char c = client.read();
        Serial.print("data : ");
        Serial.println(c);
      }
      delay(10);
    }
    client.stop();
    Serial.println(F("Client disconnected"));
  }
}