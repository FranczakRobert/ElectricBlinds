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
    ErrorCode stop() override;

    private:
    const int IN1 = 14;
    const int IN2 = 27;
    const int IN3 = 26;
    const int IN4 = 25;

    Stepper myStepper;
    ErrorCode init() override;
    ErrorCode deinit() override;

};

#endif //MOTOR_L298N_DRIVER_HPP