#ifndef THREAD_HPP
#define THREAD_HPP

#include <Arduino.h>
#include "ErrorCode.hpp"

class Thread {
    protected:
    bool isRunning = true;
    pthread_t ptid;

    ErrorCode startThread(Driver* driver, void *funcPtr(void*)) {
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