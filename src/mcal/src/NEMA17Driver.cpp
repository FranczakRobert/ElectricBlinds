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
    motor_state = {RELEASE,ARROW_UP};
    int er = pthread_create(&ptid, NULL, &NEMA17Driver::run, this);
    myStepper.setSpeed(60);

    return ErrorCode();
}

ErrorCode NEMA17Driver::deinit()
{
    isRunning = 0;
    int a = pthread_join(ptid, NULL);
    Serial.println("[NEMA17] - deinit");
    return ErrorCode();
}

ErrorCode NEMA17Driver::start() { 

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW); 
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    if(1 == motor_state.direction) {
        myStepper.step(10);
    }else{
        myStepper.step(-10);
    }

    return ErrorCode();
}

void *NEMA17Driver::run(void *args)
{
    NEMA17Driver* self = static_cast<NEMA17Driver*>(args);
    while (self->isRunning) {

        while(self->motor_state.status) {
            self->start();
        }

        if(self->prev_motor_state != self->motor_state.status) {
            self->stop();
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
        self->prev_motor_state = self->motor_state.status;
    }
    return nullptr;
}


ErrorCode NEMA17Driver::stop() {

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    return ErrorCode();
}

ErrorCode NEMA17Driver::setMotorState(struct MotorStatus motorStatus) {
    memcpy(&this->motor_state,&motorStatus,sizeof(motorStatus));
    return ErrorCode();
}
