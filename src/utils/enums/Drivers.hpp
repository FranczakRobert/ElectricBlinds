#ifndef DRIVERS_HPP
#define DRIVERS_HPP

#define DRIVERS_NUMBER 4

enum Drivers {
    D_LED = 0,
    D_SCHEDULER = 1,
    D_WIFI = 2,
    D_NEMA17 = 3,
};

enum DataSignals {
    // WIFI
    S_GET_WIFI_STATUS = 0,
    S_SET_WIFI_LED_OFF,

    //LED
    S_TRIGGER_RESET,

    //NEMA17
    S_SET_NEMA_UP_STATUS,
    S_SET_NEMA_DOWN_STATUS,
    S_SET_NEMA_HOLD_STATUS,
    S_SET_NEMA_RELEASE_STATUS,
    S_SET_NEMA_WORKING_TIME,
    S_SET_NEMA_MAX,
    S_SET_NEMA_MIN,

    //SCHEDULER
    S_FETCH_DATA,
};

enum DataSignalsResponse {
    // WIFI
    S_WIFI_CONNECTED = 0,
    S_WIFI_NOT_CONNECTED = 1,
    S_WIFI_CONFIG_MODE = 2

    // NEMA17
};

#endif // DRIVERS_HPP