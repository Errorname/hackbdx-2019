#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <digitalWriteFast.h>

#include "customonewire.h"

#define PIN_EN_NEXT 10 // Pin qui va au reset du prochain bloc
#define PIN_ONEWIRE 13 // Le bus
#define FREQ_ONEWIRE 40 // KHz

customonewire* wire;

struct module_struct {
  unsigned int addr;
  unsigned int type;
  unsigned int value;
};

unsigned int module_count;
struct module_struct modules[20];
int shouldreset;

const char* ssid      = "Pierre";
const char* pwd       = "pjoubertpj";
const int httpPort    = 80;
const char* host      = "http://us-central1-hackbordeaux-2019.cloudfunctions.net";

//create client object
WiFiClient client;

HTTPClient http;


void sendGET(String host) {
  if (http.begin(client, host)) {  // HTTP


    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
}

void setup() {

    //set Serial comm
  Serial.begin(9600);

  pinMode(PIN_EN_NEXT, OUTPUT); // On reset le module suivant
  digitalWrite(PIN_EN_NEXT, LOW);

  wire = new customonewire(PIN_ONEWIRE, FREQ_ONEWIRE);

  digitalWrite(PIN_EN_NEXT, HIGH); // On réveille le bloc suivant, qui va annoncer son addresse puis réveiller le prochain
  unsigned char data = 0;
  for (int i = 0; i < 20; i++) {
    if (wire->readByte(&data, 700000) && data != 0xFF) {
      Serial.print("Received byte : ");
      Serial.println(data);
      Serial.printf("Module %d has address 0x%2X", i, data);
      modules[i].addr = data;
    } else {
      Serial.println("Timed out waiting for a module");
      module_count = i;
      break;
    }
    delayMicroseconds(1000); // On attend un peu
  }
  ESP.wdtFeed();

  Serial.printf("Found %d modules.", module_count);

  for (unsigned int i = 0; i < module_count; i++) {
    Serial.printf("Requesting type of module %d", i);
    wire->sendByte(modules[i].addr); // On envoie l'addresse
    delayMicroseconds(1000); // On attend un peu
    wire->sendByte(0x01); // On envoie la valeur pour demander le type

    if (wire->readByte(&data, 150000)) {
      if (data == modules[i].addr) { // On devrait recevoir l'adresse du module
        Serial.println("Module answered");
        if (wire->readByte(&data, 150000)) {
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
  ESP.wdtFeed();

  for (unsigned int i = 0; i < module_count; i++) {
    Serial.printf("Requesting value of module %d", i);
    wire->sendByte(modules[i].addr); // On envoie l'addresse
    delayMicroseconds(1000); // On attend un peu
    wire->sendByte(0x00); // On envoie la valeur pour demander le type

    if (wire->readByte(&data, 150000)) {
      if (data == modules[i].addr) { // On devrait recevoir l'adresse du module
        Serial.println("Module answered");
        if (wire->readByte(&data, 150000)) {
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
  ESP.wdtFeed();


  //start wifi connexion
  WiFi.begin(ssid, pwd);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  //waiting for connexion
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ESP.wdtFeed();
  String url = (String) host + "/setCurrentProgram?stack=";
  for (unsigned int i = 0; i < module_count; i++) {
    if(modules[i].type > 1) {
      url += (String)modules[i].addr + "|" + (String)wire->type[modules[i].type];
      Serial.println(url);
      sendGET(url);
    } else {
      url += (String)modules[i].addr + "|" + (String)wire->type[modules[i].type] + "-" + (String)modules[i].value;
      Serial.println(url);
      sendGET(url);
    }
  }

  url = (String)host + "/start";
  sendGET(url);

}

void loop() {
  
}