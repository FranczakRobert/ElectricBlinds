#include "WifiDriver.hpp"
#include "WiFi.h"
#include "WifiConfig.hpp"

WifiDriver::WifiDriver() {

}

WifiDriver::~WifiDriver() {
}

ErrorCode WifiDriver::init() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  delay(5000);
  return E_OK;
}

ErrorCode WifiDriver::deinit() {
  ErrorCode errorCode = E_NOT_OK;
  wl_status_t wifiStatus = WiFi.status();

  if(WL_CONNECTED == wifiStatus) {
    WiFi.disconnect(true);
  }
  else {
    return errorCode;
  }

  errorCode = WiFi.mode(WIFI_OFF) ? E_OK : E_NOT_OK;
  
  return errorCode;
}

ErrorCode WifiDriver::stop() {
  return deinit();
}

ErrorCode WifiDriver::start() {
  init();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("----------------");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("----------------");
  return E_OK;
}