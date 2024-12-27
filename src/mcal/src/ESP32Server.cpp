#include "ESP32Server.hpp"
#include <WebServer.h>
#include "ESP32Server.hpp"
#include <ArduinoJson.h>

WebServer ESP32Server::server(8080);
ESP32Server* ESP32Server::instance = nullptr;

ESP32Server::ESP32Server() {
  
  /* SINGLETON */
}

ESP32Server* ESP32Server::GetInstance() {
  if(instance == nullptr) {
    instance = new ESP32Server();
  }
  return instance;
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
          #info {
              font-size: 18px;
              margin-top: 20px;
              color: #ccc; /* Jasnoszary kolor dla tekstu informacji */
          }
      </style>
  </head>
  <body>

    <h1>ESP32 Arrow Key Control</h1>

    <button id="upButton" class="button">UP</button>
    <button id="downButton" class="button">DOWN</button>

    <p id="info">Press and hold the buttons to send signals.</p>

    <script>
    let upButton = document.getElementById('upButton');
    let downButton = document.getElementById('downButton');

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

    function sendSignal(name, val) {
        console.log(`Sending signal: ${name} with value: ${val}`); // Log dla debugowania
        fetch('/LED', {
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
        .then(data => {
            console.log(data);
            document.getElementById('info').innerText = `Last action: ${name} ${val === 1 ? 'PRESSED' : 'RELEASED'}`;
        })
        .catch(error => console.error('Error:', error));
    }
    </script>
  </body>
  </html>)rawliteral";
  ESP32Server::server.send(200, "text/html", html);
}


void ESP32Server::handlePost() {
  if (ESP32Server::server.hasArg("plain")) {
    JsonDocument doc;
    String body = ESP32Server::server.arg("plain");

    deserializeJson(doc, body);
    struct MotorStatus motorStatus;

    const char* name = doc["name"];
    uint8_t val = doc["val"];
    motorStatus.direction = strcmp(name, "ArrowUp") == 0 ? 1u : 0u;
    motorStatus.status = val == 1 ? 1u : 0u;

    ESP32Server::GetInstance()->driverManager->setMotorStatus(motorStatus);
  }
  
  ESP32Server::server.send(200, "text/html", "<html><body><h1>GPIO zmienione</h1></body></html>");
}

ErrorCode ESP32Server::init() {
  ESP32Server::server.on("/", HTTP_GET, handleRoot);
  ESP32Server::server.on("/LED", HTTP_POST, handlePost);
  ESP32Server::server.begin(8080);
  return E_OK;
}

ErrorCode ESP32Server::deinit() {
  
  return E_OK;
}

ErrorCode ESP32Server::start(){
  ESP32Server::GetInstance()->isRunning = 1;

  if(E_OK == init()) {
    if( 0 == pthread_create(&ptid, NULL, &ESP32Server::run, this)) {
      Serial.println("[Server][start] - OK");
      return E_OK;
    }
  }
  Serial.println("[Server][start] - ERROR");
  return E_NOT_OK;
}

ErrorCode ESP32Server::stop() {
  ESP32Server::GetInstance()->isRunning = 0;
  if(0 == pthread_join(ptid, NULL)) {
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
  while(ESP32Server::GetInstance()->isRunning) {
    ESP32Server::server.handleClient();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  return nullptr;
}
