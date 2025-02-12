#include "LedDriver.hpp"
#include "DriverManager.hpp"

LedDriver::LedDriver(DriverManager *driverManager)
{
    this->driverManager = driverManager;
}

LedDriver::~LedDriver() {

}

ErrorCode LedDriver::init() {
    pinMode(WIFI_LED,OUTPUT);
    return E_OK;
}

ErrorCode LedDriver::deinit() {
    digitalWrite(WIFI_LED,LOW);
    return ErrorCode();
}

ErrorCode LedDriver::start() {
    isRunning = 1;
    if(0 == pthread_create(&ptid, NULL, &LedDriver::run, this)){
        Serial.println("[LED][start] - OK");
        return E_OK;
    }
    Serial.println("[LED][start] - ERROR");
    return E_NOT_OK;
    
}

ErrorCode LedDriver::stop() {
    isRunning = 0;
    if(0 == pthread_join(ptid, NULL)) {
        Serial.println("[LED][stop] - OK");
        return E_OK;
    }
    Serial.println("[LED][stop] - ERROR");
    return E_NOT_OK;
}

ErrorCode LedDriver::setLedOff()
{
    digitalWrite(WIFI_LED,LOW);
    return ErrorCode();
}

void *LedDriver::run(void *args)
{
    LedDriver* self = static_cast<LedDriver*>(args);
    u8_t lastValue = 0;
    while (self->isRunning) {
        u8_t isWifiConnected = self->driverManager->getWifiStatus().status.isConnected;
        switch (isWifiConnected) {
            case WIFI_NOT_CONNECTED:
                digitalWrite(WIFI_LED,HIGH);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                digitalWrite(WIFI_LED,LOW);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                lastValue = WIFI_NOT_CONNECTED;
            break;

            case WIFI_CONNECTED:
                if(isWifiConnected != lastValue) {
                    digitalWrite(WIFI_LED,HIGH);
                }
                lastValue = WIFI_CONNECTED;
            break;
            
            default:
                break;
        }
        vTaskDelay(1000);
    }
    digitalWrite(WIFI_LED,LOW);
    return nullptr;
}