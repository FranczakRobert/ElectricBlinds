#ifndef MOTOR_L298N_DRIVER_HPP
#define MOTOR_L298N_DRIVER_HPP

#include "Driver.hpp"
#include <Stepper.h>
#include "MotorStatus.hpp" 
#include "Thread.hpp"

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

    static void* run(void* args);

    void saveMotorStatus();

public:
    NEMA17Driver(DriverManager* driverManager);
    ~NEMA17Driver();

    ErrorCode start() override;
    ErrorCode stop() override;
    DataSignalsResponse getData(DataSignals SIGNAL) override;
    ErrorCode setData(DataSignals SIGNAL, uint16_t count, ...) override;

private:
    static int position;

    const uint8_t IN1 = 14;
    const uint8_t IN2 = 27;
    const uint8_t IN3 = 26;
    const uint8_t IN4 = 25;
    const uint16_t stepper = 10;

    uint32_t position_MAX;
    uint32_t position_MIN;

    ErrorCode motorHigh();
    ErrorCode motorLow();

    ErrorCode motorMAXHigh();
    ErrorCode motorMAXLow();

    MotorStatus motor_state;

    Stepper myStepper;
    ErrorCode init() override;
    ErrorCode deinit() override;

};

#endif //MOTOR_L298N_DRIVER_HPP