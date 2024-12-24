#ifndef DRIVER_MANAGER_HPP
#define DRIVER_MANAGER_HPP

#include "ESP32Server.hpp"
#include "LedDriver.hpp"
#include "WifiDriver.hpp"
#include "NEMA17Driver.hpp"

#include "MotorStatus.hpp"

class DriverManager {
    public:
    DriverManager();
    ~DriverManager();
    
    ErrorCode startAllDrivers();
    ErrorCode stopAllDrivers();
    ErrorCode initAllDrivers();
    ErrorCode deinitAllDrivers();

    u8_t getWifiStatus();
    u8_t setWifiLedOff();
    
    u8_t setMotorStatus(struct MotorStatus);
    
    private:
    WifiDriver wifiDriver = WifiDriver(this);
    LedDriver ledDriver = LedDriver(this);
    NEMA17Driver stepperDriver = NEMA17Driver(this);

    Driver* drivers_array[DRIVERS_NUMBER];

};

#endif // DRIVER_MANAGER_HPP