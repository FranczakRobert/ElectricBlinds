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
    String MAX = NvmMemory::getInstance().readFromNvm("max", "maximum");
    String motor_position_nvm = NvmMemory::getInstance().readFromNvm("ESP32","MOTOR_POSITION");
    if(!motor_position_nvm.equals("")) {
        NEMA17Driver::position = motor_position_nvm.toInt();
    }
    else{
        NEMA17Driver::position = 0;
    }

    if(!MAX.equals("")) {
        NEMA17Driver::position_MAX = MAX.toInt();
        ESP32Server::GetInstance().max = MAX;
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
    int8_t prev_motor_state = -1;

    while (self->isRunning) {
        if(self->motor_state.status) {
            self->motorHigh();
        }
        else {
            self->motorLow();
        }
 
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    return nullptr;
}

void NEMA17Driver::saveMotorStatus() {
    std::string s = std::to_string(position);
    char const *pchar = s.c_str();
    NvmMemory::getInstance().writeToNvm("ESP32","MOTOR_POSITION",pchar);

    String motor_position_nvm = NvmMemory::getInstance().readFromNvm("ESP32","MOTOR_POSITION");
    Serial.print("Motor position: ");
    Serial.println(motor_position_nvm);
}

ErrorCode NEMA17Driver::motorLow()  {

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    return E_OK;
}

ErrorCode NEMA17Driver::motorMAXHigh()
{
    pthread_mutex_lock(&mutex);
    motor_state.direction = 1;
    for(int i = position; i < position_MAX; i += 10) {
        motorHigh();
    }
    pthread_mutex_unlock(&mutex);
    Serial.print("POSITION");
    Serial.println(position);
    NvmMemory::getInstance().writeToNvm("ESP32","MOTOR_POSITION",(const char*)&position);
    return ErrorCode();
}

ErrorCode NEMA17Driver::motorMAXLow()
{
    pthread_mutex_lock(&mutex);
    motor_state.direction = 0;
    for(int i = position; i > position_MIN; i -= 10) {
        motorHigh();
    }
    pthread_mutex_unlock(&mutex);

    Serial.print("POSITION");
    Serial.println(position);

    NvmMemory::getInstance().writeToNvm("ESP32","MOTOR_POSITION",(const char*)&position);
    return ErrorCode();
}

ErrorCode NEMA17Driver::motorHigh() {
    uint16_t stepper = 10;

    if(1 == motor_state.direction) {
        if(position < position_MIN) {
            position = position_MIN;
            return ErrorCode();
        }
        if(position < position_MAX) {
            myStepper.step(stepper);
            position += stepper;
        }
        
    }else if(0 == motor_state.direction) {
        if(position > position_MAX) {
            position = position_MAX;

            return ErrorCode();
        }
        if(position > position_MIN) {
            myStepper.step(-stepper);
            position -= stepper;
        }
    }

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW); 
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
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
        pthread_mutex_lock(&mutex);
        motor_state.status = HOLD;
        pthread_mutex_unlock(&mutex);
        break;
    
    case S_SET_NEMA_RELEASE_STATUS:
        pthread_mutex_lock(&mutex);
        motor_state.status = RELEASE;
        saveMotorStatus();
        pthread_mutex_unlock(&mutex);
        break;

    case S_SET_NEMA_MAX:
        if(count > 0) {
            va_list args;
            va_start(args, count);
            for (int i = 0; i < count; ++i) {
                position_MAX = va_arg(args, int);
            }
            va_end(args);            
            std::string s = std::to_string(position_MAX);
            char const *pchar = s.c_str();
            NvmMemory::getInstance().writeToNvm("max","maximum",pchar);
            ESP32Server::GetInstance().max = position_MAX;
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        
        break;

    case S_TRIGGER_NEMA_LOWERING: //TODO źle liczy kroki
        Serial.println("NEMA - S_TRIGGER_NEMA_LOWERING");
        motorMAXLow();
        motorLow();
        break;

    case S_TRIGGER_NEM_RAISING: //TODO źle liczy kroki
        Serial.println("NEMA - S_TRIGGER_NEM_RAISING");
        motorMAXHigh();
        motorLow();
        break;
    
    default:
        break;
    }
    return ErrorCode();
}

