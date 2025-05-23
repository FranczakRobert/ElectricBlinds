#ifndef DRIVER_MANAGER_HPP
#define DRIVER_MANAGER_HPP

#include "ESP32Server.hpp"
#include "LedDriver.hpp"
#include "WifiDriver.hpp"
#include "NEMA17Driver.hpp"
#include "Scheduler.hpp"

class DriverManager {
    public:
    DriverManager();
    ~DriverManager();
    
    ErrorCode startAllDrivers();
    ErrorCode stopAllDrivers();
    ErrorCode initAllDrivers();
    ErrorCode deinitAllDrivers();

    ErrorCode setDriverData(Drivers driver, DataSignals signal);
    ErrorCode setDriverData(Drivers driver, DataSignals signal ,u16_t count,  ...);

    private:
    WifiDriver wifiDriver = WifiDriver(this);
    LedDriver ledDriver = LedDriver(this);
    NEMA17Driver stepperDriver = NEMA17Driver(this);
    Scheduler scheduler = Scheduler(this);

    Driver* drivers_array[DRIVERS_NUMBER];

};

#endif // DRIVER_MANAGER_HPP