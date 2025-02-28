#include "NEMA17Driver.hpp"
#include "DriverManager.hpp"
#include "NvmMemory.hpp"

#include <Preferences.h>
class Preferences;
int NEMA17Driver::position;

NEMA17Driver::NEMA17Driver(DriverManager *driverManager)
    : myStepper(200, IN1, IN2, IN3, IN4)
{
    this->driverManager = driverManager;
    TAG = "NEMA17";
}

NEMA17Driver::~NEMA17Driver() {
}

ErrorCode NEMA17Driver::init() {
    myStepper.setSpeed(30);
    motor_state = {RELEASE,ARROW_UP};

    position_MAX = 300;
    position_MIN = 0;

    String str = NvmMemory::getInstance().readFromNvm("ESP32","MOTOR_POSITION");
    if(!str.equals("")) {
        NEMA17Driver::position = str.toInt();
    }
    else{
        NEMA17Driver::position = 0;
    }
    return ErrorCode();
}

ErrorCode NEMA17Driver::deinit()
{
    motorLow();
    return ErrorCode();
}

ErrorCode NEMA17Driver::start() { 
    return startThread(TAG,this,run);
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
            std::string s = std::to_string(self->position);
            char const *pchar = s.c_str();
            NvmMemory::getInstance().writeToNvm("ESP32","MOTOR_POSITION",pchar);
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

ErrorCode NEMA17Driver::motorMAXHigh()
{
    return ErrorCode();
}

ErrorCode NEMA17Driver::motorMAXLow()
{
    return ErrorCode();
}

ErrorCode NEMA17Driver::motorHigh() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW); 
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    if(1 == motor_state.direction) {
        if(position < position_MIN) {
            position = position_MIN;
        }
        if(position < position_MAX) {
            myStepper.step(10);
            position++;
        }
        
    }else if(0 == motor_state.direction) {
        if(position > position_MAX) {
            position = position_MAX;
        }
        if(position > position_MIN) {
            myStepper.step(-10);
            position--;
        }
    }
    return E_OK;
}

ErrorCode NEMA17Driver::stop() {
    return stopThread(TAG);
}

DataSignalsResponse NEMA17Driver::getData(DataSignals SIGNAL)
{
    return DataSignalsResponse();
}

ErrorCode NEMA17Driver::setData( DataSignals SIGNAL, uint16_t count, ...)
{
    switch (SIGNAL)
    {
    case S_SET_NEMA_UP_STATUS:
        motor_state.direction = ARROW_UP;
        break;

    case S_SET_NEMA_DOWN_STATUS:
        motor_state.direction = ARROW_DOWN;
        break;

    case S_SET_NEMA_HOLD_STATUS:
        motor_state.status = HOLD;
        break;
    
    case S_SET_NEMA_RELEASE_STATUS:
        motor_state.status = RELEASE;
        break;
    
    default:
        break;
    }
    return ErrorCode();
}

