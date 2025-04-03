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
    ledState = LED_SYSTEM_BOOT_STATE;
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

ErrorCode LedDriver::setData(DataSignals SIGNAL, uint16_t count, ...)
{
    switch (SIGNAL)
    {
    case S_SET_WIFI_LED_OFF:
        digitalWrite(WIFI_LED,LOW);
        break;

    case S_SET_LED_STATE_CONFIG_MODE:
        pthread_mutex_lock(&mutex);
        ledState = LED_SYSTEM_CONFIG_STATE;
        pthread_mutex_unlock(&mutex);
        break;

    case S_SET_LED_STATE_BOOT_MODE:
        pthread_mutex_lock(&mutex);
        ledState = LED_SYSTEM_BOOT_STATE;
        pthread_mutex_unlock(&mutex);
        break;

    case S_SET_LED_STATE_ACTIVE_MODE:
        pthread_mutex_lock(&mutex);
        ledState = LED_SYSTEM_ACTIVE_STATE;
        pthread_mutex_unlock(&mutex);
        break;
    
    default:
        break;
    }

    return ErrorCode();
}


void *LedDriver::run(void *args) {
    LedDriver* self = static_cast<LedDriver*>(args);
    int8_t previousVal = -1;

    while (self->isRunning) {

        pthread_mutex_lock(&self->mutex);
        uint16_t led = self->ledState;
        pthread_mutex_unlock(&self->mutex);

        switch (led) {
            case LED_SYSTEM_BOOT_STATE:
                self->blinkSystem(500);
                previousVal = LED_SYSTEM_BOOT_STATE;
            break;

            case LED_SYSTEM_CONFIG_STATE:
                self->blinkSystem(100);
                previousVal = LED_SYSTEM_CONFIG_STATE;
            break;

            case LED_SYSTEM_ACTIVE_STATE:
            if(previousVal != LED_SYSTEM_ACTIVE_STATE) {
                self->blinkSystem(0,true);
                previousVal = LED_SYSTEM_ACTIVE_STATE;
            }
            break;
            
            default:
                break;
        }
        vTaskDelay(500);
    }

    digitalWrite(WIFI_LED,LOW);
    return nullptr;
}

void LedDriver::blinkSystem(uint32_t time, bool isActive) {
    if(isActive) {
        digitalWrite(WIFI_LED,HIGH);
        return;
    }

    digitalWrite(WIFI_LED,HIGH);
    vTaskDelay(time / portTICK_PERIOD_MS);
    digitalWrite(WIFI_LED,LOW);
    vTaskDelay(time / portTICK_PERIOD_MS);
}