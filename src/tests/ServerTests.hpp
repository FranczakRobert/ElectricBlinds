#include <Arduino.h>

#include "test.hpp"

class ServerTests {
    DriverManager driverManager;
    public:
    void test1() {
        WifiDriver wifi = WifiDriver(&driverManager);
        
        wifi.start();
        ESP32Server::GetInstance()->start();
        
        vTaskDelay(5000);

        ESP32Server::GetInstance()->stop();
        wifi.stop();

        vTaskDelay(2000);

        wifi.start();
        ESP32Server::GetInstance()->start();
        vTaskDelay(5000);

        ESP32Server::GetInstance()->stop();
        wifi.stop();

    }
    
};