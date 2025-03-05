#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "Arduino.h"
#include "Thread.hpp"
#include "Driver.hpp"
#include "ErrorCode.hpp"

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
  DriverManager* driverManager;

  void fetchHour();
};


#endif //SCHEDULER_HPP