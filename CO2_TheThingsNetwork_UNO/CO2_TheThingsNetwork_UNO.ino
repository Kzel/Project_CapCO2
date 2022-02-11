#include <TheThingsNetwork.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const char *appEui = "0000000000000000";
const char *appKey = "7FA6FEA830ADCBA6B4C9C556C69A6B46";

#define freqPlan TTN_FP_EU868

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
TheThingsNetwork ttn(Serial1, Serial, TTN_FP_EU868);

DHT dht(6, DHT11);
void setup()
{
  Serial1.begin(57600);
  Serial.begin(115200);
  analogReference(DEFAULT);
  dht.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  display.clearDisplay(); 
  display.setTextSize(2); 
  display.setTextColor(WHITE);
  ttn.join(appEui, appKey);
}

void loop()
{
  
    int sensorValue = analogRead(A0);
  
    float voltage = sensorValue * (5000 / 1024.0);
    int voltage_diference = voltage - 400;
    float concentration = voltage_diference * 50.0 / 16.0;
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    if(concentration > 1000){
      digitalWrite(12, 1);
      digitalWrite(13, 0);
    }else{
      digitalWrite(12, 0);
      digitalWrite(13, 1);   
    }
    
  display.setCursor(0, 0); 
  display.println(concentration);
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
   
  int concentraion2;
  concentraion2 = (int)concentration/20;
  byte payload[4];
  payload[0] = concentraion2;
  payload[1] = (int) hum;
  payload[2] = (int) temp;

  ttn.sendBytes(payload, sizeof(payload));
 
  delay(5000);
  display.clearDisplay();
}
