#include "NEMA17Driver.hpp"
#include "DriverManager.hpp"


NEMA17Driver::NEMA17Driver(DriverManager *driverManager)
    : myStepper(200, IN1, IN2, IN3, IN4)
{
    this->driverManager = driverManager;
}

NEMA17Driver::~NEMA17Driver() {
}

ErrorCode NEMA17Driver::init() {
    isRunning = 1;
    myStepper.setSpeed(60);
    motor_state = {RELEASE,ARROW_UP};
    return ErrorCode();
}

ErrorCode NEMA17Driver::deinit()
{
    motorLow();
    return ErrorCode();
}

ErrorCode NEMA17Driver::start() { 
    isRunning = 1;
    if(0 == pthread_create(&ptid, NULL, &NEMA17Driver::run, this)) {
        Serial.println("[NEMA17][start] - OK");
        return E_OK;
    }
    Serial.println("[NEMA17][start] - ERROR");
    return E_NOT_OK;
}

void *NEMA17Driver::run(void *args)
{
    NEMA17Driver* self = static_cast<NEMA17Driver*>(args);
    while (self->isRunning) {
        
        if(self->motor_state.status) {
            self->motorHigh();
        }

        if(self->prev_motor_state != self->motor_state.status) {
            self->motorLow();
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
        self->prev_motor_state = self->motor_state.status;
    }
    return nullptr;
}

ErrorCode NEMA17Driver::motorLow()  {

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    return E_OK;
}

ErrorCode NEMA17Driver::motorHigh() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW); 
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    if(1 == motor_state.direction) {
        myStepper.step(10);
    }else{
        myStepper.step(-10);
    }
    return E_OK;
}

ErrorCode NEMA17Driver::stop() {
    isRunning = 0;
    if(0 == pthread_join(ptid, NULL)) {
        Serial.println("[NEMA17][stop] - OK");
        return E_OK;
    }
    Serial.println("[NEMA17][stop] - ERROR");
    return E_NOT_OK;
}

ErrorCode NEMA17Driver::setMotorState(struct MotorStatus motorStatus) {
    memcpy(&this->motor_state,&motorStatus,sizeof(motorStatus));
    return ErrorCode();
}
