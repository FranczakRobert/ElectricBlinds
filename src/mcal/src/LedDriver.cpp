#include "LedDriver.hpp"
#include "DriverManager.hpp"

LedDriver::LedDriver(DriverManager *driverManager)
{
    this->driverManager = driverManager;
    TAG = "LED";
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
    return startThread(TAG,this,run);
    
}

ErrorCode LedDriver::stop() {
    return stopThread(TAG);
}

DataSignalsResponse LedDriver::getData(DataSignals)
{
    return DataSignalsResponse();
}

ErrorCode LedDriver::setData(DataSignals SIGNAL)
{
    switch (SIGNAL)
    {
    case S_SET_WIFI_LED_OFF:
        digitalWrite(WIFI_LED,LOW);
        break;
    
    default:
        break;
    }

    return ErrorCode();
}

void *LedDriver::run(void *args)
{
    LedDriver* self = static_cast<LedDriver*>(args);
    s8_t lastValue = -1;
    while (self->isRunning) {
        u8_t isWifiConnected = self->driverManager->getDriverData(D_WIFI,S_GET_WIFI_STATUS);
        switch (isWifiConnected) {
            case WIFI_NOT_CONNECTED:
                digitalWrite(WIFI_LED,HIGH);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                digitalWrite(WIFI_LED,LOW);
                vTaskDelay(500 / portTICK_PERIOD_MS);
                lastValue = WIFI_NOT_CONNECTED;
            break;

            case WIFI_CONFIG_MODE:
                digitalWrite(WIFI_LED,HIGH);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                digitalWrite(WIFI_LED,LOW);
                vTaskDelay(100 / portTICK_PERIOD_MS);
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
        vTaskDelay(500);
    }
    digitalWrite(WIFI_LED,LOW);
    return nullptr;
}