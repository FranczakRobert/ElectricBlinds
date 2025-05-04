#include "DriverManager.hpp"
#include "test.hpp"
#include "NvmMemory.hpp"
#include "ServerTests.hpp"
#include <Preferences.h>
#include "Display.hpp"

class Preferences;

ServerTests tests;
DriverManager driverManager;

#define TEST 0

void setup() {  
  Serial.begin(9600);
  vTaskDelay(3000);

  if(!TEST) {
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
  else {
    tests.initDeinit();
  }

}

void loop() {
  Serial.print(".");
  delay(1000);
}
