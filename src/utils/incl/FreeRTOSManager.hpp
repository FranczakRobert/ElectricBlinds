#ifndef FREERTOS_MANAGER
#define FREERTOS_MANAGER
#include <Arduino.h>
#include "ErrorCode.hpp"

class FreeRTOSManager {
    public:
    static void createTask(void (*funcptr)(void* params), const char* taskName = "Task", uint32_t stackSize = 8192, UBaseType_t priority = 1, void* params = NULL) {
        xTaskCreate(
            funcptr,     
            taskName,    
            stackSize,   
            params,      
            priority,    
            NULL
        );
    }
};


#endif // FREERTOS_MANAGER