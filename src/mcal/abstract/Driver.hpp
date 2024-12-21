#ifndef DRIVER_HPP
#define DRIVER_HPP
#include <Arduino.h>
#include "ErrorCode.hpp"
#include "Drivers.hpp"

class DriverManager;

class Driver {
    public:
    Driver(){}
    virtual ~Driver(){}

    virtual ErrorCode init() = 0;
    virtual ErrorCode deinit() = 0;
    virtual ErrorCode start() = 0;
    virtual ErrorCode stop() = 0;

    protected:
    TaskHandle_t xHandle = NULL;
    DriverManager* driverManager;
};


#endif // DRIVER_HPP