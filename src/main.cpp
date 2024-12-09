#include "DriverManager.hpp"

DriverManager driverManager;

void setup() {
  Serial.begin(9600);
  driverManager.initAllDrivers();
  driverManager.startAllDrivers();
}

void loop() {
  Serial.print(".");
  vTaskDelay(1000);
}
