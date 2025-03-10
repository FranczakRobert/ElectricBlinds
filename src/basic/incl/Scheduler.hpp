#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "Arduino.h"
#include "Thread.hpp"
#include "Driver.hpp"
#include "ErrorCode.hpp"
#define HOUR 0
#define MIN 1

class DriverManager;

class Scheduler : public Driver ,public Thread {

  static void* run(void* args);

  public:
  Scheduler(DriverManager* driverManager);
  ~Scheduler();

  void setFetchData();

  ErrorCode init();
  ErrorCode deinit();
  ErrorCode start();
  ErrorCode stop();

  DataSignalsResponse getData(DataSignals);
  ErrorCode setData(DataSignals SIGNAL, uint16_t count, ...);

  private:
  volatile bool fetchData;
  uint32_t time[3];
  DriverManager* driverManager;

  void fetchHour();
};


#endif //SCHEDULER_HPP