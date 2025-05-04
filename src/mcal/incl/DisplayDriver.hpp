#ifndef LED_DRIVER_HPP
#define LED_DRIVER_HPP

#include "Driver.hpp"
#include "Thread.hpp"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>

#define WIFI_LED  21

#define OLED_SYSTEM_BOOT_STATE   0
#define OLED_SYSTEM_ACTIVE_STATE 1
#define OLED_SYSTEM_CONFIG_STATE 2
#define OLED_SYSTEM_WIFI_DISCONNECTED 3
#define OLED_SYSTEM_WIFI_CONNECTED 4
#define OLED_SYSTEM_TIME_ACTIVE 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

class DriverManager;

class DisplayDriver : public Driver, public Thread  {
    
    static void* run(void* args);

    public:
    DisplayDriver(DriverManager* driverManager);
    ~DisplayDriver();

    ErrorCode start() override;
    ErrorCode stop() override;
    DataSignalsResponse getData(DataSignals) override;
    ErrorCode setData(DataSignals SIGNAL, uint16_t count, ...) override;

    private:
    ErrorCode init() override;
    ErrorCode deinit() override;

    Adafruit_SSD1306 display;
    uint16_t displayState;

    uint16_t firstRow = 13;
    uint16_t secondRow = 32;
    uint16_t thirdRow = 57;
    String low;
    String rais;

};


#endif // LED_DRIVER_HPP