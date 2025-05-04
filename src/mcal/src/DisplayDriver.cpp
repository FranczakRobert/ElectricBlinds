#include "DisplayDriver.hpp"
#include "DriverManager.hpp"

DisplayDriver::DisplayDriver(DriverManager *driverManager)
:display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1)
{
    this->driverManager = driverManager;
    TAG = "LED";
}

DisplayDriver::~DisplayDriver() {

}

ErrorCode DisplayDriver::init() {
    // Serial.begin(115200);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println("SSD1306 allocation failed");
        for(;;);
    }
    delay(1000);
    display.setFont(&FreeSerif9pt7b);
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(WHITE);   
    display.setCursor(0, 35);
    display.println("HELLO");
    display.display();

    return E_OK;
}

ErrorCode DisplayDriver::deinit() {
    display.clearDisplay();
    return ErrorCode();
}

ErrorCode DisplayDriver::start() {
    return startThread(TAG,this,run);
}

ErrorCode DisplayDriver::stop() {
    return stopThread(TAG);
}

DataSignalsResponse DisplayDriver::getData(DataSignals)
{
    return DataSignalsResponse();
}

ErrorCode DisplayDriver::setData(DataSignals SIGNAL, uint16_t count, ...)
{
    switch (SIGNAL)
    {
        case S_OLED_WIFI_DISCONNECTED:
            pthread_mutex_lock(&mutex);
            displayState = OLED_SYSTEM_WIFI_DISCONNECTED;
            pthread_mutex_unlock(&mutex);
        break;

        case S_OLED_WIFI_CONNECTED:
            pthread_mutex_lock(&mutex);
            displayState = OLED_SYSTEM_WIFI_CONNECTED;
            pthread_mutex_unlock(&mutex);
        break;

        case S_OLED_SYSTEM_CONFIG_MODE:
            pthread_mutex_lock(&mutex);
            displayState = OLED_SYSTEM_CONFIG_STATE;
            pthread_mutex_unlock(&mutex);
        break;

        case S_OLED_SYSTEM_BOOT_STATE:
            pthread_mutex_lock(&mutex);
            displayState = OLED_SYSTEM_BOOT_STATE;
            pthread_mutex_unlock(&mutex);
        break;

        case S_OLED_SYSTEM_FULL_ACTIVE:
            pthread_mutex_lock(&mutex);
            displayState = OLED_SYSTEM_ACTIVE_STATE;
            pthread_mutex_unlock(&mutex);
        break;

        case S_SET_OLED_LOWERING:
            if(count > 0) {
                va_list args;
                va_start(args, count);
                low = String(va_arg(args, const char*));
                Serial.println(low);
                va_end(args);            
            }
        break;

        case S_SET_OLED_RAISING:
            if(count > 0) {
                va_list args;
                va_start(args, count);
                rais = String(va_arg(args, const char*));
                Serial.println(rais);
                va_end(args);            
            }
        break;

        case S_OLED_TIME_DISPLAY:
            pthread_mutex_lock(&mutex);
            displayState = OLED_SYSTEM_TIME_ACTIVE;
            pthread_mutex_unlock(&mutex);
        
        default:
            break;
    }

    return ErrorCode();
}


void *DisplayDriver::run(void *args) {
    DisplayDriver* self = static_cast<DisplayDriver*>(args);
    int8_t previousVal = -1;

    while (self->isRunning) {

        pthread_mutex_lock(&self->mutex);
        uint16_t displayState = self->displayState;
        pthread_mutex_unlock(&self->mutex);

        switch (displayState) {
            case OLED_SYSTEM_BOOT_STATE:
                previousVal = OLED_SYSTEM_BOOT_STATE;
                self->display.clearDisplay();
                self->display.setCursor(0, self->firstRow);
                self->display.println("System:booting");
                self->display.display();            
            break;

            case OLED_SYSTEM_CONFIG_STATE:
                previousVal = OLED_SYSTEM_CONFIG_STATE;
                self->display.clearDisplay();
                self->display.setCursor(0, self->firstRow);
                self->display.println("System: Config");
                self->display.setCursor(0, self->secondRow);
                self->display.println("WIFI: AP");
                self->display.display();
            break;

            case OLED_SYSTEM_ACTIVE_STATE:

                if(previousVal != OLED_SYSTEM_ACTIVE_STATE) {
                    vTaskDelay(10/portTICK_PERIOD_MS);
                    previousVal = OLED_SYSTEM_ACTIVE_STATE;
                    self->display.clearDisplay();
                    self->display.setCursor(0, self->firstRow);
                    self->display.println("System: Active");
                    self->display.display();
                }
            break;

            case OLED_SYSTEM_WIFI_DISCONNECTED:
                self->display.fillRect(0, self->secondRow - 10, SCREEN_WIDTH, 16, BLACK);
                self->display.setCursor(0, self->secondRow);
                self->display.println("wifi:disconected");
                self->display.display();
            break;

            case OLED_SYSTEM_WIFI_CONNECTED:
                self->display.fillRect(0, self->secondRow - 10, SCREEN_WIDTH, 16, BLACK);
                self->display.setCursor(0, self->secondRow);
                self->display.println("wifi:connected");
                self->display.display();
            break;

            case OLED_SYSTEM_TIME_ACTIVE:
            self->display.clearDisplay();
            self->display.setCursor(0, self->firstRow);
            self->display.println("low :  " + self->low);

            self->display.setCursor(0, self->secondRow);
            self->display.println("rais :  " + self->rais);

            self->display.setCursor(0, self->thirdRow);
            self->display.println("Wifi: ok Sys:ok");
            self->display.display();
            break;

            default:
                break;
        }
        vTaskDelay(500);
    }
    return nullptr;
}