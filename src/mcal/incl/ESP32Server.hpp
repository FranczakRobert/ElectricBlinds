#ifndef SERVER_HPP
#define SERVER_HPP

#include "Driver.hpp"
#include <WiFi.h>
#include <WebServer.h>
#include "DriverManager.hpp"
#include "MotorStatus.hpp"
#include "Thread.hpp"

class ESP32Server : public Driver , public Thread {
    protected:

    ESP32Server();
    ~ESP32Server() {}

    static void handleRoot();
    static void handleMotorPost();
    static void handleBlindsTimerPost();
    static void setRandLTimers();
    static void* run(void* args);

    private:
    DriverManager* driverManager;
    ErrorCode init() override;
    ErrorCode deinit() override;

    public:
    static WebServer server;
    static String loweringTimeVal;
    static String raisingTimeVal;

    static ESP32Server& GetInstance() {
        static ESP32Server instance;
        return instance;
    }

    ErrorCode start() override;
    ErrorCode stop() override;
    ErrorCode setManager(DriverManager *drMg);

    ESP32Server(ESP32Server &other) = delete;
    void operator=(const ESP32Server &) = delete;
    
};

#endif // SERVER_HPP