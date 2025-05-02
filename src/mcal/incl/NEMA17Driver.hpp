#ifndef MOTOR_L298N_DRIVER_HPP
#define MOTOR_L298N_DRIVER_HPP

#include "Driver.hpp"
#include <Stepper.h>
#include "MotorStatus.hpp" 
#include "Thread.hpp"
#include "A4988.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define HOLD       1
#define RELEASE    0
#define ARROW_UP   1
#define ARROW_DOWN 0

#define POSITION_MAX 30
#define POSITION_MIN 0

class DriverManager;

class NEMA17Driver : public Driver, public Thread {

public:
    NEMA17Driver(DriverManager* driverManager);
    ~NEMA17Driver();

    ErrorCode start() override;
    ErrorCode stop() override;
    DataSignalsResponse getData(DataSignals SIGNAL) override;
    ErrorCode setData(DataSignals SIGNAL, uint16_t count, ...) override;

private:
    static int position;
    const uint8_t Step = 14;
    const uint8_t Dire = 2;
    const uint8_t Sleep = 12;

    //Motor Specs
    const int spr = 200; //Steps per revolution
    int RPM = 30; //Motor Speed in revolutions per minute
    int Microsteps = 1; //Stepsize (1 for full steps, 2 for half steps, 4 for quarter steps, etc)

    const uint16_t stepper = 10;
    uint32_t position_MAX;
    uint32_t position_MIN;

    static void* run(void* args);
    ErrorCode motorHigh();
    ErrorCode motorLow();
    ErrorCode saveMotorStatus();
    ErrorCode init() override;
    ErrorCode deinit() override;

    A4988 myStepper;
    MotorStatus motor_state;

};

#endif //MOTOR_L298N_DRIVER_HPP