#include <WebServer.h>
#include "ESP32Server.hpp"
#include <ArduinoJson.h>
#include "NvmMemory.hpp"
#include "freertos/semphr.h"
#include <ESPmDNS.h>
#include "Pages.hpp"

#include <Preferences.h>
class Preferences;

struct payload {
  String loweringTime;
  String raisingTime;
};

WebServer ESP32Server::server(8080);
String ESP32Server::loweringTimeVal = "";
String ESP32Server::raisingTimeVal = "";
SemaphoreHandle_t nvmSemaphore;

ESP32Server::ESP32Server() {
  TAG = "SERVER";
  /* SINGLETON */
}

void ESP32Server::handleRoot()
{
  ESP32Server::server.send(200, "text/html", mainPage);
}


void ESP32Server::handleMotorPost() {
  if (ESP32Server::server.hasArg("plain")) {
    JsonDocument doc;
    String body = ESP32Server::server.arg("plain");

    deserializeJson(doc, body);
    struct MotorStatus motorStatus;

    const char* name = doc["name"];
    uint8_t val = doc["val"];

    strcmp(name, "ArrowUp") == 0 ? 
      ESP32Server::GetInstance().driverManager->setDriverData(D_NEMA17,S_SET_NEMA_UP_STATUS)
    : ESP32Server::GetInstance().driverManager->setDriverData(D_NEMA17,S_SET_NEMA_DOWN_STATUS);

    val == 1 ? 
      ESP32Server::GetInstance().driverManager->setDriverData(D_NEMA17,S_SET_NEMA_HOLD_STATUS)
    : ESP32Server::GetInstance().driverManager->setDriverData(D_NEMA17,S_SET_NEMA_RELEASE_STATUS);

  }
  
  ESP32Server::server.send(200);
}


void writeToNVMTask(void* pvParameters) {
  
  struct payload payloadCopy = *(static_cast<struct payload*>(pvParameters));

  const char* loweringTimeStr = payloadCopy.loweringTime.c_str();
  const char* raisingTimeStr = payloadCopy.raisingTime.c_str();

  NvmMemory::getInstance().writeToNvm("TIME", "L", loweringTimeStr);
  NvmMemory::getInstance().writeToNvm("TIME", "R", raisingTimeStr);

  vTaskDelay(100 / portTICK_PERIOD_MS);
  vTaskDelete(NULL);
}

void ESP32Server::handleBlindsTimerPost() {
  if (ESP32Server::server.hasArg("plain")) {
    JsonDocument doc;
    String body = ESP32Server::server.arg("plain");

    deserializeJson(doc, body);

    const char* loweringTime = doc["loweringTime"];
    const char* raisingTime = doc["raisingTime"];

    static struct payload taskPayload;
    taskPayload.loweringTime = String(loweringTime);
    taskPayload.raisingTime = String(raisingTime);

    xTaskCreate(writeToNVMTask, "WriteToNVMTask", 2048, (void*)&taskPayload, 1, NULL);
  }
}

void readFromNVMTask(void* pvParameters) {
  ESP32Server* server = static_cast<ESP32Server*>(pvParameters);

  server->GetInstance().loweringTimeVal = NvmMemory::getInstance().readFromNvm("TIME", "L");
  server->GetInstance().raisingTimeVal = NvmMemory::getInstance().readFromNvm("TIME", "R");

  xSemaphoreGive(nvmSemaphore);

  vTaskDelete(NULL);
}

void ESP32Server::setRandLTimers() {
  nvmSemaphore = xSemaphoreCreateBinary();

  if (&ESP32Server::server == nullptr) {
    Serial.println("ERROR: Server is not initialized.");
    return;
  }

  xTaskCreate(readFromNVMTask, "ReadFromNVMTask", 2048, (void*)&ESP32Server::GetInstance(), 1, NULL);

  if(pdTRUE == xSemaphoreTake(nvmSemaphore, portMAX_DELAY)) {
    JsonDocument doc;
    doc["loweringTime"] = ESP32Server::GetInstance().loweringTimeVal.isEmpty() ? "00:00" : ESP32Server::GetInstance().loweringTimeVal;
    doc["raisingTime"] = ESP32Server::GetInstance().raisingTimeVal.isEmpty() ? "00:00" : ESP32Server::GetInstance().raisingTimeVal;

    String response;
    serializeJson(doc, response);

    ESP32Server::GetInstance().server.send(200, "application/json", response);
  }

  vSemaphoreDelete(nvmSemaphore);
}


ErrorCode ESP32Server::init() {
  ESP32Server::server.on("/", HTTP_GET, handleRoot);
  ESP32Server::server.on("/timeUpdate", HTTP_GET, setRandLTimers);
  ESP32Server::server.on("/motorControll", HTTP_POST, handleMotorPost);
  ESP32Server::server.on("/blindsTime", HTTP_POST, handleBlindsTimerPost);
  ESP32Server::server.begin(8080);
  return E_OK;
}

ErrorCode ESP32Server::deinit() {
  
  return E_OK;
}

ErrorCode ESP32Server::start(){

  if(E_OK == init()) {
    return startThread(TAG,this,run,true);
  }
  return E_NOT_OK;
}

ErrorCode ESP32Server::stop() {
  ESP32Server::server.stop();
  return stopThread(TAG);
}

DataSignalsResponse ESP32Server::getData(DataSignals SIGNAL)
{
    return DataSignalsResponse();
}

ErrorCode ESP32Server::setData(DataSignals)
{
    return ErrorCode();
}

ErrorCode ESP32Server::setManager(DriverManager *drMg) {
  ESP32Server::driverManager = drMg;
  return ErrorCode();
}

void* ESP32Server::run(void* args) {
  while(ESP32Server::GetInstance().isRunning) {
    ESP32Server::server.handleClient();    
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  return nullptr;
}
