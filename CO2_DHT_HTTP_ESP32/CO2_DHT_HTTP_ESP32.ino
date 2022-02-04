#include <DHT.h>
#include <DHT_U.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define DHTPIN 5 //Pin pour connecter la data du DHT11
#define DHTTYPE DHT11 

// spécifier le nom et le mot de passe de wifi
const char* ssid = "Tryffin";
const char* password = "99504312";

// définir un nom de device
String device_label = "ESP32_DHT11_JY";

// Entrer votre adresse IP + 1880 + nom de URL
String serverNamePost = "https://www.tryffin.tk:1880/updatesensor";

DHT dht(DHTPIN, DHTTYPE);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int analogPin = 35;


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

  WiFi.begin(ssid, password); // connecter le wifi

  //verification de la connection
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  dht.begin(); // mettre à jour le DHT11

  display.clearDisplay(); //initialisation ecran
  // configurer la taille, la couleur et le point départ de text
  display.setTextSize(2); 
  display.setTextColor(WHITE);
}


void loop(){
    
  if(WiFi.status()== WL_CONNECTED){   
  
    HTTPClient http; 
    WiFiClient client;
    http.begin(client, serverNamePost); // associer à l'adresse du POST
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Spécifer le type de tête du HTTP

    float CO2 = ReadCO2(); //lire la concentration CO2
    float hum = ReadHum(); //lire la valeur de l'humidité
    float temp = ReadTemp(); //lire la valeur de température
  
    String s = "api_key=" + device_label + "&sensor=CO2"+ "&CO2=" + String(CO2) + "&sensor=DHT11"+ "&humidity=" + String(hum) + "&sensor=DHT11" + "&temperature=" + String(temp);

    int httpResponseCodePost = http.POST(s); //envoyer la demande de POST

    // vérification de la succès de l'envoie 
    if(httpResponseCodePost>0){
      String response = http.getString(); //Obtenez la réponse à la demande de POST
      Serial.println(response); //Afficher la réponse à la demande de POST
    }else{
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCodePost);
    }
  
    http.end();  //libérer la source
    
  
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
    
  }else{
    Serial.println("Error for WiFi connection");   
  }
  display.clearDisplay();
  delay(5000);
}
