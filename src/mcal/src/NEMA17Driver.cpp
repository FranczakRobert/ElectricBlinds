#include "NEMA17Driver.hpp"
#include "DriverManager.hpp"


NEMA17Driver::NEMA17Driver(DriverManager *driverManager) 
: myStepper(200, IN1, IN2,IN3,IN4) {
    this->driverManager = driverManager;
    motor_state = 0;
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

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW); 
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    motor_state = 1;

    

    return ErrorCode();
}

ErrorCode NEMA17Driver::run() {
    while(1) {
        switch (motor_state)
        {
        case 0:
            start();
            break;

        case 1:
            stop();
        break;
        
        default:
            break;
        }

        if(0 == motor_state) {
           
        }else{
            // OVERHIT 
            stop();
        }

    }
}


ErrorCode NEMA17Driver::stop() {
    Serial.println("[NEMA17] - STOP");
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    motor_state = 3;

    return ErrorCode();
}

