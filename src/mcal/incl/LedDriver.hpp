#ifndef LED_DRIVER_HPP
#define LED_DRIVER_HPP

#include "Driver.hpp"
#include "Thread.hpp"

class DriverManager;

class LedDriver : public Driver, public Thread  {

    static void* run(void* args);

    public:
    LedDriver(DriverManager* driverManager);
    ~LedDriver();

    ErrorCode start() override;
    ErrorCode stop() override;
    ErrorCode setLedOff();

    private:
    ErrorCode init() override;
    ErrorCode deinit() override;

};


#endif // LED_DRIVER_HPP