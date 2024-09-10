#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "IoTB"; // Cambia esto por el nombre de tu red Wi-Fi
const char* password = "inventaronelVAR"; // Cambia esto por la contraseña de tu red

WebServer server(80); // Creamos un servidor web en el puerto 80
const int relayPin = 5; // Pin donde está conectado el relé

bool relayState = false; // Estado del relé

void handleRoot() {
  String htmlPage = R"=====(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Control de Relé</title>
        <style>
            body {font-family: Arial, sans-serif; text-align: center; margin-top: 50px;}
            button {padding: 15px 30px; font-size: 16px; cursor: pointer;}
        </style>
    </head>
    <body>
        <h1>Control de Relé con ESP32</h1>
        <button id="relayButton" onclick="toggleRelay()">Encender Relé</button>

        <script>
            let relayStatus = false;

            function toggleRelay() {
                relayStatus = !relayStatus;
                document.getElementById("relayButton").innerHTML = relayStatus ? "Apagar Relé" : "Encender Relé";
                fetch("/toggleRelay").then(response => response.text()).then(data => console.log(data));
            }
        </script>
    </body>
    </html>
  )=====";
  server.send(200, "text/html", htmlPage);
}

void handleToggleRelay() {
  relayState = !relayState;
  digitalWrite(relayPin, relayState ? HIGH : LOW);
  server.send(200, "text/plain", relayState ? "Relay ON" : "Relay OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Asegúrate de que el relé esté apagado inicialmente

  // Conexión a la red Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Conectando a Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado a la red Wi-Fi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configuración de rutas del servidor web
  server.on("/", handleRoot);
  server.on("/toggleRelay", handleToggleRelay);

  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient();
}
