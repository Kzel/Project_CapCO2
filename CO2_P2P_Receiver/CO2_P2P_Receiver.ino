#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Definir les pins pour LoRa module
#define ss 5
#define rst 14
#define dio0 2

// Replace par votre WiFi SSID et pwd and server address
const char* ssid = "Tryffin";
const char* password = "99504312";
const char* mqtt_serveur = "192.168.112.175";

WiFiClient espClient;
PubSubClient client(espClient);
long dernier_mesg = 0;

void setup() {
  // Initialiser Serial
  Serial.begin(115200);
  while (!Serial);
   WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("En cours de connecter a Wifi\n");
  }
  
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Succes!\n");
  }
  
  Serial.println(WiFi.localIP());
  
  // Positionner le serveur avec le port 1883 et le callback
  client.setServer(mqtt_serveur, 1883);
  client.setCallback(callback);
  Serial.println("LoRa Receiver");

  // Setup LoRa module
  LoRa.setPins(ss, rst, dio0);
  
  // Remplace par la frequency de votre location
  //433E6 pour Asie
  //866E6 pour Europe
  //915E6 pour Amerique du Nord
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
  // Changer sync word (0xF3) pour correspondre a recepteur
  // Le sync word assure que vous ne recevez pas LoRa messages a partir les autre LoRa module
  // De 0 a 0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initialiser OK!");
}

void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Boucle jusqu'a reconnect
  while (!client.connected()) {
    Serial.print("Tenter la connection de MQTT..");

    if (client.connect("ESP32Client")) {
      Serial.println("Connectee!");

    } else {
      Serial.print("Failed, Return Code=");
      Serial.print(client.state());
      Serial.println("Ressayer apres 2s");
      delay(2000);
    }
  }
}

void loop() {

  // Essayer de parser packet
  int CO2;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Recevoir un packet
    Serial.print("Recu packet '");
    // Lire packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData.toInt());
      CO2 =  LoRaData.toInt();
      if (!client.connected()) {
          reconnect();
        }
        client.loop();  
      long actuel = millis();
      if (actuel - dernier_mesg > 5000) {
          dernier_mesg = actuel;
          char co2Char[8];
          dtostrf(CO2, 1, 2, co2Char);
          client.publish("esp32/co2", co2Char);
        }
    }
    // afficher RSSI de packet
    Serial.print("' avec RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
