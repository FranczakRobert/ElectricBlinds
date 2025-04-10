#ifndef NVM_MEMORY_HPP
#define NVM_MEMORY_HPP

#include "Arduino.h"
#include <Preferences.h>
class NvmMemory
{
private:
    NvmMemory(){/*Empty*/};
    NvmMemory(const NvmMemory&) = delete;
    NvmMemory& operator=(const NvmMemory&) = delete;

    Preferences pref;

public:
    static NvmMemory& getInstance() {
        static NvmMemory instance;
        return instance;
    }

    void writeToNvm(const char* namespaceName, const char* key, const char* value) {
        pref.begin(namespaceName, false);
        pref.putString(key,value);
        pref.end();
    }

    String readFromNvm(const char* namespaceName, const char* key) {
        pref.begin(namespaceName, false);
        String value = pref.getString(key,"");
        if(value.isEmpty()) {
            Serial.println("[NVM][readFromNvm] - readed value is empty");
        }
        pref.end();
        
        return value;
    }
};

#endif // NVM_MEMORY_HPP
