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
String ESP32Server::max = "";
pthread_mutex_t ESP32Server::timeValMutex = PTHREAD_MUTEX_INITIALIZER;


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

void ESP32Server::handleBlindsTimerPost() {
  if (ESP32Server::server.hasArg("plain")) {
    JsonDocument doc;
    String body = ESP32Server::server.arg("plain");

    deserializeJson(doc, body);

    const char* loweringTime = doc["loweringTime"];
    const char* raisingTime = doc["raisingTime"];

    pthread_mutex_lock(&timeValMutex);
    NvmMemory::getInstance().writeToNvm("TIME", "L", loweringTime);
    NvmMemory::getInstance().writeToNvm("TIME", "R", raisingTime);
    pthread_mutex_unlock(&timeValMutex);
    
    ESP32Server::GetInstance().driverManager->setDriverData(
      D_SCHEDULER,
      S_SCHEDULER_SET_NEMA_LOW_TIME,1,loweringTime);

    ESP32Server::GetInstance().driverManager->setDriverData(
      D_SCHEDULER,
      S_SCHEDULER_SET_NEMA_UP_TIME,1,raisingTime);

  }
}

void ESP32Server::setRandLTimers() {

  if (&ESP32Server::server == nullptr) {
    Serial.println("ERROR: Server is not initialized.");
    return;
  }
  pthread_mutex_lock(&timeValMutex);
  //TODO tu mona by wywaluc chyba te zmienne ?
  GetInstance().loweringTimeVal = NvmMemory::getInstance().readFromNvm("TIME", "L");
  GetInstance().raisingTimeVal = NvmMemory::getInstance().readFromNvm("TIME", "R");
  pthread_mutex_unlock(&timeValMutex);
  
    JsonDocument doc;
    doc["loweringTime"] = ESP32Server::GetInstance().loweringTimeVal.isEmpty() ? "00:00" : ESP32Server::GetInstance().loweringTimeVal;
    doc["raisingTime"] = ESP32Server::GetInstance().raisingTimeVal.isEmpty() ? "00:00" : ESP32Server::GetInstance().raisingTimeVal;
    doc["max"] = ESP32Server::GetInstance().max.isEmpty() ? "300" : ESP32Server::GetInstance().max;

    String response;
    serializeJson(doc, response);

    ESP32Server::GetInstance().server.send(200, "application/json", response);
}


void ESP32Server::handleReset()
{
  ESP32Server::GetInstance().driverManager->setDriverData(D_WIFI,S_TRIGGER_RESET);
}

ErrorCode ESP32Server::init() {
  ESP32Server::server.on("/", HTTP_GET, handleRoot);
  ESP32Server::server.on("/timeUpdate", HTTP_GET, setRandLTimers);
  ESP32Server::server.on("/motorControll", HTTP_POST, handleMotorPost);
  ESP32Server::server.on("/blindsTime", HTTP_POST, handleBlindsTimerPost);
  ESP32Server::server.on("/reset", HTTP_GET, handleReset);
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

ErrorCode ESP32Server::setData(DataSignals SIGNAL, uint16_t count, ...)
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
