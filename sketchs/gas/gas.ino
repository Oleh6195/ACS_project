#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
//#include <FS.h>


#define MQ7  A0
WiFiUDP Udp;
int tm;
int cur_time;
char charBuf[50];
String temperatureTemp;
char mqtt_server[50] = "192.168.0.115";

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

        
         } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    int cnt = 0;
    Serial.begin(115200);

    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);


    WiFi.mode(WIFI_STA);
    while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(cnt++ >= 10){
       WiFi.beginSmartConfig();
       while(1){
           delay(1000);
           if(WiFi.smartConfigDone()){
             Serial.println("SmartConfig Success");
             break;
           }
       }
      }
    }
    if (WiFi.status() == WL_CONNECTED){
      digitalWrite(4,HIGH);
    }
    WiFi.printDiag(Serial);
  
    // Start the server
    Udp.begin(49999);
    Serial.println("USP Server started");
  
    // Print the IP address
    Serial.println(WiFi.localIP());
    Serial.println("connected...yeey :)");

   

    Serial.println("local ip");
    Serial.println(WiFi.localIP());

    Serial.print(mqtt_server);
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
        delay(100);
    }
    if (!client.loop())
        client.connect("ESP8266Client");
    temperatureTemp = analogRead(A0);
    temperatureTemp.toCharArray(charBuf, 50);
    digitalWrite(5,HIGH);
    client.publish("/esp8266/tag_val", "CO2");
    client.publish("/esp8266/gas_val", charBuf);
    client.disconnect();
    digitalWrite(5,LOW);
    Serial.print(temperatureTemp);
    delay(1000);

}
