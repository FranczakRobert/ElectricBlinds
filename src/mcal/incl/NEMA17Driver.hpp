#ifndef MOTOR_L298N_DRIVER_HPP
#define MOTOR_L298N_DRIVER_HPP

#include "Driver.hpp"
#include <Stepper.h>

class DriverManager;

class NEMA17Driver : public Driver {

    public:
    NEMA17Driver(DriverManager* driverManager);
    ~NEMA17Driver();

    ErrorCode start() override;
    ErrorCode run();
    ErrorCode stop() override;

private:
    const uint8_t IN1 = 14;
    const uint8_t IN2 = 27;
    const uint8_t IN3 = 26;
    const uint8_t IN4 = 25;

    uint8_t motor_state;

    Stepper myStepper;
    ErrorCode init() override;
    ErrorCode deinit() override;

};

#endif //MOTOR_L298N_DRIVER_HPP