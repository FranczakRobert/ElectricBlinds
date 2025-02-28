#include "WifiDriver.hpp"
#include "WiFi.h"
#include "WifiConfig.hpp"
#include "DriverManager.hpp"
#include "NvmMemory.hpp"
#include "Pages.hpp"

#include <ArduinoJson.h>
#include <DNSServer.h>

WifiDriver::WifiDriver(DriverManager *driverManager)
{
  this->driverManager = driverManager;
  wifiStats.state = 0;
  isRunning = 1;
  TAG = "WIFI";
}

WifiDriver::~WifiDriver()
{
}

ErrorCode WifiDriver::init() {
  IPAddress local_IP(192, 168, 1, 184);
  IPAddress gateway(192, 168, 1, 1);

  IPAddress subnet(255, 255, 0, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  String ssid = NvmMemory::getInstance().readFromNvm("CREDENTIALS","SSID");
  String psswd = NvmMemory::getInstance().readFromNvm("CREDENTIALS","PSSWD");

  Serial.println(ssid);
  Serial.println(psswd);
  vTaskDelay(1000);

  WiFi.begin(
    ssid,
    psswd
  );

  return E_OK;
}

ErrorCode WifiDriver::deinit() {
  WiFi.disconnect();
 return E_OK;
}

ErrorCode WifiDriver::stop() {
  if(E_OK == stopThread(TAG)) {
    driverManager->setDriverData(D_LED,S_SET_WIFI_LED_OFF);
    return E_OK;
  }
  return E_NOT_OK;
}


DataSignalsResponse WifiDriver::getData(DataSignals SIGNAL)
{
  return (DataSignalsResponse)wifiStats.state;
}

ErrorCode WifiDriver::setData(DataSignals SIGNAL)
{
  return ErrorCode();
}

void* WifiDriver::run(void* args) {
  WifiDriver* self = static_cast<WifiDriver*>(args);
  uint8_t counter = 0;
  
  while (self->isRunning) {
    switch (WiFi.status())
    {
      case WL_DISCONNECTED:
        Serial.println("[Wifi] [run] - contecting....");
        counter++;
        if(counter >= WIFI_RETRIES_MAX) {
          WiFi._setStatus(WL_NO_SSID_AVAIL);
          counter = 0;
        }
        break;

      case WL_CONNECTED:
        Serial.println(WiFi.localIP());
        self->wifiStats.state = WIFI_CONNECTED;
        counter = 0;
        break;
      
      case WL_CONNECTION_LOST:
        self->wifiStats.state = WIFI_NOT_CONNECTED;
        WiFi.reconnect();
        break;
      
      case WL_NO_SSID_AVAIL:
        self->wifiStats.state = WIFI_CONFIG_MODE;
        self->getBlindsDataByAP();
        break;
      
      default:
        break;
    }
    delay(2000);
  }
  WiFi.disconnect();
  return nullptr;
}

ErrorCode WifiDriver::start() {
  return startThread(TAG,this,run);
}

WiFiServer server(80);
DNSServer dnsServer;

void WifiDriver::getBlindsDataByAP() {
  bool doIHaveData = false;
  
  WiFi.softAP("Home-Rolety", NULL);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  dnsServer.start(53, "*", IPAddress(192,168,4,1));
  server.begin();

  while (isRunning && !doIHaveData)
  {
    dnsServer.processNextRequest();
    WiFiClient client = server.available();
    if (client) {
      String header = "";
      String postData = "";
      bool isPost = false;
  
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          header += c;
          
          if (header.endsWith("\r\n\r\n")) {
            if (header.startsWith("POST")) {
              isPost = true;
            }
            break;
          }
        }
      }
      
      if (isPost) {
        while (client.available()) {
          char c = client.read();
          postData += c;
        }
        JsonDocument doc;
        deserializeJson(doc, postData);
        
        const char* ssid = doc["ssid"];
        const char* psswd = doc["password"];

        doIHaveData = true;
        WiFi.begin(String(ssid), String(psswd));
        uint8_t counter = 0;

        while (doIHaveData) {

          if (WiFi.status() == WL_CONNECTED){
            Serial.println(WiFi.localIP());
            wifiStats.status.isConnected = WIFI_CONNECTED;
            client.flush();
            client.stop();
            server.close();
            WiFi.softAPdisconnect();
            
            NvmMemory::getInstance().writeToNvm("CREDENTIALS","SSID",ssid);
            NvmMemory::getInstance().writeToNvm("CREDENTIALS","PSSWD",psswd);
            
            break;
          }

          if(WiFi.status() != WL_CONNECTED) {
            Serial.println("[Wifi][GetBlindsDataByAP] - contecting....");
              counter++;
              if(counter >= WIFI_RETRIES_MAX) {
                doIHaveData = false;
              }
              delay(2000);
          }
        }       
      }
    
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println("Connection: close");
      client.println();
      client.println(configPage);

      client.flush();
      client.stop();
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
