#include "Scheduler.hpp"
#include "DriverManager.hpp"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "NvmMemory.hpp"
#include "config.hpp"

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

      if(self->fetchHour())
      {
        self->fetchData = true;
        Serial.println("[Scheduler] [run] - request API");
        vTaskDelay(2000);
        continue;
      }
      startCounting = true;
    }
    else{
      pthread_mutex_unlock(&self->mutex);
    }

    if(startCounting) {
      seconds = (seconds + SECOND_GP) % 60;
      if(0 == seconds) {
        self->clock[MIN] = (self->clock[MIN] + 1) % 60;
        stopper = false;
        if(self->clock[MIN] == 0) {
          self->clock[HOUR] = (self->clock[HOUR] + 1) % 24;
        }
      }
      pthread_mutex_lock(&self->mutex);

      if(!stopper) {
        bool hours = self->NEMAWorkingTime[NEMA_RAISING][HOUR] == self->NEMAWorkingTime[NEMA_LOWERING][HOUR];
        bool minutes = self->NEMAWorkingTime[NEMA_RAISING][MIN] == self->NEMAWorkingTime[NEMA_LOWERING][MIN];

        if((hours & minutes) == false)
        {
            if(self->NEMAWorkingTime[NEMA_RAISING][HOUR] == self->clock[HOUR] &&
              self->NEMAWorkingTime[NEMA_RAISING][MIN] == self->clock[MIN]) {
                self->driverManager->setDriverData(D_NEMA17,S_TRIGGER_NEM_RAISING);
                stopper = true;
            }
      
            if(self->NEMAWorkingTime[NEMA_LOWERING][HOUR] == self->clock[HOUR] &&
              self->NEMAWorkingTime[NEMA_LOWERING][MIN] == self->clock[MIN]) {
                self->driverManager->setDriverData(D_NEMA17,S_TRIGGER_NEMA_LOWERING);
                stopper = true;
            }
        }
      }

      pthread_mutex_unlock(&self->mutex);
      if(CLOCK_SHOW) {
        Serial.print(self->clock[HOUR]);
        Serial.print(" : ");
        Serial.println(self->clock[MIN]);
      }
      
    }

    vTaskDelay(SECOND_GP * 1000);
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

ErrorCode Scheduler::init()
{
  fetchData = false;
  String loweringTime = NvmMemory::getInstance().readFromNvm("TIME", "L");
  String raisingTime = NvmMemory::getInstance().readFromNvm("TIME", "R");
  
  setTimeClock(NEMAWorkingTime[NEMA_RAISING],raisingTime);
  setTimeClock(NEMAWorkingTime[NEMA_LOWERING],loweringTime);
  
  return E_OK;
}

ErrorCode Scheduler::deinit()
{
  return ErrorCode();
}

ErrorCode Scheduler::start()
{
  return startThread(TAG,this,run,true);
}

ErrorCode Scheduler::stop()
{
  return stopThread(TAG);
}

DataSignalsResponse Scheduler::getData(DataSignals)
{
  return DataSignalsResponse();
}

void Scheduler::setTimeClock(uint32_t* result, String time) {
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
      }
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
  return E_OK;
}

ErrorCode Scheduler::fetchHour() 
{
  HTTPClient http;
  String queryString = "https://www.timeapi.io/api/time/current/zone?timeZone=Europe\%2FWarsaw";
  http.begin(queryString.c_str());
  int httpResponseCode = http.GET();
      
  if (httpResponseCode>0) {
    String payload = http.getString();
    JsonDocument doc;
    deserializeJson(doc, payload);
    
    pthread_mutex_lock(&mutex);
    clock[HOUR] = doc["hour"];
    clock[MIN] = doc["minute"];
    pthread_mutex_unlock(&mutex);
    
  }
  else {
    driverManager->setDriverData(D_DISPLAY,S_OLED_SYSTEM_BOOT_STATE);
    return E_NOT_OK;
  }
  
  http.end();
  driverManager->setDriverData(D_DISPLAY,S_OLED_SYSTEM_FULL_ACTIVE);
  return E_OK;
  }
