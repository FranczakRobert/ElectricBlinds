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

    static WebServer server;
    static ESP32Server* instance;
    // bool isRunning;

    ESP32Server();
    ~ESP32Server() {delete instance;}

    static void handleRoot();
    static void handlePost();
    static void* run(void* args);

    private:
    DriverManager* driverManager;
    
    private:
    ErrorCode init() override;
    ErrorCode deinit() override;

    public:
    static ESP32Server* GetInstance();

    ErrorCode start() override;
    ErrorCode stop() override;
    ErrorCode setManager(DriverManager *drMg);

    ESP32Server(ESP32Server &other) = delete;
    void operator=(const ESP32Server &) = delete;
    
};

#endif // SERVER_HPP