#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2
const char* ssid = "Tryffin";
const char* password = "99504312";
const char* mqtt_serveur = "192.168.112.175";

WiFiClient espClient;
PubSubClient client(espClient);
long dernier_mesg = 0;

void setup() {
  //initialize Serial Monitor
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

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
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
  // Boucle jusqu'a reconncter
  while (!client.connected()) {
    Serial.print("Tenter la connection de MQTT..");
    // Tenter a reconnecter

    if (client.connect("ESP32Client")) {
      Serial.println("Connecte!");

    } else {
      Serial.print("Echec, Return Code=");
      Serial.print(client.state());
      Serial.println(" Reessayer apres 5s");
      delay(5000);
    }
  }
}

void loop() {
  // try to parse packet
  
  
  int CO2;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
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
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
  
  
}
