#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT

#include <BlynkSimpleSerialBLE.h>

char blynk_auth[] = "***";

void setup() {
    Serial.begin(38400);

    Blynk.begin(blynk_auth, Serial);
    while (!Blynk.connect()) {}
}

void loop(){
    Blynk.run();
}
