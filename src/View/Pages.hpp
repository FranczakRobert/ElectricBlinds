#ifndef PAGES
#define PAGES
#include "Arduino.h"

const String mainPage = R"rawliteral(
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

      <div id="maxValue">Max: Loading...</div>  

      <button id="setButton">SET</button>

      <button id="resetButton">TRIGGER RESET</button>
  
      <script>
  let upButton = document.getElementById('upButton');
  let downButton = document.getElementById('downButton');
  let setButton = document.getElementById('setButton');
  let setResetButton = document.getElementById('resetButton');
  
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
                  document.getElementById('maxValue').textContent = `Max: ${timeData.max}`;
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

  setResetButton.addEventListener('click', function() {
        fetch('/reset', {
          method: 'GET',
          headers: {
              'Content-Type': 'application/json'
          },
      })
      .then(response => response.text())
      .catch(error => console.error('Error:', error));
  });
      </script>
    </body>
    </html>)rawliteral";


const String configPage =
    "<!DOCTYPE html>"
    "<html lang='pl'><head>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<style> body { text-align: center; font-family: Arial; } "
    " input, button { font-size: 18px; padding: 10px; margin: 5px; width: 80%; }"
    " label { display: block; margin-bottom: 10px; } "
    " form { margin: 0 auto; width: 300px; } "
    "</style>"
    "</head><body>"
    "<h1>Konfiguracja Rolety</h1>"
    "<form id='wifiForm'>"
    "<label for='ssid'>Nazwa sieci WiFi:</label>"
    "<input type='text' id='ssid' name='ssid'><br>"
    "<label for='password'>Haslo:</label>"
    "<input type='password' id='password' name='password'><br>"
    "<label for='Max'>Max:</label>"
    "<input type='number' id='max' name='max'><br>"
    "<button type='button' onclick='sendData()'>Ustaw</button>"
    "</form>"
    "<script>"
    "function sendData() {"
    "  var ssid = document.getElementById('ssid').value;"
    "  var password = document.getElementById('password').value;"
    "  var max = document.getElementById('max').value;"
    "  var jsonData = JSON.stringify({ssid: ssid, password: password, max: parseInt(max,10)});"
    "  var xhr = new XMLHttpRequest();"
    "  xhr.open('POST', '/submit', true);"
    "  xhr.setRequestHeader('Content-Type', 'application/json');"
    "  xhr.send(jsonData);"
    "  alert('Dane zapisane! Poczekaj do 10 sekund połączenie.');"
    "  document.getElementById('ssid').value = '';"
    "  document.getElementById('password').value = '';"
    "  document.getElementById('max').value = '';"
    "}" 
    "</script>"
    "</body></html>";


#endif //PAGES