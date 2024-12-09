#ifndef WIFI_DRIVER_HPP
#define WIFI_DRIVER_HPP

#include "WiFi.h"
#include "Driver.hpp"

class WifiDriver : public Driver {

    public:
    WifiDriver();
    ~WifiDriver();
    
    ErrorCode start() override;
    ErrorCode stop() override;

    private:
    ErrorCode init() override;
    ErrorCode deinit() override;

};

#endif // WIFI_DRIVER_HPP