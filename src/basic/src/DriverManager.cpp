#include "DriverManager.hpp"

DriverManager::DriverManager() {
    drivers_array[D_WIFI] = &wifiDriver;
    drivers_array[D_LED] = &ledDriver;
    drivers_array[D_NEMA17] = &stepperDriver;
}

ErrorCode DriverManager::initAllDrivers() {

    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            if(E_NOT_OK == drivers_array[driverIndex]->init()) {
                Serial.printf("[Driver Manager] - init driver nr %d failed \n",driverIndex);
                return E_NOT_OK;
            }
        }
        else {
            return E_NOT_OK;
        }
    }
    Serial.println("Init performed...");
    return E_OK;
}

ErrorCode DriverManager::deinitAllDrivers() {

    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            if(E_NOT_OK == drivers_array[driverIndex]->deinit()) {
                Serial.printf("[Driver Manager] - deinit driver nr %d failed \n",driverIndex);
                return E_NOT_OK;
            }
        }
        else {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

u8_t DriverManager::getWifiStatus()
{
    return wifiDriver.getIsConnected();
}

ErrorCode DriverManager::startAllDrivers() {

    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            if(E_NOT_OK == drivers_array[driverIndex]->start()) {
                Serial.printf("[Driver Manager] - start driver nr %d failed \n",driverIndex);
                return E_NOT_OK;
            }
        }
        else {
            return E_NOT_OK;
        }
    }

    ESP32Server::GetInstance()->start();

    drivers_array[D_NEMA17]->stop();

    return E_OK;
}

ErrorCode DriverManager::stopAllDrivers() {
    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            if(E_NOT_OK == drivers_array[driverIndex]->stop()) {
                Serial.printf("[Driver Manager] - stop driver nr %d failed \n",driverIndex);
                return E_NOT_OK;
            }
        }
        else {
            return E_NOT_OK;
        }
    }
    
    ESP32Server::GetInstance()->stop();

    return E_OK;
}

DriverManager::~DriverManager() {

}