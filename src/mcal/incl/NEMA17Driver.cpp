#include "NEMA17Driver.hpp"
#include "DriverManager.hpp"


NEMA17Driver::NEMA17Driver(DriverManager *driverManager) 
: myStepper(200, IN1, IN2,IN3,IN4) {
    this->driverManager = driverManager;
}

NEMA17Driver::~NEMA17Driver()
{
}

ErrorCode NEMA17Driver::init() {

    myStepper.setSpeed(60);
    return ErrorCode();
}

ErrorCode NEMA17Driver::deinit()
{
    return ErrorCode();
}

ErrorCode NEMA17Driver::start() {
    const int stepsPerRevolution = 200; // Ilość kroków na obrót (dla 1.8° na krok)
    const int fullTurnSteps = stepsPerRevolution; // 360 stopni

    // Obrót o 360 stopni w prawo
    myStepper.step(fullTurnSteps);

    // Krótka przerwa
    vTaskDelay(300 / portTICK_PERIOD_MS);

    // Obrót o 520 stopni w lewo
    myStepper.step(-fullTurnSteps);

    // Krótka przerwa
    vTaskDelay(300 / portTICK_PERIOD_MS);

    return ErrorCode();
}


ErrorCode NEMA17Driver::stop()
{
    return ErrorCode();
}
