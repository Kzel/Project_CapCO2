#include <DHT.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define DHTPIN 25 //Pin pour connecter la data du DHT11
#define DHTTYPE DHT11 

#define LEDPINR 33
#define LEDPINV 32

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Tryffin"; // Remplacer avec votre ssid de Wifi
const char* password = "99504312"; // Remplacer avec votre mot de passe de Wifi

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int analogPin = 35;

const char* mqtt_serveur = "192.168.112.175"; // L'adresse IP du Cloud
// Le client et le dernier message
WiFiClient espClient;
PubSubClient client(espClient);
long dernier_mesg = 0;


// fonction pour lire la valeur de humidité
float ReadHum(){
  float hum = dht.readHumidity();
  if (isnan(hum)) {    
    Serial.println("erreur de lire");
    return NULL;
  }
  else {
    Serial.println("Humidity:");
    Serial.println(hum);
    return hum;
  }
}


// fonction pour lire la valeur de température
float ReadTemp(){
  float temp = dht.readTemperature();
  if (isnan(temp)) {    
    Serial.println("erreur de lire");
    return NULL;
  }
  else {
    Serial.println("Temperature:");
    Serial.println(temp);
    return temp;
  }   
}


// fonction pour lire la valeur de concentration CO2
float ReadCO2(){
  //read voltage
  int sensorValue = analogRead(analogPin);
  
  // converte the analog signal to a voltage
  float voltage = sensorValue*(3.3/4095.0);
  if(voltage == 0)
  {
    Serial.println("FAULT");
    return NULL;
  }
  else if(voltage < 0.4)
  {
    Serial.println("pre-heating");
    return NULL;
  }
  else
  {
    // calculate for concentration CO2
    float voltage_diference=voltage-0.4;
    float concentration=voltage_diference*5000.0/1.6;
    
    // Print Voltage
    Serial.print("voltage:");
    Serial.print(voltage);
    Serial.println("V");
    
    //Print CO2 concentration
    Serial.print(concentration);
    Serial.println("ppm");
    return concentration;
  }
}


void setup(){
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);

  dht.begin(); // mettre à jour le DHT11

  pinMode(LEDPINR,OUTPUT);
  pinMode(LEDPINV,OUTPUT);
  
  // Connecter avec Wifi
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
  
  display.clearDisplay(); //initialisation ecran
  
  // configurer la taille, la couleur et le point départ de text
  display.setTextSize(2); 
  display.setTextColor(WHITE);
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

void loop(){
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  float CO2 = ReadCO2(); //lire la concentration CO2
  float hum = ReadHum(); //lire la valeur de l'humidité
  float temp = ReadTemp(); //lire la valeur de température

  if(CO2 >= 1000 ){
      digitalWrite(LEDPINV,LOW);
      digitalWrite(LEDPINR,HIGH);
  }else{
      digitalWrite(LEDPINR,LOW);
      digitalWrite(LEDPINV,HIGH);
  }

  // Display static text
  display.setCursor(0, 0); 
  display.println(CO2);
  display.setCursor(90, 0);
  display.println("ppm");

  display.setCursor(0, 25); 
  display.println("H: ");
  display.setCursor(25, 25);
  display.println(hum);
  display.setCursor(90, 25);
  display.println("%");

  display.setCursor(0, 50); 
  display.println("T:");
  display.setCursor(25, 50);
  display.println(temp);
  display.setCursor(90, 50);
  display.println("C");
  
  display.display();
  display.clearDisplay();

  long actuel = millis();
  if (actuel - dernier_mesg > 5000) {
    dernier_mesg = actuel;
    
    // Lire la temperature par DHT11
    // Convertir la temperature en array de char et Publish dans le topic de temperature
    char tempChar[8];
    dtostrf(temp, 1, 2, tempChar);
    client.publish("esp32/temp", tempChar);
    
    // Convertir l'humidite en array de char et Publish dans le topic de temperature
    char humdChar[8];
    dtostrf(hum, 1, 2, humdChar);
    client.publish("esp32/hum", humdChar);

    // Convertir l'humidite en array de char et Publish dans le topic de temperature
    char co2Char[8];
    dtostrf(CO2, 1, 2, co2Char);
    client.publish("esp32/co2", co2Char);
  }
  delay(5000);
}
