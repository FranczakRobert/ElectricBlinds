#ifndef THREAD_HPP
#define THREAD_HPP

#include <Arduino.h>
#include "ErrorCode.hpp"
#include "Driver.hpp"
#define CUSTOM_STACK_SIZE 8192

class Thread {
    protected:
    bool isRunning = true;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t ptid;

    ErrorCode startThread(const char* component, Driver* driver, void *funcPtr(void*), bool maxStackSize = false) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
    
        if (maxStackSize) {
            pthread_attr_setstacksize(&attr, CUSTOM_STACK_SIZE);
        }
    
        if (0 == pthread_create(&ptid, maxStackSize ? &attr : NULL, funcPtr, driver)) {
            Serial.printf("%s [start] - OK\n", component);
            pthread_attr_destroy(&attr);
            return E_OK;
        }
    
        Serial.printf("%s [start] - ERROR\n", component);
        pthread_attr_destroy(&attr);
        return E_NOT_OK;
    }
    

    ErrorCode stopThread(const char* component) {
        isRunning = 0;
        if(0 == pthread_join(ptid, NULL)) {
            Serial.printf("%s [stop] - OK\n",component);
            return E_OK;
        }
        Serial.printf("%s [stop] - ERROR\n",component);
        return E_NOT_OK;
    }
};

#endif //THREAD_HPP