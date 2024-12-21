#ifndef LED_DRIVER_HPP
#define LED_DRIVER_HPP

#include <Arduino.h>
#include "ErrorCode.hpp"
#include "Drivers.hpp"
#include "Driver.hpp"
#include "WifiDriver.hpp"

class DriverManager;

class LedDriver : public Driver {
    TaskHandle_t xHandle = NULL;

    static void led_wifi_connecting_state_wrapper(void* _this);
    
    public:
    LedDriver(DriverManager* driverManager);
    ~LedDriver();

    ErrorCode start() override;
    ErrorCode stop() override;

    ErrorCode wifi_led_connecting();

    private:
    DriverManager* driverManager;
    ErrorCode init() override;
    ErrorCode deinit() override;

};


#endif // LED_DRIVER_HPP