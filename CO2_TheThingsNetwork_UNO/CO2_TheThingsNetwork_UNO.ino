#include <TheThingsNetwork.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Set your AppEUI and AppKey
const char *appEui = "0000000000000000";
const char *appKey = "7FA6FEA830ADCBA6B4C9C556C69A6B46";

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) and TheThingsNetwork
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

int sensorIn = A0;

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(115200);
  analogReference(DEFAULT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);

  display.clearDisplay(); //initialisation ecran
  // configurer la taille, la couleur et le point départ de text
  display.setTextSize(2); 
  display.setTextColor(WHITE);
  
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;
  debugSerial.println("-- STATUS");
  ttn.showStatus();
  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
}

void loop()
{
  debugSerial.println("-- SENSOR");
  int sensorValue = analogRead(sensorIn);

  // The analog signal is converted to a voltage
  float voltage = sensorValue * (5000 / 1024.0);
  if (voltage == 0)
  {
    Serial.println("Fault");
  }
  else if (voltage < 400)
  {
    Serial.println("preheating");
  }
  else
  {
    int voltage_diference = voltage - 400;
    float concentration = voltage_diference * 50.0 / 16.0;
    
    // Print Voltage
    Serial.print("voltage:");
    Serial.print(voltage);
    Serial.println("mv");
    
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
    
    // For conversion into centaine for better bytes code format
    int concentraion2;
    concentraion2 = (int)concentration/10;
    
    // LOOP for sending CO2 concentration data
    debugSerial.println("-- SEND");
    byte payload[1];
    payload[0] = concentraion2;
    // Send its value
    ttn.sendBytes(payload, sizeof(payload));
  }
  
  // Prepare payload of 1 byte to indicate 
  delay(5000);
  display.clearDisplay(); 
}
