//#include <FirebaseCloudMessaging.h>
//#include <Firebase.h>
//#include <FirebaseHttpClient.h>
//#include <SerialTransceiver.h>
//#include <Thing.h>
#include <Firebase.h>
//#include <FirebaseError.h>
//#include <FirebaseObject.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <FS.h>

#define FIREBASE_HOST "https://sensorsnetwork-c38e4.firebaseio.com/"
#define FIREBASE_AUTH "sensorsnetwork-c38e4-firebase-adminsdk-cnn2m-f0b4040288.json"
#define MQ7  A0
int tm;
int cur_time;
char charBuf[50];
String temperatureTemp;
char mqtt_server[50] = "10.10.115.11";

WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);
bool shouldSaveConfig = false;

void saveConfigCallback() {
    Serial.println("Should save config");
    shouldSaveConfig = true;
}

void callback(String topic, byte *message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++) {
        Serial.print((char) message[i]);
        messageTemp += (char) message[i];
    }
    Serial.println();

    Serial.println();
}

void reconnect() {
    tm = millis();
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");

        if (client.connect("ESP8266Client")) {
            Serial.println("connected");

        }
        if (millis() - tm > 20000){
          ESP.eraseConfig();
          ESP.reset();
          wifiManager.resetSettings();
          delay(1000);
          break;
          
         } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
//     Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//     Serial.print(Firebase.getInt("users/686"));
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    if (SPIFFS.begin()) {
        Serial.println("mounted file system");
        if (SPIFFS.exists("/config.json")) {
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile) {
                Serial.println("opened config file");
                size_t size = configFile.size();
                std::unique_ptr<char[]> buf(new char[size]);
                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, buf.get());
                serializeJson(doc, Serial);
                auto error = deserializeJson(doc, buf.get());
                if (error) {
                    Serial.print(F("deserializeJson() failed with code "));
                    Serial.println(error.c_str());
                    return;
                }
                configFile.close();
            }
        }
    } else {
        Serial.println("failed to mount FS");
    }
    
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    

    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.addParameter(&custom_mqtt_server);
    if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        ESP.reset();
        delay(5000);
    }
    digitalWrite(4,HIGH);
    Serial.println("connected...yeey :)");
    strcpy(mqtt_server, custom_mqtt_server.getValue());

    if (shouldSaveConfig) {
        Serial.println("saving config");
        DynamicJsonDocument doc(1024);
        doc["mqtt_server"] = mqtt_server;


        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) {
            Serial.println("failed to open config file for writing");
        }
        serializeJson(doc, Serial);
        serializeJson(doc, configFile);

        configFile.close();

    }

    Serial.println("local ip");
    Serial.println(WiFi.localIP());

    Serial.print(mqtt_server);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    if (!client.loop())
        client.connect("ESP8266Client");
    temperatureTemp = analogRead(A0);
    temperatureTemp.toCharArray(charBuf, 50);
    client.publish("/esp8266/tag_val", "CO2");
    client.publish("/esp8266/gas_val", charBuf);
    Serial.print(temperatureTemp);
    delay(1000);

}
