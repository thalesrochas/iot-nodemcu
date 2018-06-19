// Configurações do Blynk
#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT

#include <BlynkSimpleSerialBLE.h>

// Chave de autenticação
char blynk_auth[] = "***";

void setup() {
    Serial.begin(38400);

    // Inicia a conexão via Bluetooth
    Blynk.begin(blynk_auth, Serial);

    // Espera o Blynk conectar antes de iniciar
    while (!Blynk.connect()) {}
}

void loop() {
    Blynk.run();
}
