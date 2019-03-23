#include "customonewire.h"

#define PIN_EN_NEXT 12 // Pin qui va au reset du prochain bloc
#define PIN_ONEWIRE 14 // Le bus
#define FREQ_ONEWIRE 50 // KHz

customonewire* wire;

struct module_struct {
  unsigned int addr;
  unsigned int type;
  unsigned int value;
};

unsigned int module_count;
struct module_struct modules[100];
int shouldreset;

void setup() {
  pinMode(PIN_EN_NEXT, OUTPUT); // On reset le module suivant
  digitalWrite(PIN_EN_NEXT, LOW);

  wire = new customonewire(PIN_ONEWIRE, FREQ_ONEWIRE);

  digitalWrite(PIN_EN_NEXT, HIGH); // On réveille le bloc suivant, qui va annoncer son addresse puis réveiller le prochain
  unsigned char data = 0;
  for (int i = 0; i < 255; i++) {
    if (wire->readByte(&data, 1000000)) {
      Serial.print("Received byte : ");
      Serial.println(data);
      Serial.printf("Module %d has address 0x%2X", i, data);
      modules[i].addr = data;
    } else {
      Serial.println("Timed out waiting for a module");
      module_count = i;
      break;
    } 
  }

  Serial.printf("Found %d modules.", module_count);

  for (int i = 0; i < module_count; i++) {
    Serial.printf("Requesting type of module %d", i);
    wire->sendByte(modules[i].addr); // On envoie l'addresse
    delayMicroseconds(1000); // On attend un peu
    wire->sendByte(0x01); // On envoie la valeur pour demander le type

    if (wire->readByte(&data, 15000)) {
      if (data == modules[i].addr) { // On devrait recevoir l'adresse du module
        Serial.println("Module answered");
        if (wire->readByte(&data, 15000)) {
          Serial.printf("Module type : %d", data);
          modules[i].type = data;
        }
        else {
          Serial.println("Timed out waiting for type");
          shouldreset = 1;
        }
      }
      else {
        Serial.println("Unexpected address replied !");
        shouldreset = 1;
      }
    }
    else {
      Serial.println("Timed out waiting for module's answer");
      shouldreset = 1;
    }
    delayMicroseconds(1000); // On attend un peu
  }

  for (int i = 0; i < module_count; i++) {
    Serial.printf("Requesting value of module %d", i);
    wire->sendByte(modules[i].addr); // On envoie l'addresse
    delayMicroseconds(1000); // On attend un peu
    wire->sendByte(0x00); // On envoie la valeur pour demander le type

    if (wire->readByte(&data, 15000)) {
      if (data == modules[i].addr) { // On devrait recevoir l'adresse du module
        Serial.println("Module answered");
        if (wire->readByte(&data, 15000)) {
          Serial.printf("Module value : %d", data);
          modules[i].value = data;
        }
        else {
          Serial.println("Timed out waiting for value");
          shouldreset = 1;
        }
      }
      else {
        Serial.println("Unexpected address replied !");
        shouldreset = 1;
      }
    }
    else {
      Serial.println("Timed out waiting for module's answer");
      shouldreset = 1;
    }
    delayMicroseconds(1000); // On attend un peu
  }

  // Debug
  Serial.begin(115200);
}

int i ;

void loop() {
  unsigned char data;
  for (int i = 0; i < module_count; i++) {
    Serial.printf("Requesting value of module %d", i);
    wire->sendByte(modules[i].addr); // On envoie l'addresse
    delayMicroseconds(1000); // On attend un peu
    wire->sendByte(0x00); // On envoie la valeur pour demander le type

    if (wire->readByte(&data, 15000)) {
      if (data == modules[i].addr) { // On devrait recevoir l'adresse du module
        Serial.println("Module answered");
        if (wire->readByte(&data, 15000)) {
          Serial.printf("Module value : %d", data);
          modules[i].value = data;
        }
        else {
          Serial.println("Timed out waiting for value");
          shouldreset = 1;
        }
      }
      else {
        Serial.println("Unexpected address replied !");
        shouldreset = 1;
      }
    }
    else {
      Serial.println("Timed out waiting for module's answer");
      shouldreset = 1;
    }
    delayMicroseconds(1000); // On attend un peu
  }
}
