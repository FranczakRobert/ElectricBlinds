#include "WifiDriver.hpp"
#include "WiFi.h"
#include "WifiConfig.hpp"
#include "DriverManager.hpp"

WifiDriver::WifiDriver(DriverManager *driverManager) {
  this->driverManager = driverManager;
  isConnected = WIFI_NOT_CONNECTED;
}

WifiDriver::~WifiDriver()
{
}

ErrorCode WifiDriver::init() {
  WiFi.begin(ssid, password);
  delay(5000);
  return E_OK;
}

ErrorCode WifiDriver::deinit() {
  ErrorCode errorCode = E_NOT_OK;
  wl_status_t wifiStatus = WiFi.status();

  if(WL_CONNECTED == wifiStatus) {
    WiFi.disconnect(true);
  } else {
    return errorCode;
  }

  errorCode = WiFi.mode(WIFI_OFF) ? E_OK : E_NOT_OK;
  
  return errorCode;
}

ErrorCode WifiDriver::stop() {
  WiFi.disconnect();
  return E_OK;
}

u8_t WifiDriver::getIsConnected() {
  return isConnected;
}

ErrorCode WifiDriver::start() {
  Serial.print("[Wifi] - started: ");
  Serial.println(WiFi.localIP());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  isConnected = WIFI_CONNECTED;

  // Serial.println("----------------");
  // Serial.println("WiFi connected!");
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
  // Serial.println("----------------");
  return E_OK;
}