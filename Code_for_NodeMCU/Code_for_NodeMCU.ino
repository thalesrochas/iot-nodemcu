// Configurações do Blynk
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// Configuração do DHT11
#define DHTTYPE DHT11

// Define os pinos do NodeMCU para facilitar o uso das portas
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

// Cria um objeto do sensor DHT11
DHT dht(D2, DHTTYPE);

// Timers para disparar ações em um determinado tempo
BlynkTimer timerDHT, timerSolo, timerThingSpeak, timerNotify;

// Chaves de autenticação
const char blynk_auth[] = "***";

WiFiClient thingSpeak;
const char* server = "api.thingspeak.com";
const String keyThingSpeak = "***";

const char ssid[] = "***";
const char pass[] = "***";

// Variáveis dos sensores
float temperatura;
float umidade;
float umidadeSolo;

void setup() {
    Serial.begin(38400);
    
    // Inicializa o sensor DHT11
    dht.begin();
    
    // Inicia a conexão via Wi-Fi
    Blynk.begin(blynk_auth, ssid, pass);
    
    // Espera o Blynk conectar antes de iniciar
    while (!Blynk.connect()) {}
    
    // Configura o intervalo de tempo e a ação a ser disparada por cada timer
    timerDHT.setInterval(2000L, readDHT); // Dispara a cada 2 segundos
    timerSolo.setInterval(1000L, readSolo); // Dispara a cada 1 segundo
    timerThingSpeak.setInterval(20000L, sendToThingSpeak); // Dispara a cada 20 segundos
    timerNotify.setInterval(30000L, notificate); // Dispara a cada 30 segundos
}

void loop() {
    Blynk.run();
    timerDHT.run();
    timerSolo.run();
    timerThingSpeak.run();
    timerNotify.run();
}

// Leitura e envio de dados do sensor DHT11
void readDHT() {
    temperatura = dht.readTemperature();
    umidade = dht.readHumidity();
    
    if (!digitalRead(D1)) {
        // Informa que o sensor está desativado
        Blynk.virtualWrite(V0, "Desativado");
        Blynk.virtualWrite(V1, "Desativado");
    }
    else {
        if (!isnan(temperatura)) { // Só envia os dados se não ocorrer erro na leitura
            Blynk.virtualWrite(V0, temperatura);
            Serial.print("Temperatura:\t");
            Serial.print(temperatura);
            Serial.println("°C");
        }
        if (!isnan(umidade)) { // Só envia os dados se não ocorrer erro na leitura
            Blynk.virtualWrite(V1, umidade);
            Serial.print("Umidade:\t");
            Serial.print(umidade);
            Serial.println("%");
        }
    }
}

// Leitura e envio de dados do sensor de Umidade de Solo
void readSolo() {
    umidadeSolo = map(analogRead(A0), 216, 1023, 100, 0);
    Blynk.virtualWrite(V2, umidadeSolo);
    Serial.print("Umidade Solo:\t");
    Serial.print(umidadeSolo);
    Serial.println("%");
}

// Envio dos dados ao ThingSpeak
void sendToThingSpeak() {
    if (thingSpeak.connect(server, 80)) {
        Serial.println("Enviando informações para ThingSpeak...");
        String postStr = keyThingSpeak;

        // Os dados só são enviados se o sensor estiver ativado
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

// Envio de Notificações ao aplicativo do Blynk no smartphone
void notificate() {
    // Notifica quando a umidade do ar estiver muito alta
    if (umidade >= 80) {
        Blynk.notify("Alta Umidade! " + String(umidade) + "%");
    }
    
    // Notifica quando é preciso regar a planta
    if (umidadeSolo <= 20) {
        Blynk.notify("Baixa umidade no solo " + String(umidadeSolo) + "%\nRegue sua planta!");
    }
}
