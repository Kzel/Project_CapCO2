#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define DHTPIN 23 //Pin pour connecter la data du DHT11
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int analogPin = 4;


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
  
  display.clearDisplay(); //initialisation ecran
  // configurer la taille, la couleur et le point départ de text
  display.setTextSize(2); 
  display.setTextColor(WHITE);
}


void loop(){

  float CO2 = ReadCO2(); //lire la concentration CO2
  float hum = ReadHum(); //lire la valeur de l'humidité
  float temp = ReadTemp(); //lire la valeur de température

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
  delay(5000);
}
