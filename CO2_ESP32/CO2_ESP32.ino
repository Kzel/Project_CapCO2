#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int analogPin = 4;

void setup(){
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  
  display.clearDisplay(); //initialisation ecran
  // configurer la taille, la couleur et le point départ de text
  display.setTextSize(2); 
  display.setTextColor(WHITE);
}

void loop(){
  //read voltage
  int sensorValue = analogRead(analogPin);
  Serial.println("sensorValue:");
  Serial.println(sensorValue);
  // converte the analog signal to a voltage
  float voltage = sensorValue*(3.3/4095.0);
  if(voltage == 0)
  {
    Serial.println("FAULT");
  }
  else if(voltage < 0.4)
  {
    Serial.println("pre-heating");
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

    // Display static text
    display.setCursor(50, 0); 
    display.println("CO2");

    display.setCursor(0, 20);
    display.println(concentration);

    display.setCursor(85, 20);
    display.println("ppm");
    display.display();
  }
  delay(5000);
  display.clearDisplay(); 
}
