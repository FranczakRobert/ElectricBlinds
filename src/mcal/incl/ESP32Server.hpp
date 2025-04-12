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
    static void handleReset();
    static void handleMax();
    static void handleMin();
    static void* run(void* args);

private:
    DriverManager* driverManager;
    ErrorCode init() override;
    ErrorCode deinit() override;

public:
    static ESP32Server& GetInstance();
    ESP32Server(ESP32Server &other) = delete;
    void operator=(const ESP32Server &) = delete;

    ErrorCode setManager(DriverManager *drMg);

    static WebServer server;
    static String loweringTimeVal;
    static String raisingTimeVal;
    static String max;
    static pthread_mutex_t timeValMutex;

    ErrorCode start() override;
    ErrorCode stop() override;
    DataSignalsResponse getData(DataSignals SIGNAL) override;
    ErrorCode setData(DataSignals SIGNAL, uint16_t count, ...) override;
};

#endif // SERVER_HPP