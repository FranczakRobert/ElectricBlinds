#include "DriverManager.hpp"

DriverManager::DriverManager() {
    drivers_array[D_LED] = &ledDriver;
    drivers_array[D_WIFI] = &wifiDriver;
    drivers_array[D_NEMA17] = &stepperDriver;
    drivers_array[D_SCHEDULER] = &scheduler;
}

ErrorCode DriverManager::initAllDrivers() {

    for(int driverIndex = 0; driverIndex < DRIVERS_NUMBER; driverIndex++) {
        if(nullptr != drivers_array[driverIndex]) {
            if(E_NOT_OK == drivers_array[driverIndex]->init()) {
                Serial.printf("[Driver Manager] - init driver nr %d failed \n",driverIndex);
                return E_NOT_OK;
            }
            Serial.printf("[Driver Manager] - init driver nr %d \n",driverIndex);
        }
        else {
            Serial.printf("[Driver Manager] - init driver nr %d is nullptr \n",driverIndex);
            return E_NOT_OK;
        }
    }

    ESP32Server::GetInstance().setManager(this);

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
            Serial.printf("[Driver Manager] - deinit driver nr %d is nullptr \n",driverIndex);
            return E_NOT_OK;
        }
    }
    return E_OK;
}

ErrorCode DriverManager::setDriverData(Drivers driver, DataSignals signal)
{
    setDriverData(driver,signal,0);
    return ErrorCode();
}

ErrorCode DriverManager::setDriverData(Drivers driver, DataSignals signal ,u16_t count = 0, ...) {
    s32_t result = -1;

    if(count > 0) {
        va_list args;
        va_start(args, count);
        
        for (int i = 0; i < count; ++i) {
            result = va_arg(args, int);
            drivers_array[driver]->setData(signal,1,result);
        }            
        va_end(args);
        return ErrorCode();
    }
    drivers_array[driver]->setData(signal,0);
    return ErrorCode();
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
            Serial.printf("[Driver Manager] - start driver nr %d is nullptr \n",driverIndex);
            return E_NOT_OK;
        }
    }

    ESP32Server::GetInstance().start();
    return E_OK;
}

ErrorCode DriverManager::stopAllDrivers() {

    ESP32Server::GetInstance().stop();

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
    return E_OK;
}

DriverManager::~DriverManager() {

}