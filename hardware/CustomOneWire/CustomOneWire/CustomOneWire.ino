#include "customonewire.h"


#define ADDRESS 0x01 // Module's address

#define PIN_EN_NEXT 12 // Pin qui va au reset du prochain bloc
#define PIN_ONEWIRE 14 // Le bus
#define FREQ_ONEWIRE 50 // KHz
#define PIN_LED 13 // La led qui s'allume quand le bloc est exécuté
#define BLOC_TYPE 0x01 // Type de bloc à annoncer au maître

customonewire* wire;

void setup() {
  pinMode(PIN_EN_NEXT, OUTPUT); // On reset le module suivant
  digitalWrite(PIN_EN_NEXT, LOW);

  wire = new customonewire(PIN_ONEWIRE, FREQ_ONEWIRE);

  wire->sendByte(ADDRESS); // On s'annonce avec notre adresse

  delay(100); // On attend un peu

  digitalWrite(PIN_EN_NEXT, HIGH); // On réveille le bloc suivant

  // Debug
  Serial.begin(115200);
}

int i ;

void loop() {

  // On attend un ordre
  unsigned char data = 0;
  if(wire->readByte(&data, 1500000)) {
    Serial.print("Address : ");
    Serial.println(data, HEX);

    // C'est l'adresse de ce module
    if (data == ADDRESS) {
      Serial.println("Message for this module !");
      wire->readByte(&data, 15000);
      Serial.print("Data : ");
      Serial.println(data, HEX);

      switch(data) {
        case 0x00 : // Le maître demande la valeur du bloc
          wire->sendByte(ADDRESS);
          delayMicroseconds(1000);
          wire->sendByte(0xAA); // Envoyer la réponse ici
          Serial.print("Received value query, answering ");
          Serial.println(0xAA);
          break;

        case 0x01 : // Le maître demande le type de bloc
          wire->sendByte(ADDRESS);
          delayMicroseconds(1000);
          wire->sendByte(BLOC_TYPE); // Envoyer la réponse ici
          Serial.print("Received type query, answering ");
          Serial.println(BLOC_TYPE);
          break;

        case 0x10 : // Allumer la LED
          digitalWrite(PIN_LED, HIGH);
          Serial.println("Turning LED on");
          break;

        case 0x11 : // Eteindre la LED
          digitalWrite(PIN_LED, LOW);
          Serial.println("Turning LED off");
          break;

        default :
          Serial.println("Unknown code !");
      }

    } else {
      if (wire->readByte(&data, 15000)) {
        Serial.print("Data : ");
        Serial.println(data, HEX);
      }
    }
  }
}
