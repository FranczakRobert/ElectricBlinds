#include "DriverManager.hpp"

DriverManager::DriverManager() {
    drivers_array[D_WIFI] = new WifiDriver();
}

ErrorCode DriverManager::initAllDrivers() {
    for(Driver *driver : drivers_array) {
        if(nullptr != driver) {
            driver->init();
        }
        else {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

ErrorCode DriverManager::deinitAllDrivers() {
    
    for(Driver *driver : drivers_array) {
        if(nullptr != driver) {
            driver->deinit();
        }
        else {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

ErrorCode DriverManager::startAllDrivers() {
    for(Driver *driver : drivers_array) {
        if(nullptr != driver) {
            driver->start();
        }
        else {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

ErrorCode DriverManager::stopAllDrivers() {
    for(Driver *driver : drivers_array) {
        if(nullptr != driver) {
            driver->stop();
        }
        else {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

DriverManager::~DriverManager() {
    for(Driver *driver : drivers_array) {
        delete driver;
    }
}