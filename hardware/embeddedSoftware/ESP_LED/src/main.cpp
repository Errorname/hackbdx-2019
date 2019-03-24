#include <Arduino.h>
#include "customonewire.h"
#include <digitalWriteFast.h>


#define ADDRESS 0x07 // Module's address

#define PIN_EN_NEXT 10 // Pin qui va au reset du prochain bloc
#define PIN_ONEWIRE 11 // Le bus
#define FREQ_ONEWIRE 2 // KHz
#define PIN_LED 12 // La led qui s'allume quand le bloc est exécuté
#define BLOC_TYPE 0x04 // Type de bloc à annoncer au maître


customonewire* wire;


void setup() {

  pinMode(PIN_EN_NEXT, OUTPUT); // On reset le module suivant
  digitalWrite(PIN_EN_NEXT, LOW);

  pinMode(PIN_LED, OUTPUT);

  wire = new customonewire(PIN_ONEWIRE, FREQ_ONEWIRE);

  wire->sendByte(ADDRESS); // On s'annonce avec notre adresse

  delay(100); // On attend un peu

  digitalWrite(PIN_EN_NEXT, HIGH); // On réveille le bloc suivant

}

void loop() {

  // On attend un ordre
  unsigned char data = 0;
  if(wire->readByte(&data, 1500000)) {
//    Serial.print("Address : ");
//    Serial.println(data, HEX);

    // C'est l'adresse de ce module
    if (data == ADDRESS) {
//      Serial.println("Message for this module !");
      wire->readByte(&data, 15000);
//      Serial.print("Data : ");
//      Serial.println(data, HEX);

      switch(data) {

        case 0x01 : // Le maître demande le type de bloc
          wire->sendByte(ADDRESS);
          delayMicroseconds(1000);
          wire->sendByte(BLOC_TYPE); // Envoyer la réponse ici
//          Serial.print("Received type query, answering ");
//          Serial.println(BLOC_TYPE);
          break;

        case 0x10 : // Allumer la LED
          digitalWrite(PIN_LED, HIGH);
//          Serial.println("Turning LED on");
          break;

        case 0x11 : // EteinPIN_LEDdre la LED
          digitalWrite(PIN_LED, LOW);
//          Serial.println("Turning LED off");
          break;
      }

    } else {
      if (wire->readByte(&data, 15000)) {
//        Serial.print("Data : ");
//        Serial.println(data, HEX);
      }
    }
  }
}
