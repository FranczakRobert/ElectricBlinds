#ifndef DRIVERS_HPP
#define DRIVERS_HPP

#define DRIVERS_NUMBER 3

enum Drivers {
    D_LED = 0,
    D_WIFI = 1,
    D_NEMA17 = 2,
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

    S_SET_NEMA_MAX,
    S_SET_NEMA_MIN,
};

enum DataSignalsResponse {
    // WIFI
    S_WIFI_CONNECTED = 0,
    S_WIFI_NOT_CONNECTED = 1,
    S_WIFI_CONFIG_MODE = 2

    // NEMA17
};

#endif // DRIVERS_HPP