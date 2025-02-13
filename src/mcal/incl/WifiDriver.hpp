// WifiDriver.hpp
#ifndef WIFI_DRIVER_HPP
#define WIFI_DRIVER_HPP

#include "WiFi.h"
#include "Driver.hpp"
#include "Thread.hpp"

#define WIFI_CONNECTED     1
#define WIFI_NOT_CONNECTED 0

union WifiStats{
	u8_t state;
	struct STATS{
		u8_t isConnected : 2;
        u8_t hasCredentials : 1; 
	}status;
};

struct WifiData{
    String ssid;
    String psswd;
};

typedef struct WifiData WifiDataS;
typedef union WifiStats WifiStatsU;

class DriverManager;

class WifiDriver : public Driver, public Thread {
    
    static void* run(void* args);
    
public:
    WifiDriver(DriverManager* driverManager);
    ~WifiDriver();
    
    ErrorCode start() override;
    ErrorCode stop() override;
    WifiStats getWifiStats();

    private:
    WifiStatsU wifiStats = {0};
    WifiDataS wifiData;

    ErrorCode init() override;
    ErrorCode deinit() override;

    WifiData GetBlindsDataByAP();
};

#endif // WIFI_DRIVER_HPP
