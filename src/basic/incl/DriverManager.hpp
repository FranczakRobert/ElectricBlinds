#ifndef DRIVER_MANAGER_HPP
#define DRIVER_MANAGER_HPP

#include "Driver.hpp"
#include "WifiDriver.hpp"
#include "ESP32Server.hpp"

class DriverManager {
    public:
    DriverManager();
    ~DriverManager();
    
    ErrorCode initAllDrivers();
    ErrorCode deinitAllDrivers();
    ErrorCode startAllDrivers();
    ErrorCode stopAllDrivers();

    private:
    WifiDriver wifiDriver;
    Driver* drivers_array[DRIVERS_NUMBER];

};

#endif // DRIVER_MANAGER_HPP