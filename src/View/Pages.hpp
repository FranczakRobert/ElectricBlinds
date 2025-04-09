#ifndef PAGES
#define PAGES
#include "Arduino.h"

const String mainPage = R"rawliteral(
   <!DOCTYPE html>
<html lang="pl">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>Panel Sterowania</title>
  <style>
    :root {
      --primary-color: #ffffff;
      --background-color: #f0f4f8;
      --accent-color: #4a90e2;
      --gold-color: #ffce00;
      --success-color: #32cd32;
      --danger-color: #ff3b30;
      --card-bg: #ffffff;
      --text-color: #333333;
    }

    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background-color: var(--background-color);
      color: var(--text-color);
      margin: 0;
      padding: 0;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
    }

    h1 {
      margin: 30px 0 10px;
      font-size: 2rem;
      color: var(--accent-color);
    }

    .container {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
      gap: 20px;
      max-width: 1000px;
      width: 90%;
      margin: 20px auto;
    }
    @media (max-width: 768px) {
      .container {
        grid-template-columns: 1fr;
        gap: 30px; 
        padding: 0 10px;
      }
    }

    .card {
      background-color: var(--card-bg);
      border-radius: 16px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      padding: 20px;
      text-align: center;
    }

    .button {
      display: block;
      width: 100%;
      padding: 15px 25px;
      font-size: 16px;
      font-weight: bold;
      border: none;
      border-radius: 10px;
      cursor: pointer;
      margin: 10px 0;
      transition: background-color 0.3s ease;
    }

    .button.up {
      background-color: var(--gold-color);
      color: #000;
    }

    .button.down {
      background-color: var(--gold-color);
      color: #000;

    }
    .button-group-vertical {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      width: 50%;
      margin: 0 auto;
      display: block;
    }


    .button.set {
      background-color: var(--success-color);
      color: #fff;
      margin
    }

    .button.reset {
      background-color: var(--danger-color);
      color: #fff;
    }

    .button.max,
    .button.min {
      background-color: var(--accent-color);
      color: #fff;
    }

    .button:hover {
      opacity: 0.9;
    }

    .input-group {
      margin-top: 10px;
    }

    .input-group label {
      font-size: 16px;
      display: block;
      margin-bottom: 6px;
    }
    .input-group input {
      padding: 10px;
      font-size: 16px;
      border-radius: 8px;
      border: 1px solid #ccc;
      width: 50%;
      max-width: 250px;
      margin: 0 auto;
      display: block;
    }

    .note {
      font-size: 14px;
      margin-top: 10px;
      color: #777;
    }

    #maxValue {
      font-weight: bold;
      margin-top: 15px;
    }
  </style>
</head>
<body>
  <h1>Panel Sterowania</h1>

  <div class="container">
    <div class="card">
      <h2>Sterowanie ręczne</h2>
      <div class="button-group-vertical">
        <button id="upButton" class="button up">UP</button>
        <button id="downButton" class="button down">DOWN</button>
      </div>
    </div>

    <div class="card">
      <h2>Ustawienia czasowe</h2>
      <div class="input-group">
        <label for="loweringTime">Opuszczanie</label>
        <input type="time" id="loweringTime" />
      </div>
      <div class="input-group">
        <label for="raisingTime">Podnoszenie</label>
        <input type="time" id="raisingTime" />
      </div>
      <div class="note">Ustaw taki sam czas, jeżeli chcesz wyłączyć automatyzację.</div>
      <button id="setButton" class="button set">SET</button>
    </div>

    <div class="card">
      <h2>Reset & Pozycje</h2>
      <button id="MAX" class="button max">MAX UP</button>
      <button id="MIN" class="button min">MAX DOWN</button>
      <button id="resetButton" class="button reset">RESET</button>

      <div id="maxValue">Długość rolety: Loading...</div>
    </div>
  </div>
  
<script>
    let upButton = document.getElementById('upButton');
    let downButton = document.getElementById('downButton');
    let setButton = document.getElementById('setButton');
    let setResetButton = document.getElementById('resetButton');
    let minButton = document.getElementById('MIN');
    let maxButton = document.getElementById('MAX');

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
    
    // Ładowanie czasu przy starcie strony
    window.addEventListener('DOMContentLoaded', () => {
        if (!timeData) {
            fetch('/timeUpdate')
                .then(response => response.json())
                .then(data => {
                    timeData = data;  // Przechowaj pobrane dane
                    document.getElementById('loweringTime').value = timeData.loweringTime;
                    document.getElementById('raisingTime').value = timeData.raisingTime;
                    document.getElementById('maxValue').textContent = `Max: ${timeData.max}`;
                })
                .catch(error => console.error('Error:', error));
        }
    });
    
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

    maxButton.addEventListener('click', function() {
            fetch('/max', {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json'
            },
        })
        .then(response => response.text())
        .catch(error => console.error('Error:', error));
    });

    minButton.addEventListener('click', function() {
            fetch('/min', {
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
</html>
)rawliteral";


const String configPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pl">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8" />
  <title>Konfiguracja Rolety</title>
  <style>
    :root {
      --background-color: #f0f4f8;
      --card-bg: #ffffff;
      --text-color: #333;
      --accent-color: #4a90e2;
      --button-color: #32cd32;
      --button-hover: #28a428;
    }

    body {
      margin: 0;
      padding: 0;
      font-family: 'Segoe UI', sans-serif;
      background-color: var(--background-color);
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
    }

    h1 {
      margin-top: 30px;
      font-size: 2rem;
      color: var(--accent-color);
    }

    .card {
      background-color: var(--card-bg);
      padding: 30px 20px;
      margin: 20px;
      border-radius: 16px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
      width: 90%;
      max-width: 400px;
      text-align: center;
    }

    form {
      display: flex;
      flex-direction: column;
    }

    label {
      text-align: left;
      font-weight: bold;
      margin-bottom: 5px;
      color: var(--text-color);
    }

    input {
      padding: 12px;
      font-size: 16px;
      margin-bottom: 15px;
      border-radius: 8px;
      border: 1px solid #ccc;
    }

    button {
      background-color: var(--button-color);
      color: white;
      border: none;
      padding: 12px;
      font-size: 16px;
      font-weight: bold;
      border-radius: 8px;
      cursor: pointer;
      transition: background-color 0.3s ease;
    }

    button:hover {
      background-color: var(--button-hover);
    }
  </style>
</head>
<body>

  <h1>Konfiguracja Rolety</h1>

  <div class="card">
    <form id="wifiForm">
      <label for="ssid">Nazwa sieci WiFi:</label>
      <input type="text" id="ssid" name="ssid" required>

      <label for="password">Hasło:</label>
      <input type="password" id="password" name="password" required>

      <label for="max">Max:</label>
      <input type="number" id="max" name="max" required>

      <button type="button" onclick="sendData()">Ustaw</button>
    </form>
  </div>

  <script>
    function sendData() {
      var ssid = document.getElementById('ssid').value;
      var password = document.getElementById('password').value;
      var max = document.getElementById('max').value;

      var jsonData = JSON.stringify({
        ssid: ssid,
        password: password,
        max: parseInt(max, 10)
      });

      var xhr = new XMLHttpRequest();
      xhr.open('POST', '/submit', true);
      xhr.setRequestHeader('Content-Type', 'application/json');
      xhr.send(jsonData);

      alert('Dane zapisane! Poczekaj do 10 sekund połączenie.');

      document.getElementById('ssid').value = '';
      document.getElementById('password').value = '';
      document.getElementById('max').value = '';
    }
  </script>

</body>
</html>
)rawliteral";


#endif //PAGES