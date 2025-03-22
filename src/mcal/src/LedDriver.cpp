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
    // Serial.println("HALOOOO");
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


// TODO Zrobić aby led mrugał dopiero po fetchu godziny
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
            break;

            case LED_SYSTEM_CONFIG_STATE:
                self->blinkSystem(100);
            break;

            case LED_SYSTEM_ACTIVE_STATE:
            if(previousVal != LED_SYSTEM_BOOT_STATE) {
                self->blinkSystem(0,true);
                previousVal = LED_SYSTEM_BOOT_STATE;
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

// TODO Zrobić aby led mrugał dopiero po fetchu godziny
// void *LedDriver::run(void *args)
// {
//     LedDriver* self = static_cast<LedDriver*>(args);
//     s8_t lastValue = -1;

//     while (self->isRunning) {
//         u8_t isWifiConnected = self->driverManager->getDriverData(D_WIFI,S_GET_WIFI_STATUS);
//         switch (isWifiConnected) {
//             case WIFI_NOT_CONNECTED:
//                 digitalWrite(WIFI_LED,HIGH);
//                 vTaskDelay(500 / portTICK_PERIOD_MS);
//                 digitalWrite(WIFI_LED,LOW);
//                 vTaskDelay(500 / portTICK_PERIOD_MS);
//                 lastValue = WIFI_NOT_CONNECTED;
//             break;

//             case WIFI_CONFIG_MODE:
//                 digitalWrite(WIFI_LED,HIGH);
//                 vTaskDelay(100 / portTICK_PERIOD_MS);
//                 digitalWrite(WIFI_LED,LOW);
//                 vTaskDelay(100 / portTICK_PERIOD_MS);
//                 lastValue = WIFI_NOT_CONNECTED;
//             break;

//             case WIFI_CONNECTED:
//                 if(isWifiConnected != lastValue) {
//                     digitalWrite(WIFI_LED,HIGH);
//                 }
//                 lastValue = WIFI_CONNECTED;
//             break;
            
//             default:
//                 break;
//         }
//         vTaskDelay(500);
//     }
//     digitalWrite(WIFI_LED,LOW);
//     return nullptr;
// }