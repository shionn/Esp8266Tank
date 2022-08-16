#include <ESP8266WiFi.h>

#define __AP_MODE__

struct t_move {
    int16_t speed;
    int16_t rot;
};

struct t_radar {
    int16_t angle;
    int16_t dist;
};


WiFiServer wifiServer(2300);
t_move move;
t_radar radar;

#ifndef __AP_MODE__
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
#endif

void initNetwork() {
#ifdef __AP_MODE__
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 10, 1), 0, IPAddress(255, 255, 255, 0));
    WiFi.softAP("EspTank");
    Serial.println(WiFi.softAPIP());
#else
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("EspTank");
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    WiFi.begin(F("AsusHome"), F("aazzeerrttyy"));
    reconnectIfNeed();
#endif

    wifiServer.begin();
}

void setup() {
    Serial.begin(115200);
    initNetwork();
}

void loop() {
#ifndef __AP_MODE__
    reconnectIfNeed();
#endif
    WiFiClient client = wifiServer.available();
    if (client && client.connected()) {
        client.setTimeout(10);
        if (client.readBytes((uint8_t*)(&move), sizeof(move)) == sizeof(move)) {
            Serial.print(move.speed);
            Serial.print(' ');
            Serial.println(move.rot);
        };
        radar.angle = 321;
        radar.dist = 654;
        client.write((char*)(&radar));
    }
}