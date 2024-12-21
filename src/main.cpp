#include "DriverManager.hpp"

DriverManager driverManager;

void setup() {
  Serial.begin(9600);
  if(E_OK == driverManager.initAllDrivers()) {
    Serial.println("[Driver Manager] - Init all drivers.");
    if(E_OK == driverManager.startAllDrivers()) {
      Serial.println("[Driver Manager] - Started all drivers.");
    }
  }
  
}

void loop() {
  Serial.print(".");
  vTaskDelay(1000);
}
