// Configurações do Blynk
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

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

// Timers para disparar ações em um determinado tempo
BlynkTimer timerNivel, timerNotify;

// Chave de autenticação
const char blynk_auth[] = "***";

const char ssid[] = "***";
const char pass[] = "***";

// Variável do sensor
float nivel;

void setup() {
    Serial.begin(38400);
    
    // Inicia a conexão via Wi-Fi
    Blynk.begin(blynk_auth, ssid, pass);

    // Espera o Blynk conectar antes de iniciar
    while (!Blynk.connect()) {}
    
    // Configura o intervalo de tempo e a ação a ser disparada por cada timer
    timerNivel.setInterval(1000L, readNivel); // Dispara a cada 1 segundo
    timerNotify.setInterval(30000L, notificate); // Dispara a cada 30 segundos
}

void loop() {
    Blynk.run();
    timerNivel.run();
    timerNotify.run();
}

// Leitura e envio de dados do sensor de Nível
void readNivel() {
    nivel = analogRead(A0);
    
    if (nivel<=480){ 
        Blynk.virtualWrite(V3, "Caixa Vazia!");
        Serial.println("Nível de água: Vazio!"); 
    }
    else if (nivel>480 && nivel<=530){ 
        Blynk.virtualWrite(V3, "0mm ~ 5mm");
        Serial.println("Nível de água: 0mm a 5mm"); 
    }
    else if (nivel>530 && nivel<=615){
        Blynk.virtualWrite(V3, "5mm ~ 10mm");
        Serial.println("Nível de água: 5mm a 10mm"); 
    }
    else if (nivel>615 && nivel<=660){ 
        Blynk.virtualWrite(V3, "10mm ~ 15mm");
        Serial.println("Nível de água: 10mm a 15mm"); 
    }   
    else if (nivel>660 && nivel<=680){ 
        Blynk.virtualWrite(V3, "15mm ~ 20mm");
        Serial.println("Nível de água: 15mm a 20mm"); 
    }
    else if (nivel>680 && nivel<=690){ 
        Blynk.virtualWrite(V3, "20mm ~ 25mm");
        Serial.println("Nível de água: 20mm a 25mm"); 
    }
    else if (nivel>690 && nivel<=700){ 
        Blynk.virtualWrite(V3, "25mm ~ 30mm");
        Serial.println("Nível de água: 25mm a 30mm"); 
    }
    else if (nivel>700 && nivel<=705){ 
        Blynk.virtualWrite(V3, "30mm ~ 35mm");
        Serial.println("Nível de água: 30mm a 35mm"); 
    }
    else if (nivel>705){ 
        Blynk.virtualWrite(V3, "35mm ~ 40mm");
        Serial.println("Nível de água: 35mm a 40mm"); 
    }
}

// Envio de Notificações ao aplicativo do Blynk no smartphone
void notificate() {
    // Notifica quando não há água ou quando o nível está muito baixo
    if (nivel<=480){ 
        Blynk.notify("Caixa D'água vazia. :(");
    }
    else if (nivel>480 && nivel<=530){ 
        Blynk.notify("Baixo nível de água na caixa.");
    }
}
