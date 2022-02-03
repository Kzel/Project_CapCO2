int analogPin = 4;

void setup(){
  Serial.begin(115200);
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
  }
  delay(5000);
}
