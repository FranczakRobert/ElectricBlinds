#include <WebServer.h>
#include "ESP32Server.hpp"
#include <ArduinoJson.h>
#include "NvmMemory.hpp"
#include "freertos/semphr.h"

#include <Preferences.h>
class Preferences;

struct payload {
  String loweringTime;
  String raisingTime;
};

WebServer ESP32Server::server(8080);
String ESP32Server::loweringTimeVal = "";
String ESP32Server::raisingTimeVal = "";
SemaphoreHandle_t nvmSemaphore;

ESP32Server::ESP32Server() {
  
  /* SINGLETON */
}

void ESP32Server::handleRoot()
{
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP32 Control</title>
      <style>
          body {
              font-family: Arial, sans-serif;
              text-align: center;
              background-color: #000; /* Czarne tło */
              color: #fff; /* Biały tekst */
              margin: 0;
              padding: 0;
              height: 100vh;
              display: flex;
              flex-direction: column;
              justify-content: center;
              align-items: center;
          }
          h1 {
              margin-bottom: 20px;
          }
          .button {
              display: inline-block;
              background-color: #FFD700; /* Złoty kolor */
              color: black;
              border: none;
              padding: 15px 30px;
              margin: 10px;
              font-size: 18px;
              border-radius: 5px;
              cursor: pointer;
              transition: background-color 0.3s ease;
              width: 150px; /* Stała szerokość dla obu przycisków */
              height: 50px; /* Stała wysokość dla obu przycisków */
              user-select: none;
              -webkit-user-select: none;
              -moz-user-select: none;
              -ms-user-select: none;
              touch-action: manipulation;
          }
          .button:hover {
              background-color: #FFCC00; /* Jaśniejszy złoty po najechaniu */
          }
          .button:active {
              background-color: #FFD700; /* Ciemniejszy złoty po kliknięciu */
          }
          .input-group {
              margin: 20px 0;
          }
          .input-group label {
              display: block;
              font-size: 18px;
              margin-bottom: 5px;
              color: #ccc;
          }
          .input-group input {
              padding: 10px;
              font-size: 16px;
              border-radius: 5px;
              border: none;
              width: 150px;
              text-align: center;
          }
          #setButton {
              background-color: #32CD32; /* Zielony kolor */
              color: #fff;
              border: none;
              padding: 15px 30px;
              font-size: 18px;
              border-radius: 5px;
              cursor: pointer;
              transition: background-color 0.3s ease;
              margin-top: 20px;
          }
          #setButton:hover {
              background-color: #28A428; /* Ciemniejszy zielony po najechaniu */
          }
      </style>
  </head>
  <body>

    <h1>ESP32 Arrow Key Control</h1>

    <button id="upButton" class="button">UP</button>
    <button id="downButton" class="button">DOWN</button>

    <div class="input-group">
        <label for="loweringTime">Lowering the blinds</label>
        <input type="time" id="loweringTime">
    </div>
    <div class="input-group">
        <label for="raisingTime">Raising the blinds</label>
        <input type="time" id="raisingTime" >
    </div>

    <button id="setButton">SET</button>

    <script>
let upButton = document.getElementById('upButton');
let downButton = document.getElementById('downButton');
let setButton = document.getElementById('setButton');

// Zmienna do przechowywania pobranych danych
let timeData = null;

let isUpActive = false;
let isDownActive = false;

// Obsługuje zarówno zdarzenia dla myszki jak i dotyku
upButton.addEventListener('mousedown', function() {
    sendSignal('ArrowUp', 1);
});
upButton.addEventListener('mouseup', function() {
    sendSignal('ArrowUp', 0);
});

downButton.addEventListener('mousedown', function() {
    sendSignal('ArrowDown', 1);
});
downButton.addEventListener('mouseup', function() {
    sendSignal('ArrowDown', 0);
});

// Obsługa dotyku z zapobieganiem wielokrotnym sygnałom
upButton.addEventListener('touchstart', function(event) {
    event.preventDefault();
    if (!isUpActive) {
        isUpActive = true;
        sendSignal('ArrowUp', 1);
    }
});
upButton.addEventListener('touchend', function(event) {
    event.preventDefault();
    if (isUpActive) {
        isUpActive = false;
        sendSignal('ArrowUp', 0);
    }
});

downButton.addEventListener('touchstart', function(event) {
    event.preventDefault();
    if (!isDownActive) {
        isDownActive = true;
        sendSignal('ArrowDown', 1);
    }
});
downButton.addEventListener('touchend', function(event) {
    event.preventDefault();
    if (isDownActive) {
        isDownActive = false;
        sendSignal('ArrowDown', 0);
    }
});

// Funkcja wysyłająca sygnały do serwera
function sendSignal(name, val) {
    fetch('/motorControll', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            name: name,
            val: val
        })
    })
    .then(response => response.text())
    .catch(error => console.error('Error:', error));
}

// Ładowanie czasu przy starcie strony (raz na załadowanie strony)
window.addEventListener('DOMContentLoaded', () => {
    if (!timeData) {  // Jeśli dane jeszcze nie zostały pobrane
        fetch('/timeUpdate')
            .then(response => response.json())
            .then(data => {
                timeData = data;  // Przechowaj pobrane dane
                // Ustawianie wartości z JSON na inputach
                document.getElementById('loweringTime').value = timeData.loweringTime;
                document.getElementById('raisingTime').value = timeData.raisingTime;
            })
            .catch(error => console.error('Error:', error));
    }
});

// Ustawianie czasu po kliknięciu przycisku
setButton.addEventListener('click', function() {
    let loweringTime = document.getElementById('loweringTime').value;
    let raisingTime = document.getElementById('raisingTime').value;

    fetch('/blindsTime', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            loweringTime: loweringTime,
            raisingTime: raisingTime
        })
    })
    .then(response => response.text())
    .catch(error => console.error('Error:', error));
});

    </script>
  </body>
  </html>)rawliteral";
  ESP32Server::server.send(200, "text/html", html);
}


void ESP32Server::handleMotorPost() {
  if (ESP32Server::server.hasArg("plain")) {
    JsonDocument doc;
    String body = ESP32Server::server.arg("plain");

    deserializeJson(doc, body);
    struct MotorStatus motorStatus;

    const char* name = doc["name"];
    uint8_t val = doc["val"];
    motorStatus.direction = strcmp(name, "ArrowUp") == 0 ? 1u : 0u;

    motorStatus.status = val == 1 ? 1u : 0u;

    ESP32Server::GetInstance().driverManager->setMotorStatus(motorStatus);
  }
  
  ESP32Server::server.send(200);
}


void writeToNVMTask(void* pvParameters) {
  
  struct payload payloadCopy = *(static_cast<struct payload*>(pvParameters));

  const char* loweringTimeStr = payloadCopy.loweringTime.c_str();
  const char* raisingTimeStr = payloadCopy.raisingTime.c_str();

  NvmMemory::getInstance().writeToNvm("TIME", "L", loweringTimeStr);
  NvmMemory::getInstance().writeToNvm("TIME", "R", raisingTimeStr);

  vTaskDelay(100 / portTICK_PERIOD_MS);
  vTaskDelete(NULL);
}

void ESP32Server::handleBlindsTimerPost() {
  if (ESP32Server::server.hasArg("plain")) {
    JsonDocument doc;
    String body = ESP32Server::server.arg("plain");

    deserializeJson(doc, body);

    const char* loweringTime = doc["loweringTime"];
    const char* raisingTime = doc["raisingTime"];

    static struct payload taskPayload;
    taskPayload.loweringTime = String(loweringTime);
    taskPayload.raisingTime = String(raisingTime);

    xTaskCreate(writeToNVMTask, "WriteToNVMTask", 2048, (void*)&taskPayload, 1, NULL);
  }
}

void readFromNVMTask(void* pvParameters) {
  ESP32Server* server = static_cast<ESP32Server*>(pvParameters);

  server->GetInstance().loweringTimeVal = NvmMemory::getInstance().readFromNvm("TIME", "L");
  server->GetInstance().raisingTimeVal = NvmMemory::getInstance().readFromNvm("TIME", "R");

  xSemaphoreGive(nvmSemaphore);

  vTaskDelete(NULL);
}

void ESP32Server::setRandLTimers() {
  nvmSemaphore = xSemaphoreCreateBinary();

  if (&ESP32Server::server == nullptr) {
    Serial.println("ERROR: Server is not initialized.");
    return;
  }

  xTaskCreate(readFromNVMTask, "ReadFromNVMTask", 2048, (void*)&ESP32Server::GetInstance(), 1, NULL);

  if(pdTRUE == xSemaphoreTake(nvmSemaphore, portMAX_DELAY)) {
    JsonDocument doc;
    doc["loweringTime"] = ESP32Server::GetInstance().loweringTimeVal.isEmpty() ? "00:00" : ESP32Server::GetInstance().loweringTimeVal;
    doc["raisingTime"] = ESP32Server::GetInstance().raisingTimeVal.isEmpty() ? "00:00" : ESP32Server::GetInstance().raisingTimeVal;

    String response;
    serializeJson(doc, response);

    ESP32Server::GetInstance().server.send(200, "application/json", response);
  }

  vSemaphoreDelete(nvmSemaphore);
}


ErrorCode ESP32Server::init() {
  ESP32Server::server.on("/", HTTP_GET, handleRoot);
  ESP32Server::server.on("/timeUpdate", HTTP_GET, setRandLTimers);
  ESP32Server::server.on("/motorControll", HTTP_POST, handleMotorPost);
  ESP32Server::server.on("/blindsTime", HTTP_POST, handleBlindsTimerPost);
  ESP32Server::server.begin(8080);
  return E_OK;
}

ErrorCode ESP32Server::deinit() {
  
  return E_OK;
}

ErrorCode ESP32Server::start(){
  ESP32Server::GetInstance().isRunning = 1;

  if(E_OK == init()) {
    pthread_attr_t attr;
    size_t stack_size = 8000;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stack_size);

    if( E_OK == startThread(this,run)) {
      Serial.println("[Server][start] - OK");
      return E_OK;
    }
  }
  Serial.println("[Server][start] - ERROR");
  return E_NOT_OK;
}

ErrorCode ESP32Server::stop() {
  ESP32Server::GetInstance().isRunning = 0;
  if(E_OK == stopThread()) {
    Serial.println("[Server][stop]- OK");
    return E_OK;
  }
  Serial.println("[Server][stop] - ERROR");
  return E_OK;
}

ErrorCode ESP32Server::setManager(DriverManager *drMg) {
  ESP32Server::driverManager = drMg;
  return ErrorCode();
}

void* ESP32Server::run(void* args) {
  while(ESP32Server::GetInstance().isRunning) {
    ESP32Server::server.handleClient();    
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  return nullptr;
}
