#ifndef SERVER_HPP
#define SERVER_HPP

#include "Driver.hpp"
#include <WiFi.h>
#include <WebServer.h>

class ESP32Server : public Driver {
    protected:

    static WebServer server;
    static ESP32Server* instance;

    ESP32Server();
    ~ESP32Server() {delete instance;}

    static void handleRoot();
    static void handlePost();

    public:
    ErrorCode init() override;
    ErrorCode deinit() override;
    ErrorCode start() override;
    ErrorCode stop() override;

    static ESP32Server* GetInstance();
    ESP32Server(ESP32Server &other) = delete;
    void operator=(const ESP32Server &) = delete;

};

#endif // SERVER_HPP