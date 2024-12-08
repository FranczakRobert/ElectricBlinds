#ifndef WIFI_DRIVER_HPP
#define WIFI_DRIVER_HPP

#include "WiFi.h"
#include "Driver.hpp"

class WifiDriver : public Driver {

    public:
    WifiDriver();
    ~WifiDriver();
    
    ErrorCode init() override;
    ErrorCode deinit() override;
    ErrorCode start() override;
    ErrorCode stop() override;

};

#endif // WIFI_DRIVER_HPP