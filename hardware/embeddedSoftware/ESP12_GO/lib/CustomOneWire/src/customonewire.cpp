#include "customonewire.h"
#include <Arduino.h>
#include <digitalWriteFast.h>

customonewire::customonewire(int pin_, int freq_khz) {
  pin = pin_;
  digitalWriteFast(pin, 0);
  wait_us = 1000 / freq_khz;
};

int customonewire::readByte(unsigned char* data, unsigned long int timeout) {
  pinMode(pin, INPUT); // Listen to line
  *data = 0;

  while (digitalReadFast(pin)) 
    if (timeout--) {
      delayMicroseconds(1);
    } else {
      return 0;
    }

  int seuil = 0;
  while (!digitalReadFast(pin)) { // Mesure du temps bas
    delayMicroseconds(1);
    seuil++;
  }

  int bit_time;
  for (int i = 7; i >= 0; i--) {
    bit_time = 0;  
    while (digitalReadFast(pin)) { // Mesure du temps haut
      delayMicroseconds(1);
      bit_time++;
    }

    if (bit_time > seuil) { // Temps haut > 1.5 temps, donc bit à 1
      *data |= 1 << i;
    }

    while(!digitalReadFast(pin)) ; // Attente de la remontée du signal pour le prochain bit
  }
  Serial.printf("dernier bit_time : %d, seuil : %d\r\n", bit_time, seuil);
};

int customonewire::sendByte(unsigned char data) {

  writeBit(0);
  delayMicroseconds(wait_us*1.5); // Temps bas pendant 1 cycle et demi (ça donne le temps à utiliser pour le seuil)

  for (int i = 0; i < 8; i++) {
    writeBit(1);
    if ((data << i) & (1 << 7)) {
      delayMicroseconds(wait_us*2);  // Bit à un = signal haut pendant deux temps
    }
    else { 
      delayMicroseconds(wait_us); // Bit à 0 = signal haut pendant un temps
    }

    writeBit(0);
    delayMicroseconds(wait_us); // Un temps mort à 0
  }
  pinMode(pin, INPUT); // Release line
};

void customonewire::writeBit(int val) {
  if (val) {
    pinMode(pin, INPUT);
  } else {
    pinMode(pin, OUTPUT);
  }
}