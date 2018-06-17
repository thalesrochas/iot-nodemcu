#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#define DHTTYPE DHT11

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9 3
#define D10 1

DHT dht(D2, DHTTYPE);
BlynkTimer timerDHT, timerSolo, timerThingSpeak, timerNotify;

const char blynk_auth[] = "***";

WiFiClient thingSpeak;
const char* server = "api.thingspeak.com";
const String keyThingSpeak = "***";

const char ssid[] = "***";
const char pass[] = "***";

float temperatura;
float umidade;
float umidadeSolo;

void setup() {
    Serial.begin(38400);
    
    dht.begin();
    
    Blynk.begin(blynk_auth, ssid, pass);
    
    while (!Blynk.connect()) {}
    
    timerDHT.setInterval(2000L, readDHT);
    timerSolo.setInterval(1000L, readSolo);
    timerThingSpeak.setInterval(20000L, sendToThingSpeak);
    timerNotify.setInterval(30000L, notificate);
}

void loop() {
    Blynk.run();
    timerDHT.run();
    timerSolo.run();
    timerThingSpeak.run();
    timerNotify.run();
}

void readDHT() {
    temperatura = dht.readTemperature();
    umidade = dht.readHumidity();
    
    if (!digitalRead(D1)) {
        Blynk.virtualWrite(V0, "Desativado");
        Blynk.virtualWrite(V1, "Desativado");
    }
    else {
        if (!isnan(temperatura)) {
            Blynk.virtualWrite(V0, temperatura);
            Serial.print("Temperatura:\t");
            Serial.print(temperatura);
            Serial.println("ºC");
        }
        if (!isnan(umidade)) {
            Blynk.virtualWrite(V1, umidade);
            Serial.print("Umidade:\t");
            Serial.print(umidade);
            Serial.println("%");
        }
    }
}

void readSolo() {
    umidadeSolo = map(analogRead(A0), 216, 1023, 100, 0);
    Blynk.virtualWrite(V2, umidadeSolo);
    Serial.print("Umidade Solo:\t");
    Serial.print(umidadeSolo);
    Serial.println("%");
}

void sendToThingSpeak() {
    if (thingSpeak.connect(server, 80)) {
        Serial.println("Enviando informações para ThingSpeak...");
        String postStr = keyThingSpeak;
        if (digitalRead(D1)) {
            if (!isnan(temperatura)) {
                postStr += "&field1=";
                postStr += String(temperatura);
            }
            if (!isnan(umidade)) {
                postStr += "&field2=";
                postStr += String(umidade);
            }
        }
        else {
            Serial.println("O sensor DHT11 está desativado.\nEnvio para o ThingSpeak cancelado.");
        }
        postStr += "&field3=";
        postStr += String(umidadeSolo);
        postStr += "\r\n\r\n";

        thingSpeak.print("POST /update HTTP/1.1\n");
        thingSpeak.print("Host: api.thingspeak.com\n");
        thingSpeak.print("Connection: close\n");
        thingSpeak.print("X-THINGSPEAKAPIKEY: " + keyThingSpeak + "\n");
        thingSpeak.print("Content-Type: application/x-www-form-urlencoded\n");
        thingSpeak.print("Content-Length: ");
        thingSpeak.print(postStr.length());
        thingSpeak.print("\n\n");
        thingSpeak.print(postStr);
        Serial.println("Informações enviadas!");
    }
    else {
        Serial.println("Falha ao conectar com " + String(server));
    }
    thingSpeak.stop();
}

void notificate() {
    if (umidade >= 80) {
        Blynk.notify("Alta Umidade! " + String(umidade) + "%");
    }
    
    if (umidadeSolo <= 20) {
        Blynk.notify("Baixa umidade no solo " + String(umidadeSolo) + "%\nRegue sua planta!");
    }
}
