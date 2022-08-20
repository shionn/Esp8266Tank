#include <ESP8266WiFi.h>
#include <Servo.h>

#define __AP_MODE__

#define MOT_R_PIN_1 4
#define MOT_R_PIN_2 5
#define MOT_L_PIN_1 0
#define MOT_L_PIN_2 2

#define SERVO_PIN 12
#define RADAR_ECHO_PIN 16
#define RADAR_TRIGGER_PIN 14
#define RADAR_MEASURE_TIMEOUT 25000UL
#define RADAR_MEASURE_COUNT 5
#define RADAR_MAX_ANGLE 140
#define RADAR_MIN_ANGLE 40

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
Servo servo;
int16_t radarSpeed = 2;

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

    radar.angle = 90;
    radar.dist = 0;
    move.rot = 0;
    move.speed = 0;

    pinMode(MOT_L_PIN_1, OUTPUT);
    pinMode(MOT_L_PIN_2, OUTPUT);
    pinMode(MOT_R_PIN_1, OUTPUT);
    pinMode(MOT_R_PIN_2, OUTPUT);
    pinMode(RADAR_TRIGGER_PIN, OUTPUT);
    pinMode(RADAR_ECHO_PIN, INPUT);

    digitalWrite(RADAR_TRIGGER_PIN, LOW);
    analogWrite(MOT_L_PIN_1, 0);
    analogWrite(MOT_L_PIN_2, 0);
    analogWrite(MOT_R_PIN_1, 0);
    analogWrite(MOT_R_PIN_2, 0);

    servo.attach(SERVO_PIN);
    servo.write(0);
}

void updateNetwork() {
    WiFiClient client = wifiServer.available();
    if (client && client.connected()) {
        client.setTimeout(10);
        if (client.readBytes((uint8_t*)(&move), sizeof(move)) == sizeof(move)) {
            Serial.print(move.speed);
            Serial.print(' ');
            Serial.println(move.rot);
        }
        client.write((char*)(&radar));
    }
}

void updateRadar() {
    servo.write(radar.angle);
    long measure = 0;
    for (uint8_t i = 0; i < RADAR_MEASURE_COUNT; i++) {
        digitalWrite(RADAR_TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(RADAR_TRIGGER_PIN, LOW);
        measure += pulseIn(RADAR_ECHO_PIN, HIGH, RADAR_MEASURE_TIMEOUT);
    }
    radar.dist = measure / RADAR_MEASURE_COUNT;
    radar.angle += radarSpeed;
    if (radar.angle >= RADAR_MAX_ANGLE || radar.angle <= RADAR_MIN_ANGLE) {
        radar.angle = 90;
        radarSpeed = -radarSpeed;
    }
}

void updateMotor() {
    int left = max(min(255, move.speed + move.rot), -255);
    int right = max(min(255, move.speed - move.rot), -255);
    if (left > 0) {
        analogWrite(MOT_L_PIN_1, left);
        analogWrite(MOT_L_PIN_2, 0);
    } else if (left < 0) {
        analogWrite(MOT_L_PIN_1, 0);
        analogWrite(MOT_L_PIN_2, abs(left));
    } else {
        analogWrite(MOT_L_PIN_1, 0);
        analogWrite(MOT_L_PIN_2, 0);
    }

    if (right > 0) {
        analogWrite(MOT_R_PIN_1, right);
        analogWrite(MOT_R_PIN_2, 0);
    } else if (right < 0) {
        analogWrite(MOT_R_PIN_1, 0);
        analogWrite(MOT_R_PIN_2, abs(right));
    } else {
        analogWrite(MOT_R_PIN_1, 0);
        analogWrite(MOT_R_PIN_2, 0);
    }
}

void loop() {
#ifndef __AP_MODE__
    reconnectIfNeed();
#endif
    updateRadar();
    updateNetwork();
    updateMotor();
}