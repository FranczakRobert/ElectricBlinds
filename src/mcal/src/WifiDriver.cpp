#include "WifiDriver.hpp"
#include "WiFi.h"
#include "WifiConfig.hpp"
#include "DriverManager.hpp"

#include <ArduinoJson.h>
#include <DNSServer.h>

WifiDriver::WifiDriver(DriverManager *driverManager)
{
  this->driverManager = driverManager;
  wifiStats.state = 0;
  isRunning = 1;
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

  WiFi.begin(ssid, password);
  return E_OK;
}

ErrorCode WifiDriver::deinit() {
  WiFi.disconnect();
 return E_OK;
}

ErrorCode WifiDriver::stop() {
  if(E_OK == stopThread()) {
    driverManager->setWifiLedOff();
    Serial.println("[WIFI][stop] - OK");

    return E_OK;
  }
  Serial.println("[WIFI][stop] - ERROR");

  return E_NOT_OK;
}

WifiStats WifiDriver::getWifiStats() {
  return wifiStats;
}

void* WifiDriver::run(void* args) {
  WifiDriver* self = static_cast<WifiDriver*>(args);
  int counter = 0;
  WifiDataS wifiCred;
  while (self->isRunning) {
    switch (WiFi.status())
    {
      case WL_IDLE_STATUS:
        Serial.println("[Wifi] [run] - contecting....");
        Serial.println(counter);
        Serial.println(WiFi.status());
        counter++;
        if(counter >= 4) {
          WiFi.disconnect();
          WiFi._setStatus(WL_DISCONNECTED);
          counter = 0;
        }
        delay(2000);
        break;

      case WL_CONNECTED:
        Serial.println(WiFi.localIP());
        self->wifiStats.status.isConnected = WIFI_CONNECTED;
        delay(2000);
        counter = 0;
        break;
      
      case WL_CONNECTION_LOST:
        self->wifiStats.status.isConnected = 0;
        delay(1000);
        break;
      
      case WL_DISCONNECTED:
        wifiCred = self->GetBlindsDataByAP();
        // WiFi.begin(wifiCred.ssid, wifiCred.psswd);
        delay(1000);
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
  if(E_OK == startThread(this,run),true) {
    Serial.println("[WIFI][start] - OK");
    return E_OK;
  }
  Serial.println("[WIFI][start] - ERROR");
  return E_NOT_OK;
}

WiFiServer server(80);
DNSServer dnsServer;

WifiDataS WifiDriver::GetBlindsDataByAP() {
  const char* ssid     = "Home-Rolety";
  const char* password = "12345678";
  bool doIHaveData = false;
  
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  dnsServer.start(53, "*", IPAddress(192,168,4,1));

  IPAddress IP = WiFi.softAPIP();
  server.begin();
  String header;

  while (isRunning && !doIHaveData)
  {
    dnsServer.processNextRequest();
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Nowy klient podłączony.");
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
        wifiData.psswd = String(psswd);
        wifiData.ssid = String(ssid);

        WiFi.begin(wifiData.ssid, wifiData.psswd);
        bool tryagain = false;
        int counter = 0;

        while (!tryagain) {

          if (WiFi.status() == WL_CONNECTED){
            Serial.println(WiFi.localIP());
            wifiStats.status.isConnected = WIFI_CONNECTED;
            client.stop();
            server.close();
            WiFi.softAPdisconnect();
            
            return wifiData;
          }

          if(WiFi.status() != WL_CONNECTED) {
            Serial.println("[Wifi][GetBlindsDataByAP] - contecting....");
              counter++;
              if(counter >= 4) {
                doIHaveData = false;
                tryagain = true;
              }
              delay(2000);
          }
        }       
      }
    
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println("Connection: close");
      client.println();
      client.println("<!DOCTYPE html>");
      client.println("<html><head>");
      client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
      client.println("<style> body { text-align: center; font-family: Arial; } ");
      client.println(" input, button { font-size: 18px; padding: 10px; margin: 5px; width: 80%; }");
      client.println(" label { display: block; margin-bottom: 10px; } ");
      client.println(" form { margin: 0 auto; width: 300px; } ");
      client.println("</style>");
      client.println("</head><body>");
      client.println("<h1>Konfiguracja Rolety</h1>");
      client.println("<form id='wifiForm'>");
      client.println("<label for='ssid'>Nazwa sieci WiFi:</label>");
      client.println("<input type='text' id='ssid' name='ssid'><br>");
      client.println("<label for='password'>Haslo:</label>");
      client.println("<input type='password' id='password' name='password'><br>");
      client.println("<button type='button' onclick='sendData()'>ZAPISZ</button>");
      client.println("</form>");
      client.println("<script>");
      client.println("function sendData() {");
      client.println("  var ssid = document.getElementById('ssid').value;");
      client.println("  var password = document.getElementById('password').value;");
      client.println("  var jsonData = JSON.stringify({ssid: ssid, password: password});");
      client.println("  var xhr = new XMLHttpRequest();");
      client.println("  xhr.open('POST', '/submit', true);");
      client.println("  xhr.setRequestHeader('Content-Type', 'application/json');");
      client.println("  xhr.send(jsonData);");
      client.println("  alert('Dane zapisane!');");
      client.println("  document.getElementById('ssid').value = '';");
      client.println("  document.getElementById('password').value = '';");
      client.println("}");
      client.println("</script>");
      client.println("</body></html>");

      client.stop();
      Serial.println("Połączenie zamknięte.");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  return wifiData;
}
