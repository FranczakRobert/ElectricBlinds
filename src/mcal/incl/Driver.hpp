#ifndef DRIVER_HPP
#define DRIVER_HPP
#include <Arduino.h>
#include "ErrorCode.hpp"
#include "Drivers.hpp"

class Driver {
    public:
    Driver(){}
    virtual ~Driver(){}

    virtual ErrorCode init() = 0;
    virtual ErrorCode deinit() = 0;
    virtual ErrorCode start() = 0;
    virtual ErrorCode stop() = 0;
};


#endif // DRIVER_HPP