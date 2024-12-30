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
    
    void initDeinit() {
    if(E_OK == driverManager.initAllDrivers()) {
        Serial.println("[Driver Manager] - Init all drivers.");
        if(E_OK == driverManager.startAllDrivers()) {
        Serial.println("[Driver Manager] - Started all drivers.");
        } else {
        Serial.println("[Driver Manager] - Started all drivers FAILED.");
        }
    } else {
        Serial.println("[Driver Manager] - Init all drivers FAILED.");
    }

    vTaskDelay(10000);

    if(E_OK == driverManager.stopAllDrivers()) {
        Serial.println("[Driver Manager] - stop all drivers.");
        if(E_OK == driverManager.deinitAllDrivers()) {
        Serial.println("[Driver Manager] - deinit all drivers.");
        } else {
        Serial.println("[Driver Manager] - deinit all drivers FAILED.");
        }
    } else {
        Serial.println("[Driver Manager] - stop all drivers FAILED.");
    }

    vTaskDelay(3000);

    if(E_OK == driverManager.initAllDrivers()) {
        Serial.println("[Driver Manager] - Init all drivers.");
        if(E_OK == driverManager.startAllDrivers()) {
        Serial.println("[Driver Manager] - Started all drivers.");
        } else {
        Serial.println("[Driver Manager] - Started all drivers FAILED.");
        }
    } else {
        Serial.println("[Driver Manager] - Init all drivers FAILED.");
    }

    vTaskDelay(10000);

    if(E_OK == driverManager.stopAllDrivers()) {
        Serial.println("[Driver Manager] - stop all drivers.");
        if(E_OK == driverManager.deinitAllDrivers()) {
        Serial.println("[Driver Manager] - deinit all drivers.");
        } else {
        Serial.println("[Driver Manager] - deinit all drivers FAILED.");
        }
    } else {
        Serial.println("[Driver Manager] - stop all drivers FAILED.");
    }
    }
    
};