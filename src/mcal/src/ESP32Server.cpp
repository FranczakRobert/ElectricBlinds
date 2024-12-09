#include "ESP32Server.hpp"
#include <WebServer.h>
#include "ESP32Server.hpp"
#include <ArduinoJson.h>

WebServer ESP32Server::server(8080);
ESP32Server* ESP32Server::instance = nullptr;
TaskHandle_t xHandle = NULL;

ESP32Server::ESP32Server() {
  
  /* SINGLETON */
}

ESP32Server* ESP32Server::GetInstance() {
  if(instance == nullptr) {
    instance = new ESP32Server();
  }
  return instance;
}

void ESP32Server::handleRoot()
{
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 Control</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          text-align: center;
          background-color: #f4f4f9;
          color: #333;
          margin: 0;
          padding: 0;
          height: 100vh;
          display: flex;
          flex-direction: column;
          justify-content: center;
          align-items: center;
        }
        h1 {
          margin-bottom: 20px;
        }
        #toggleButton {
          padding: 15px 30px;
          font-size: 18px;
          color: #fff;
          background-color: #4CAF50; /* Zielony dla ON */
          border: none;
          border-radius: 5px;
          cursor: pointer;
          transition: background-color 0.3s, transform 0.2s;
        }
        #toggleButton.off {
          background-color: #f44336; /* Czerwony dla OFF */
        }
        #toggleButton:hover {
          transform: scale(1.05);
        }
      </style>
    </head>
    <body>
      <p> Hello </p>
    </body>
    </html>
    )rawliteral";

    ESP32Server::server.send(200, "text/html", html);
}

void ESP32Server::handlePost() {
  if (ESP32Server::server.hasArg("plain")) {
    JsonDocument doc;
    String body = ESP32Server::server.arg("plain");

    deserializeJson(doc, body);

    int val2 = doc["name"];
    int val = doc["val"];
    
    Serial.println(val);
    Serial.println(val2);
  }
  
  ESP32Server::server.send(200, "text/html", "<html><body><h1>GPIO zmienione</h1></body></html>");
}

ErrorCode ESP32Server::init() {
  
  ESP32Server::server.on("/", HTTP_GET, handleRoot);
  ESP32Server::server.on("/LED", HTTP_POST, handlePost);
  ESP32Server::server.begin(8080);

  return E_OK;
}

ErrorCode ESP32Server::deinit() {
  ESP32Server::server.stop();
  return E_OK;
}

ErrorCode ESP32Server::start(){

  init();
  vTaskDelay(100);
  xTaskCreate(runWrapper, "Server", 2048, NULL, 1, &xHandle);

  return E_OK;
}

ErrorCode ESP32Server::stop() {
  vTaskDelete(xHandle);
  deinit();
  return E_OK;
}

void ESP32Server::runWrapper(void *params) {
  ESP32Server::GetInstance()->run();
}

void ESP32Server::run() {
  while(1) {
    ESP32Server::server.handleClient();
    vTaskDelay(1);
  }
}
