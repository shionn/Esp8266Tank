#include <ESP8266WiFi.h>

WiFiServer wifiServer(2300);

void reconnectIfNeed() {

    if (WiFi.status() != WL_CONNECTED) {
        Serial.print(F("Connecting"));
        while (!WiFi.isConnected()) {
            delay(1000);
            Serial.print(F("."));
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
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(F("AsusHome"), F("aazzeerrttyy"));

    reconnectIfNeed();
    wifiServer.begin();
}

WiFiClient client;

void loop() {
    reconnectIfNeed();
    WiFiClient client = wifiServer.available();
    if (client && client.connected()) {
        while (client.available()) {
            Serial.println((char)client.read());
        }
        // deserializeJson(doc, client);
        // Serial.println(doc["s"].as<int16_t>());
        // Serial.println(doc["r"].as<int16_t>());
    } else {
        Serial.print("n");
        delay(100);
    }
}