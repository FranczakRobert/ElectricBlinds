// WifiDriver.hpp
#ifndef WIFI_DRIVER_HPP
#define WIFI_DRIVER_HPP

#include "WiFi.h"
#include "Driver.hpp"

#define WIFI_CONNECTED     1
#define WIFI_NOT_CONNECTED 0

class DriverManager;

class WifiDriver : public Driver {
    public:
    WifiDriver(DriverManager* driverManager);
    ~WifiDriver();
    
    ErrorCode start() override;
    ErrorCode stop() override;
    u8_t getIsConnected();

    private:
    u8_t isConnected;
    ErrorCode init() override;
    ErrorCode deinit() override;

    DriverManager* driverManager;
};

#endif // WIFI_DRIVER_HPP
