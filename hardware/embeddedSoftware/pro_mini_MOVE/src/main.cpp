#include <Arduino.h>
#include "customonewire.h"
#include <digitalWriteFast.h>


#define ADDRESS 0x09 // Module's address

#define PIN_EN_NEXT 10 // Pin qui va au reset du prochain bloc
#define PIN_ONEWIRE 11 // Le bus
#define FREQ_ONEWIRE 25 // KHz
#define PIN_LED 12 // La led qui s'allume quand le bloc est exécuté
#define BLOC_TYPE 0x01 // Type de bloc à annoncer au maître

#define ENC_A   2
#define ENC_B   3

#define NB_LED  5

int led[] = {4, 5, 6, 7, 8};

int posLED = 0;

customonewire* wire;

void gaucheINTER() {
  if(digitalRead(ENC_B))
    posLED++;
  else
    posLED--;
}

void writeLED(int nbLED) {
  int i = 0;
  for (; i < nbLED; i++) {
    digitalWrite(led[i], HIGH);
  }
  for (; i < NB_LED; i++) {
    digitalWrite(led[i], LOW);
  }
}

void setup() {

  pinMode(PIN_EN_NEXT, OUTPUT); // On reset le module suivant
  digitalWrite(PIN_EN_NEXT, LOW);

  wire = new customonewire(PIN_ONEWIRE, FREQ_ONEWIRE);

  wire->sendByte(ADDRESS); // On s'annonce avec notre adresse

  delay(100); // On attend un peu

  digitalWrite(PIN_EN_NEXT, HIGH); // On réveille le bloc suivant

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  //interrupt on ENC_A
  attachInterrupt(digitalPinToInterrupt(ENC_A), gaucheINTER, RISING);
  //set pinMode for all the LEDs
  for (int i = 0; i < NB_LED; i++) {
    pinMode(led[i], OUTPUT);
  }

}

void loop() {

  if (posLED < 0) posLED = 0;
  else if (posLED > NB_LED) posLED = NB_LED;
  //update the value on LEDs
  writeLED(posLED);

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
        case 0x00 : // Le maître demande la valeur du bloc
          wire->sendByte(ADDRESS);
          delayMicroseconds(1000);
          wire->sendByte((unsigned char) posLED); // Envoyer la réponse ici
//          Serial.print("Received value query, answering ");
//          Serial.println(0xAA);
          break;

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

        case 0x11 : // Eteindre la LED
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
