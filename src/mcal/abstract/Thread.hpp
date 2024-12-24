#ifndef THREAD_HPP
#define THREAD_HPP

#include <Arduino.h>

class Thread {
    protected:
    bool isRunning = true;
    pthread_t ptid;
};

#endif //THREAD_HPP