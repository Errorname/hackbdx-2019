#include <Arduino.h>
#include <ESP8266WiFi.h>

#define CONF_PIN  16
const char* ssid      = "azazaz";
const char* pwd       = "azazazazazaz";
const int httpPort    = 80;
const char* host      = "blocky_and_wheely.com";

//create client object
WiFiClient client;

void sendGET() {

}

void setup() {
  //set Serial comm
  Serial.begin(9600);

  pinMode(CONF_PIN, OUTPUT);
  digitalWrite(CONF_PIN, LOW);

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

}

void loop() {
  
  //try to connect to the client
  if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
  }

  while (client.available()) {
    //trame received
    String line = client.readStringUntil('\r');
  }
}