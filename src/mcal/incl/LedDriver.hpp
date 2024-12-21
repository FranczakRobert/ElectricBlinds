#ifndef LED_DRIVER_HPP
#define LED_DRIVER_HPP

#include "Driver.hpp"

class DriverManager;

class LedDriver : public Driver {

    static void led_wifi_connecting_state_wrapper(void* _this);

    public:
    LedDriver(DriverManager* driverManager);
    ~LedDriver();

    ErrorCode start() override;
    ErrorCode stop() override;
    ErrorCode wifi_led_connecting();

    private:
    ErrorCode init() override;
    ErrorCode deinit() override;

};


#endif // LED_DRIVER_HPP