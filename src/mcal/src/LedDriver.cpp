#include "LedDriver.hpp"
#include "DriverManager.hpp"

LedDriver::LedDriver(DriverManager *driverManager)
{
    this->driverManager = driverManager;
}

LedDriver::~LedDriver() {

}

ErrorCode LedDriver::init()
{
    Serial.println("[LED] init...");
    pinMode(LED_BUILTIN,OUTPUT);
    return E_OK;
}

ErrorCode LedDriver::deinit()
{
    return ErrorCode();
}

ErrorCode LedDriver::start() {
    isRunning = 1;
    int er = pthread_create(&ptid, NULL, &LedDriver::run, this);
    return E_NOT_OK;
}

ErrorCode LedDriver::stop() {
    isRunning = 0;
    pthread_exit(NULL);
    vTaskDelay(30 / portTICK_PERIOD_MS);
    int a = pthread_join(ptid, NULL);
    Serial.println("[LED] - led stopped");

    return E_OK;
}

void *LedDriver::run(void *args)
{
    LedDriver* self = static_cast<LedDriver*>(args);

    while (self->isRunning) {
        switch (self->driverManager->getWifiStatus())
        {
        case 0:
            digitalWrite(LED_BUILTIN,HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            digitalWrite(LED_BUILTIN,LOW);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        break;

        case 1:
            digitalWrite(LED_BUILTIN,HIGH);
        break;
        
        default:
            break;
        }
        vTaskDelay(100);
    }
    digitalWrite(LED_BUILTIN,LOW);
    return nullptr;
}