#ifndef LED_DRIVER_HPP
#define LED_DRIVER_HPP

#include "Driver.hpp"
#include "Thread.hpp"
#define WIFI_LED  21

#define LED_SYSTEM_BOOT_STATE   0
#define LED_SYSTEM_ACTIVE_STATE 1
#define LED_SYSTEM_CONFIG_STATE 2

class DriverManager;

class LedDriver : public Driver, public Thread  {

    static void* run(void* args);

    public:
    LedDriver(DriverManager* driverManager);
    ~LedDriver();

    ErrorCode start() override;
    ErrorCode stop() override;
    DataSignalsResponse getData(DataSignals) override;
    ErrorCode setData(DataSignals SIGNAL, uint16_t count, ...) override;

    private:
    ErrorCode init() override;
    ErrorCode deinit() override;
    void blinkSystem(uint32_t time, bool isActive = false);

    volatile uint8_t ledState;

};


#endif // LED_DRIVER_HPP