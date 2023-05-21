/*
Sorbonne University (M1 SESI)
Binome : Kavish RAGHUBAR et Weiyi GONG
Date : 21 Mai 2023
Enseignant : Franck Wajsburt
*/

const { readFileSync, writeFileSync } = require('fs');
const express = require('express');
const mqtt = require('mqtt');
const app = express();

/*
Function qui nous permet de lire le fichier text afin de récupérer
la valeur de la luminosité. 'luminosity.txt' sera modifié par le
client.py
dans le code html, que l'on envoie, on fera une mis à jour tout la seconde afin d'avoir
la valeur du capteur de luminosité (esp32) en temps réel. 
*/
function getLuminosityValue() {
  const data = readFileSync('luminosity.txt', 'utf8');
  const value = parseInt(data.trim(), 10);
  return value;
}


app.get('/', (req, res) => {
  const luminosityValue = getLuminosityValue();
  res.send(`
  <!DOCTYPE html>
  <html>
    <head>
      <title>Projet IOC</title>
      <link rel="stylesheet" type="text/css" href="/style.css">
      <style>
        h1 {
          font-size: 48px;
          text-align: center;
        }
        
        .box {
          width: 150px;
          height: 50px;
          border: 1px solid black;
          text-align: center;
          line-height: 50px;
          font-size: 24px;
          margin: 10px auto;
        }
      </style>
      <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    </head>
    <body>
      <h1>IOC</h1>
      <form>
        <div style="text-align: center;">
            <p>Binome : Weiyi GONG, Kavish RAGHUBAR</p>
            <p>M1 SESI : 2022/2023</p>
          <label for="led-checkbox">Allumer la LED de l'ESP32:</label>
          <input type="checkbox" id="led-checkbox" name="led-checkbox"><br>
          
          <label for="blink-checkbox">Clignoter la LED de l'ESP32:</label>
          <input type="checkbox" id="blink-checkbox" name="blink-checkbox"><br>
          <div>
            <label for="blink-speed">Vitesse de clignotement:</label>
            <input type="range" id="blink-speed" name="blink-speed" min="2" max="10" step="2" oninput="updateBlinkSpeed()"><br>
            <div id="blink-speed-value"></div>
          </div>
          <br>
          <p><br>Nombre d'appui sur le bouton poussoir:</p>
          <div class="box" id="push-button-value">0</div>
          <br>
          <p><br>Veillez cliquer sur la musique que vous voulez jouer sur l'ESP32:</p>
            <button onclick="playMusic('mario')">Mario</button>
            <button onclick="playMusic('got')">Game of Thrones</button>
            <button onclick="playMusic('harrypotter')">Harry Potter</button>
            <button onclick="playMusic('imperialmarch')">Imperial March Star Wars</button>

          <p><br>Valeur du capteur de luminosité:</p>
          <div class="box" id="light-sensor-value">${luminosityValue}</div>

          <h2>Graph de Capteur de Luminosité sur l'ESP32</h2>
          <canvas id="chart" style="width: 200px; height: 100px;"></canvas>

        </div>
  
      <script>
        function updateBlinkSpeed() {
          var blinkSpeed = document.getElementById("blink-speed").value;
          document.getElementById("blink-speed-value").textContent = "Vitesse actuelle: " + blinkSpeed + " Hz";
        }

        // JavaScript code
          function updateLuminosityValue() {
            fetch('/luminosityValue')
              .then(response => response.text())
              .then(value => {
                document.getElementById("light-sensor-value").textContent = value;
              });
          }

          setInterval(updateLuminosityValue, 1000); // Update every 1 second

        // Example data
        var data = [5, 15, 15, 80, 100, 50, 25, 30, 75];
        var chart = new Chart(document.getElementById('chart'), {
          type: 'line',
          data: {
            labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9'],
            datasets: [{
              label: 'Capteur de luminosité',
              backgroundColor: 'rgb(200, 50, 200)',
              borderColor: 'rgb(200 , 50, 200)',
              borderWidth: 2,
              data: data,
              fill: false
            }]
          },
          options: {
            responsive: true,
            maintainAspectRatio: true,
            aspectRatio: 5,
            title: {
              display: true,
              text: 'Capteur de luminosité'
            },
            scales: {
              yAxes: [{
                ticks: {
                  beginAtZero: true,
                  fontColor: 'black',
                  fontSize: 14,
                  fontStyle: 'bold'
                },
                scaleLabel: {
                  display: true,
                  labelString: 'Intensité (%)',
                  fontColor: 'black',
                  fontSize: 14,
                  fontStyle: 'bold'
                }
              }],
              xAxes: [{
                ticks: {
                  fontColor: 'black',
                  fontSize: 14,
                  fontStyle: 'bold'
                },
                scaleLabel: {
                  display: true,
                  labelString: 'Temps (mins)',
                  fontColor: 'black',
                  fontSize: 14,
                  fontStyle: 'bold'
                }
              }]
            }
          }
        });

  </script>
</form>

    </body>
  </html>
  
  `);
});

/*
 Il faut créer une fonction JS ici : playMusic, qui va jouer la musique sur l'ESP32 !
 tout ce qu'on fera, c'est en fonction du click de l'utilisateur, on écrira dans le fichier
 'music.txt' les valeurs : 1,2,3,4 pour chaque musique.
 avec le code client.py on lit ce fichier et en publish ce message
 le client esp32 sera abonné à ce topic_music */

//mis à jour de la vitesse du clignottement
function updateBlinkSpeed() {
    let speed = document.getElementById("blink-speed").value;
    let speedValue = "Fréquence : " + speed + "Hz";
    document.getElementById("blink-speed-value").innerHTML = speedValue;
  }
  

  app.get('/luminosityValue', (req, res) => {
    const value = getLuminosityValue();
    res.send(value.toString());
  });
  

app.get('/style.css', (req, res) => {
  res.sendFile(__dirname + "/style.css");
});

app.get('/background.jpg', (req, res) => {
  res.sendFile(__dirname + "/background.jpg");
});

app.listen(8000, () => console.log('Server raghubar_gong started !')); //here is what used to be displayed on the log upon server launch
