#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
  
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 

//#include <FS.h>


//#define MQ7  A0
WiFiUDP Udp;
StaticJsonDocument<256> doc;
int tm;
int cur_time;
char charBuf[50];
String temperatureTemp;
char mqtt_server[50] = "192.168.0.115";

WiFiClient espClient;
//HttpClient clienthttp = HttpClient(espClient, "192.168.0.195", 5000);
int status = WL_IDLE_STATUS;
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
    dht.begin();
    Serial.begin(115200);

//    pinMode(4, OUTPUT);
//    pinMode(5, OUTPUT);
//    pinMode(16, OUTPUT);



    WiFi.mode(WIFI_STA);
    bool is = true;
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
            if(WiFi.smartConfigDone()){
            Serial.println("SmartConfig Success");
             break;
             }
        }
    }
      digitalWrite(4,HIGH);
    
    WiFi.printDiag(Serial);
  
    // Start the server
    Udp.begin(49999);
    Serial.println("USP Server started");
  
    // Print the IP address
    Serial.println(WiFi.localIP());
    Serial.println("connected...yeey :)");
    String payload = "";
    while(payload == ""){
      digitalWrite(16,HIGH);
      HTTPClient http;
      String url = "http://olehtyzhai.pythonanywhere.com/" + String(WiFi.SSID());
      Serial.println(url);
      http.begin(url);
      int httpCode = http.GET();                                                                  //Send the request
      if (httpCode > 0) { //Check the returning code
          payload = http.getString();   //Get the request response payload
          Serial.println(payload);  
      }//Print the response payload
      Serial.println(httpCode);  
      Serial.println(".");  
   
      http.end();
      delay(3000);
    }
    digitalWrite(16,LOW);

    Serial.println(payload);
    deserializeJson(doc, payload);
    JsonArray array = doc.as<JsonArray>();
//    Serial.println(array);
    String a = doc[0];
    Serial.println(a);
    
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
//    temperatureTemp = analogRead(A0);
    temperatureTemp = String(dht.readTemperature());
    
    float t = dht.readTemperature();
    Serial.print(t);
    
    temperatureTemp.toCharArray(charBuf, 50);
//    digitalWrite(5,HIGH);
    client.publish("/esp8266/tag_val", "CO2");
    client.publish("/esp8266/gas_val", charBuf);
    client.disconnect();
//    digitalWrite(5,LOW);
    Serial.print(temperatureTemp);
    delay(1000);

}
