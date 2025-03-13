#include "Scheduler.hpp"
#include "DriverManager.hpp"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "NvmMemory.hpp"

void *Scheduler::run(void *args)
{
  Scheduler* self = static_cast<Scheduler*>(args);
  bool startCounting = false;
  uint16_t seconds = 0;
  bool stopper = false;;

  while (self->isRunning) {
    pthread_mutex_lock(&self->mutex);
    if(self->fetchData) {
      self->fetchData = false;
      pthread_mutex_unlock(&self->mutex);

      self->fetchHour();
      startCounting = true;
      self->clock[MIN] = 59;
      self->clock[HOUR] = 23;
    }
    else{
      pthread_mutex_unlock(&self->mutex);
    }

    if(startCounting) {
      seconds = (seconds + 5) % 60;
      if(0 == seconds) {
        self->clock[MIN] = (self->clock[MIN] + 1) % 60;
        stopper = false;
        if(self->clock[MIN] == 0) {
          self->clock[HOUR] = (self->clock[HOUR] + 1) % 24;
        }
      }
      vTaskDelay(3000);

      pthread_mutex_lock(&self->mutex);

      if(!stopper) {
        if(self->NEMAWorkingTime[NEMA_RAISING][HOUR] == self->clock[HOUR] &&
          self->NEMAWorkingTime[NEMA_RAISING][MIN] == self->clock[MIN]) {
            self->driverManager->setDriverData(D_NEMA17,S_TRIGGER_NEM_RAISING);
        }
  
        if(self->NEMAWorkingTime[NEMA_LOWERING][HOUR] == self->clock[HOUR] &&
          self->NEMAWorkingTime[NEMA_LOWERING][MIN] == self->clock[MIN]) {
            self->driverManager->setDriverData(D_NEMA17,S_TRIGGER_NEMA_LOWERING);
        }
        stopper = true;
      }

      

      pthread_mutex_unlock(&self->mutex);
    }

    vTaskDelay(200);
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
  String loweringTime = NvmMemory::getInstance().readFromNvm("TIME", "L");
  String raisingTime = NvmMemory::getInstance().readFromNvm("TIME", "R");
  
  setTimeClock(NEMAWorkingTime[NEMA_RAISING],raisingTime);
  setTimeClock(NEMAWorkingTime[NEMA_LOWERING],loweringTime);
  
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

void Scheduler::setTimeClock(volatile uint32_t* result, String time) {
  String tmp ="";
  
  for (int i = 0; i < time.length(); i++)
  {
    if(time[i] != ':') {
      tmp += time[i];
    }
    else {
      result[HOUR] = atoi(tmp.c_str());
      tmp = "";
    }
  }
  result[MIN] = atoi(tmp.c_str());
}

ErrorCode Scheduler::setData(DataSignals SIGNAL, uint16_t count, ...)
{
  switch (SIGNAL)
  {
  case S_SCHEDULER_FETCH_DATA:
    pthread_mutex_lock(&mutex);
    fetchData = true;    
    pthread_mutex_unlock(&mutex);
    break;

  case S_SCHEDULER_SET_NEMA_LOW_TIME:
    if (count > 0) {
      va_list args;
      va_start(args, count);
      pthread_mutex_lock(&mutex);
      setTimeClock(NEMAWorkingTime[NEMA_LOWERING], String(va_arg(args, const char*)));
      pthread_mutex_unlock(&mutex);
      
    break;

  case S_SCHEDULER_SET_NEMA_UP_TIME:
    if (count > 0) {
      va_list args;
      va_start(args, count);
      pthread_mutex_lock(&mutex);
      setTimeClock(NEMAWorkingTime[NEMA_RAISING], String(va_arg(args, const char*)));
      pthread_mutex_unlock(&mutex);
    }
    break;

  default:
    break;
  }
}
    return ErrorCode();
}

// TODO Ustabilizować
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
      
      pthread_mutex_lock(&mutex);
      clock[HOUR] = doc["hour"];
      clock[MIN] = doc["minute"];
      pthread_mutex_unlock(&mutex);      
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
