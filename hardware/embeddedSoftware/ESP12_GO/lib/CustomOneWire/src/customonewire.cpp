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

  while (digitalReadFast(pin)) 
    if (timeout--) {
      delayMicroseconds(1);
    } else {
      return 0;
    }

  delayMicroseconds(wait_us * 2); // Wait start bit + half a bit to sample on the middle of the signal
  for (int i = 7; i >= 0; i--) {
    *data |= digitalReadFast(pin) << i;
    delayMicroseconds(wait_us);
  }
};

int customonewire::sendByte(unsigned char data) {

  writeBit(0);
  delayMicroseconds(wait_us * 1.5);

  for (int i = 0; i < 8; i++) {
    writeBit((data << i) & (1 << 7));
    delayMicroseconds(wait_us);
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