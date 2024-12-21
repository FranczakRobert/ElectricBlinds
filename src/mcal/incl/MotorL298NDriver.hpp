#ifndef MOTOR_L298N_DRIVER_HPP
#define MOTOR_L298N_DRIVER_HPP

#include "Driver.hpp"

class DriverManager;

class MotorL298NDriver : public Driver {

    public:
    MotorL298NDriver(DriverManager* driverManager);
    ~MotorL298NDriver();

    ErrorCode start() override;
    ErrorCode stop() override;

    private:
    ErrorCode init() override;
    ErrorCode deinit() override;

};

#endif //MOTOR_L298N_DRIVER_HPP