#include <ESP8266WiFi.h>

const char *ssid = "AsusHome";
const char *password = "aazzeerrttyy";

// https://www.dfrobot.com/blog-994.html
WiFiServer wifiServer(2300);

void reconnectIfNeed() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(WiFi.status());
    }
    Serial.println();
    Serial.print("IP : ");
    Serial.print(WiFi.localIP());
    Serial.print("\tEth : ");
    Serial.println(WiFi.macAddress());
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.disconnect();
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  reconnectIfNeed();
  wifiServer.begin();
}

void loop() {
  reconnectIfNeed();
  // put your main code here, to run repeatedly:
}