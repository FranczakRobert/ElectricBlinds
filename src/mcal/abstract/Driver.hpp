#ifndef DRIVER_HPP
#define DRIVER_HPP
#include <Arduino.h>
#include "ErrorCode.hpp"
#include "Drivers.hpp"

class DriverManager;

class Driver {
    public:
    const char* TAG;

    Driver(){}
    virtual ~Driver(){}

    virtual ErrorCode init() = 0;
    virtual ErrorCode deinit() = 0;
    virtual ErrorCode start() = 0;
    virtual ErrorCode stop() = 0;

    protected:
    DriverManager* driverManager;
};


#endif // DRIVER_HPP