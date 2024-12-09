#include "DriverManager.hpp"

DriverManager::DriverManager() {
    drivers_array[D_WIFI] = &wifiDriver;
}

ErrorCode DriverManager::initAllDrivers() {

    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            drivers_array[driverIndex]->init();
        }
        else {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

ErrorCode DriverManager::deinitAllDrivers() {

    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            drivers_array[driverIndex]->deinit();
        }
        else {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

ErrorCode DriverManager::startAllDrivers() {

    initAllDrivers();

    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            drivers_array[driverIndex]->start();
        }
        else {
            return E_NOT_OK;
        }
    }
    ESP32Server::GetInstance()->init();

    return E_OK;
}

ErrorCode DriverManager::stopAllDrivers() {
    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            drivers_array[driverIndex]->stop();
        }
        else {
            return E_NOT_OK;
        }
    }

    deinitAllDrivers();
    
    return E_OK;
}

DriverManager::~DriverManager() {

}