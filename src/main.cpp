#include "DriverManager.hpp"

DriverManager driverManager;

void setup() {
  
  Serial.begin(9600);
  
  driverManager.initAllDrivers();
  driverManager.startAllDrivers();
  delay(500);

  ESP32Server::GetInstance()->init();
  delay(500);
  Serial.println("Serwer HTTP uruchomiony na porcie 8080");
}

void loop() {
  ESP32Server::GetInstance()->start();
}
