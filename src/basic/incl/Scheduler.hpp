#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "Arduino.h"
#include "Thread.hpp"
#include "Driver.hpp"
#include "ErrorCode.hpp"
#define HOUR 0
#define MIN 1

#define NEMA_RAISING 0
#define NEMA_LOWERING 1

#define LOWER 0
#define UP 1
#define SECOND_GP 1

class DriverManager;

class Scheduler : public Driver ,public Thread {

  static void* run(void* args);

  public:
  Scheduler(DriverManager* driverManager);
  ~Scheduler();

  ErrorCode init();
  ErrorCode deinit();
  ErrorCode start();
  ErrorCode stop();

  DataSignalsResponse getData(DataSignals);
  ErrorCode setData(DataSignals SIGNAL, uint16_t count, ...);

  private:
  bool fetchData;
  uint32_t clock[2];

  uint32_t NEMAWorkingTime[2][2];

  DriverManager* driverManager;

  ErrorCode fetchHour();
  void setTimeClock(uint32_t* result, String time);
};


#endif //SCHEDULER_HPP