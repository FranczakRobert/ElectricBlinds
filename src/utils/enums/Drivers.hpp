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

    //LED
    S_SET_WIFI_LED_OFF = 1,

    //NEMA17
    S_SET_NEMA_UP_STATUS =2,
    S_SET_NEMA_DOWN_STATUS = 3,

    S_SET_NEMA_HOLD_STATUS = 4,
    S_SET_NEMA_RELEASE_STATUS = 5,

};

enum DataSignalsResponse {
    // WIFI
    S_WIFI_CONNECTED = 0,
    S_WIFI_NOT_CONNECTED = 1

    // NEMA17
};

#endif // DRIVERS_HPP