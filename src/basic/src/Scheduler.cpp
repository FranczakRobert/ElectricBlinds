#include "Scheduler.hpp"
#include "DriverManager.hpp"
#include <HTTPClient.h>
#include <ArduinoJson.h>

void *Scheduler::run(void *args)
{
  Scheduler* self = static_cast<Scheduler*>(args);
  bool startCounting = false;
  uint16_t seconds = 0;

  while (self->isRunning) {
    pthread_mutex_lock(&self->mutex);
    if(self->fetchData) {
      self->fetchData = false;
      pthread_mutex_unlock(&self->mutex);

      self->fetchHour();
      startCounting = true;
      self->time[MIN] = 59;
      self->time[HOUR] = 23;
    }
    else{
      pthread_mutex_unlock(&self->mutex);
    }

    if(startCounting) {
      seconds = (seconds + 5) % 60;
      if(0 == seconds) {
        self->time[MIN] = (self->time[MIN] + 1) % 60;
        if(self->time[MIN] == 0) {
          self->time[HOUR] = (self->time[HOUR] + 1) % 24;
        }
      }
      // Serial.print(self->time[HOUR]);
      // Serial.print(" : ");
      // Serial.print(self->time[MIN]);
      // Serial.print(" : ");
      // Serial.print(seconds);
      // Serial.println(" sec");
    }

    vTaskDelay(30000);
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
      JsonDocument doc;
      deserializeJson(doc, payload);
      
      // pthread_mutex_lock(&mutex);
      time[HOUR] = doc["hour"];
      time[MIN] = doc["minute"];
      // pthread_mutex_unlock(&mutex);      
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
