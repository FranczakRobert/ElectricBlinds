#include "DriverManager.hpp"
#include "test.hpp"

DriverManager driverManager;

void setup() {
  Serial.begin(9600);
  vTaskDelay(3000);

  if(E_OK == driverManager.initAllDrivers()) {
    Serial.println("[Driver Manager] - Init all drivers.");
    if(E_OK == driverManager.startAllDrivers()) {
      Serial.println("[Driver Manager] - Started all drivers.");
    } else {
      Serial.println("[Driver Manager] - Started all drivers FAILED.");
    }
  } else {
    Serial.println("[Driver Manager] - Init all drivers FAILED.");
  }
}

void loop() {
  Serial.print(".");
  vTaskDelay(1000);
}
