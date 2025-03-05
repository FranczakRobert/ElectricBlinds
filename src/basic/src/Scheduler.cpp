#include "Scheduler.hpp"
#include "DriverManager.hpp"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// TODO Fix Stack overflow in this thread
void *Scheduler::run(void *args)
{
  Scheduler* self = static_cast<Scheduler*>(args);
  while (self->isRunning) {
    pthread_mutex_lock(&self->mutex);
    if(self->fetchData) {
      self->fetchData = false;
      pthread_mutex_unlock(&self->mutex);

      self->fetchHour();
    }
    else{
      pthread_mutex_unlock(&self->mutex);
    }
    
    vTaskDelay(500);
  }
  
  return nullptr;
}

Scheduler::Scheduler(DriverManager *driverManager)
{
  this->driverManager = driverManager;
  TAG = "Scheduler";
}

Scheduler::~Scheduler()
{
}

void Scheduler::setFetchData()
{
  
}

ErrorCode Scheduler::init()
{
  fetchData = false;
  
  return ErrorCode();
}

ErrorCode Scheduler::deinit()
{
    return ErrorCode();
}

ErrorCode Scheduler::start()
{
  startThread(TAG,this,run,true);
  return ErrorCode();
}

ErrorCode Scheduler::stop()
{
  stopThread(TAG);
  return ErrorCode();
}

DataSignalsResponse Scheduler::getData(DataSignals)
{
    return DataSignalsResponse();
}

ErrorCode Scheduler::setData(DataSignals SIGNAL, uint16_t count, ...)
{
  switch (SIGNAL)
  {
  case S_FETCH_DATA:
    pthread_mutex_lock(&mutex);
    fetchData = true;    
    pthread_mutex_unlock(&mutex);
    break;
  
  default:
    break;
  }
    return ErrorCode();
}

void Scheduler::fetchHour() {
  Serial.println("Scheduler fetching data...");

    HTTPClient http;
    String queryString = "https://www.timeapi.io/api/time/current/zone?timeZone=Europe\%2FWarsaw";
    http.begin(queryString.c_str());
    int httpResponseCode = http.GET();
        
    if (httpResponseCode>0) {
      String payload = http.getString();
      Serial.println("[Scheduler] [fetchHour] - HAPPENING");
      Serial.println(payload);
      JsonDocument doc;
      deserializeJson(doc, payload);
  
      int hour = doc["hour"];
      int min  = doc["minute"];
  
      driverManager->setDriverData(D_NEMA17,S_SET_NEMA_WORKING_TIME,2,hour,min);
      
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
