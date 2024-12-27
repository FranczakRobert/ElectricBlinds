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
  isRunning = 0;
  if(0 == pthread_join(ptid, NULL)) {
    Serial.println("[WIFI][stop] - OK");
    driverManager->setWifiLedOff();
    return E_OK;
  }
  return E_NOT_OK;
}

u8_t WifiDriver::getIsConnected() {
  return isConnected;
}

void* WifiDriver::run(void* args) {
  WifiDriver* self = static_cast<WifiDriver*>(args);
  WiFi.begin(ssid, password);

  while (self->isRunning) {
    switch (WiFi.status())
    {
      case !WL_CONNECTED:
        Serial.println("[Wifi] [run] - contecting....");
        WiFi.begin(ssid, password);
        delay(3000);
        break;

      case WL_CONNECTED:
        Serial.println(WiFi.localIP());
        self->isConnected = WIFI_CONNECTED;
        delay(5000);
        break;
      
      default:
        break;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  WiFi.disconnect();
  return nullptr;
}

ErrorCode WifiDriver::start() {
  isRunning = 1;
  if(0 == pthread_create(&ptid, NULL, &WifiDriver::run, this)) {
    Serial.println("[WIFI][start] - OK");
    return E_OK;
  }
  Serial.println("[WIFI][start] - ERROR");

  return E_NOT_OK;
}