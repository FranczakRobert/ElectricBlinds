#include "LedDriver.hpp"
#include "DriverManager.hpp"

TaskHandle_t asd = NULL;


LedDriver::LedDriver(DriverManager *driverManager) {
    this->driverManager = driverManager;
}

LedDriver::~LedDriver()
{
}

ErrorCode LedDriver::init() {
    Serial.println("[LED] init...");
    pinMode(LED_BUILTIN,OUTPUT);
    return ErrorCode();
}

ErrorCode LedDriver::deinit()
{
    return ErrorCode();
}

ErrorCode LedDriver::start() {
    Serial.println("[LED] - started");
    if (xTaskCreate(led_wifi_connecting_state_wrapper, "LED_Connecting_State", 2048, this, 1, &asd) == pdPASS) {
        return E_OK;
    }
    return E_NOT_OK;
}

ErrorCode LedDriver::stop() {
    vTaskDelete(asd);
    digitalWrite (LED_BUILTIN, LOW);
    return ErrorCode();
}

void LedDriver::led_wifi_connecting_state_wrapper(void* _this) {
    static_cast<LedDriver*>(_this)->wifi_led_connecting();
    vTaskDelete(NULL);
}

ErrorCode LedDriver::wifi_led_connecting() {
    Serial.println(driverManager->getWifiStatus());
    while (!driverManager->getWifiStatus()) {
        digitalWrite(LED_BUILTIN,HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN,LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    return ErrorCode();
}

// ErrorCode LedDriver::wifi_led_connected() {
//     vTaskDelete(asd);
//     digitalWrite(LED_BUILTIN,HIGH);
//     return ErrorCode();
// }

// ErrorCode LedDriver::wifi_led_disconneced() {
//     digitalWrite(LED_BUILTIN,LOW);
//     return ErrorCode();
// }
