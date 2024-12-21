#include "LedDriver.hpp"
#include "DriverManager.hpp"


LedDriver::LedDriver(DriverManager *driverManager) {
    this->driverManager = driverManager;
}

LedDriver::~LedDriver() {

}

ErrorCode LedDriver::init() {
    Serial.println("[LED] init...");
    pinMode(LED_BUILTIN,OUTPUT);
    return E_OK;
}

ErrorCode LedDriver::deinit()
{
    return ErrorCode();
}

ErrorCode LedDriver::start() {
    Serial.println("[LED] - started");
    if (xTaskCreate(led_wifi_connecting_state_wrapper, "LED_Connecting_State", 2048, this, 1, &xHandle) == pdPASS) {
        return E_OK;
    }
    return E_NOT_OK;
}

ErrorCode LedDriver::stop() {
    if(eTaskGetState(xHandle) == eRunning) {
        vTaskDelete(xHandle);
        if (eTaskGetState(xHandle) == eDeleted) {
            digitalWrite(LED_BUILTIN, LOW);
            return E_OK;
        }
    }
    return E_NOT_OK;
}

void LedDriver::led_wifi_connecting_state_wrapper(void* _this) {
    if(E_OK == static_cast<LedDriver*>(_this)->wifi_led_connecting()) {
        vTaskDelete(NULL);
    }
    else {
        Serial.println("[LED] Task not running...");
    }
}

ErrorCode LedDriver::wifi_led_connecting() {
    
    while (!driverManager->getWifiStatus()) {
        digitalWrite(LED_BUILTIN,HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN,LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    return E_OK;
}
