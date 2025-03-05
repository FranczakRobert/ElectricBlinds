#ifndef LED_DRIVER_HPP
#define LED_DRIVER_HPP

#include "Driver.hpp"
#include "Thread.hpp"
#define WIFI_LED  21

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

};


#endif // LED_DRIVER_HPP