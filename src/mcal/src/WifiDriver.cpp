#include "WifiDriver.hpp"
#include "WiFi.h"
#include "WifiConfig.hpp"
#include "DriverManager.hpp"
#include "NvmMemory.hpp"
#include "Pages.hpp"
#include "config.hpp"

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

  vTaskDelay(1000);

  if(TRIGGER_WIFI_SEARCH) {
    WiFi.begin(
      "ssid",
      "psswd"
    );
  }
  else {
    WiFi.begin(
      ssid,
      psswd
    );
  }

  return E_OK;
}

ErrorCode WifiDriver::deinit() {
  WiFi.disconnect();
 return E_OK;
}

ErrorCode WifiDriver::stop() {
  if(E_OK == stopThread(TAG)) {
    driverManager->setDriverData(D_DISPLAY,S_OLED_WIFI_DISCONNECTED);
    return E_OK;
  }
  return E_NOT_OK;
}

DataSignalsResponse WifiDriver::getData(DataSignals SIGNAL)
{
  return (DataSignalsResponse)wifiStats.state;
}

ErrorCode WifiDriver::setData( DataSignals SIGNAL, uint16_t count, ...)
{
  switch (SIGNAL)
  {
  case S_TRIGGER_RESET:
      Serial.println("[Wifi] [setData] - RESETING");
      pthread_mutex_lock(&mutex);
      wifiStats.state = WIFI_CONFIG_MODE;
      pthread_mutex_unlock(&mutex);
      WiFi.disconnect();
    break;
  
  default:
    break;
  }
  
  return ErrorCode();
}

void* WifiDriver::run(void* args) {
  WifiDriver* self = static_cast<WifiDriver*>(args);
  uint8_t counter = 1;
  int8_t previousVal = -1;
  bool wasEverConnected = false;
  
  while (self->isRunning) {
    delay(2000);
    switch (WiFi.status())
    {
      case WL_DISCONNECTED:
        Serial.println("[Wifi] [run] - contecting....");
        counter++;
        if(counter >= WIFI_RETRIES_MAX) {
          WiFi._setStatus(WL_NO_SSID_AVAIL);
          counter = 0;
        }
        if(previousVal != WL_DISCONNECTED) {
          self->driverManager->setDriverData(D_DISPLAY,S_OLED_WIFI_DISCONNECTED);
          previousVal = WL_DISCONNECTED;
        }
        break;

      case WL_CONNECTED:
        
        if(WIFI_IP_SHOW) {
          Serial.println(WiFi.localIP());  
        }
        self->wifiStats.state = WIFI_CONNECTED;
        if(counter != 0) {
          counter = 0;
        }
        
        if(previousVal != WL_CONNECTED) {
          self->driverManager->setDriverData(D_DISPLAY,S_OLED_SYSTEM_FULL_ACTIVE);
          if(!wasEverConnected) {
            self->driverManager->setDriverData(D_SCHEDULER,S_SCHEDULER_FETCH_DATA);
          }
          previousVal = WL_CONNECTED;
        }
        wasEverConnected = true;
        break;
      
      case WL_CONNECTION_LOST:
        if(previousVal != WL_CONNECTION_LOST) {
          self->driverManager->setDriverData(D_DISPLAY,S_OLED_WIFI_DISCONNECTED);
          WiFi.reconnect();
          previousVal = WL_CONNECTION_LOST;
        }
        break;
      
      case WL_NO_SSID_AVAIL:
        self->wifiStats.state = WIFI_CONFIG_MODE;
        if(previousVal != WL_NO_SSID_AVAIL) {
          self->driverManager->setDriverData(D_DISPLAY,S_OLED_SYSTEM_CONFIG_MODE);
          previousVal = WL_NO_SSID_AVAIL;
        }
        self->getBlindsDataByAP();
        
        break;
      
      default:
        break;
    }
   
  }
  WiFi.disconnect();
  return nullptr;
}

ErrorCode WifiDriver::start() {
  return startThread(TAG,this,run,true);
}

WiFiServer server(80);
DNSServer dnsServer;

ErrorCode WifiDriver::getBlindsDataByAP() {
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

        u32_t max = doc["max"];

        doIHaveData = true;
        WiFi.begin(String(ssid), String(psswd));
        uint8_t counter = 0;

        while (doIHaveData) {

          if (WiFi.status() == WL_CONNECTED){
            if(WIFI_IP_SHOW) {
              Serial.println(WiFi.localIP());  
            }
            
            wifiStats.state = WIFI_CONNECTED;
            client.flush();
            client.stop();
            server.close();
            WiFi.softAPdisconnect();

            driverManager->setDriverData(D_NEMA17,S_SET_NEMA_MAX,1,max);

            NvmMemory::getInstance().writeToNvm("CREDENTIALS","SSID",ssid);
            NvmMemory::getInstance().writeToNvm("CREDENTIALS","PSSWD",psswd);
            driverManager->setDriverData(D_DISPLAY,S_OLED_SYSTEM_FULL_ACTIVE);
            break;
          }

          if(WiFi.status() != WL_CONNECTED) {
            Serial.println("[Wifi][GetBlindsDataByAP] - contecting....");
            driverManager->setDriverData(D_DISPLAY,S_OLED_SYSTEM_BOOT_STATE);
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
  return E_OK;
}
