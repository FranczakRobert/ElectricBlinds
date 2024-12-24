#include "WifiDriver.hpp"
#include "WiFi.h"
#include "WifiConfig.hpp"
#include "DriverManager.hpp"

WifiDriver::WifiDriver(DriverManager *driverManager)
{
  this->driverManager = driverManager;
  isConnected = WIFI_NOT_CONNECTED;
  isRunning = 1;
}

WifiDriver::~WifiDriver()
{
}

ErrorCode WifiDriver::init() {

  return E_OK;
}

ErrorCode WifiDriver::deinit() {
 return E_OK;
}

ErrorCode WifiDriver::stop() {
  WiFi.disconnect();
  isRunning = 0;
  vTaskDelay(30 / portTICK_PERIOD_MS);
  int a = pthread_join(ptid, NULL);
  Serial.println("[WIFI][stop] - wifi stopped");

  return E_OK;
}

u8_t WifiDriver::getIsConnected() {
  return isConnected;
}

void* WifiDriver::run(void* args) {
  WifiDriver* self = static_cast<WifiDriver*>(args);
  while (self->isRunning) {
    switch (WiFi.status())
    {
      case !WL_CONNECTED:
        Serial.println("[Wifi] [run] - contecting....");
        WiFi.begin(ssid, password);
        delay(1000);
        break;

      case WL_CONNECTED:
        Serial.println(WiFi.localIP());
        self->isConnected = WIFI_CONNECTED;
        delay(1000);
        break;
      
      default:
        break;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  return nullptr;
}

ErrorCode WifiDriver::start() {
  Serial.println("[WIFI][start] - wifi start");
  isRunning = 1;
  WiFi.begin(ssid, password);
  int er = pthread_create(&ptid, NULL, &WifiDriver::run, this);
  
  return E_OK;
}