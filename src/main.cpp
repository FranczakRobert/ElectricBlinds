#include "DriverManager.hpp"

DriverManager driverManager;

void setup() {
  Serial.begin(9600);

  driverManager.startAllDrivers();
}

void loop() {
  ESP32Server::GetInstance()->start();
}
