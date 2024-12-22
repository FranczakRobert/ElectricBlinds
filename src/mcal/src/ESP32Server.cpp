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

    // Obsługuje zarówno zdarzenia dla myszki jak i dotyku
    upButton.addEventListener('mousedown', function() {
        sendSignal('ArrowUp', 1);
    });
    upButton.addEventListener('mouseup', function() {
        sendSignal('ArrowUp', 0);
    });
    upButton.addEventListener('touchstart', function() {
        sendSignal('ArrowUp', 1);
    });
    upButton.addEventListener('touchend', function() {
        sendSignal('ArrowUp', 0);
    });

    downButton.addEventListener('mousedown', function() {
        sendSignal('ArrowDown', 1);
    });
    downButton.addEventListener('mouseup', function() {
        sendSignal('ArrowDown', 0);
    });
    downButton.addEventListener('touchstart', function() {
        sendSignal('ArrowDown', 1);
    });
    downButton.addEventListener('touchend', function() {
        sendSignal('ArrowDown', 0);
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

    const char* name = doc["name"];
    int val = doc["val"];

    if (strcmp(name, "ArrowUp") == 0) {
      if (val == 1) {
        Serial.println("Arrow Up PRESSED");
      } else {
        Serial.println("Arrow Up RELEASED");
      }
    } else if (strcmp(name, "ArrowDown") == 0) {
      if (val == 1) {
        Serial.println("Arrow Down PRESSED");
      } else {
        Serial.println("Arrow Down RELEASED");
      }
    } else {
      Serial.println("Unknown action received");
    }
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
  ESP32Server::server.stop();
  return E_OK;
}

ErrorCode ESP32Server::start(){

  if(E_OK == init()) {
    vTaskDelay(100);
    if(pdPASS == xTaskCreate(runWrapper, "Server", 2048, NULL, 1, &xHandle)) {
      Serial.println("[ESP32Server] Started");
      return E_OK;
    }  
  }
  return E_NOT_OK;
}

ErrorCode ESP32Server::stop() {
  if(eTaskGetState(xHandle) == eRunning) {
    vTaskDelete(xHandle);
    if(E_OK == deinit()) {
      return E_OK;
    }
  }
  
  return E_NOT_OK;
}

void ESP32Server::runWrapper(void *params) {
  ESP32Server::GetInstance()->run();
}

void ESP32Server::run() {
  while(1) {
    ESP32Server::server.handleClient();
    vTaskDelay(1);
  }
}
