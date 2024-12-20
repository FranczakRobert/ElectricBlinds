#ifndef DRIVER_MANAGER_HPP
#define DRIVER_MANAGER_HPP

#include "Driver.hpp"
#include "ESP32Server.hpp"
#include "LedDriver.hpp"
#include "WifiDriver.hpp"

class DriverManager {
    public:
    DriverManager();
    ~DriverManager();
    
    ErrorCode startAllDrivers();
    ErrorCode stopAllDrivers();
    ErrorCode initAllDrivers();
    ErrorCode deinitAllDrivers();

    u8_t getWifiStatus();
    
    private:
    WifiDriver wifiDriver = WifiDriver(this);
    LedDriver ledDriver = LedDriver(this);;

    Driver* drivers_array[DRIVERS_NUMBER];

};

#endif // DRIVER_MANAGER_HPP