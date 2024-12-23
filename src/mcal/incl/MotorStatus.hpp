#ifndef MOTOR_STATUS_HPP
#define MOTOR_STATUS_HPP

#include <Arduino.h>

struct MotorStatus {
    uint8_t status    : 2; // hold 1 release 0
    uint8_t direction : 2; // gora 1 dol 0

};


#endif //MOTOR_STATUS_HPP