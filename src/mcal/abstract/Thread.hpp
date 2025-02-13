#ifndef THREAD_HPP
#define THREAD_HPP

#include <Arduino.h>
#include "ErrorCode.hpp"
#define CUSTOM_STACK_SIZE 8192

class Thread {
    protected:
    bool isRunning = true;
    pthread_t ptid;

    ErrorCode startThread(Driver* driver, void *funcPtr(void*),bool maxStackSize = false) {

        if(maxStackSize) {
            pthread_t ptid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setstacksize(&attr, CUSTOM_STACK_SIZE);
        }

        if(0 == pthread_create(&ptid, NULL, funcPtr, driver)) {
            return E_OK;
        }
        
        return E_NOT_OK;
    }

    ErrorCode stopThread() {
        isRunning = 0;
        if(0 == pthread_join(ptid, NULL)) {
            return E_OK;
        }
        return E_NOT_OK;
    }
};

#endif //THREAD_HPP