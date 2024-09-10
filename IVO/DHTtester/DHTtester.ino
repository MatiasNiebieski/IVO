
#include <WiFi.h>
#include <WebServer.h>

// Configura tus credenciales Wi-Fi
const char* ssid = "IoTB";  // Cambia esto por tu SSID
const char* password = "inventaronelVAR";  // Cambia esto por tu contraseña

WebServer server(80);  // Servidor web corriendo en el puerto 80
const int relayPin = 5;  // Pin de control del relé

void handleRoot() {
  String htmlPage = R"=====(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Control de Relé con ESP32</title>
        <style>
            body {font-family: Arial, sans-serif; text-align: center; margin-top: 50px;}
            button {padding: 15px 30px; font-size: 16px; cursor: pointer;}
            a {text-decoration: none; color: white;}
        </style>
    </head>
    <body>
        <h1>Control de Relé con ESP32</h1>
        <button style="background-color: green;">
            <a href="/relayOn">Encender Relé</a>
        </button>
        <button style="background-color: red;">
            <a href="/relayOff">Apagar Relé</a>
        </button>
    </body>
    </html>
  )=====";
  server.send(200, "text/html", htmlPage);
}

void handleRelayOn() {
  digitalWrite(relayPin, HIGH);  // Enciende el relé
  server.send(200, "text/html", "Relé Encendido. <a href='/'>Volver</a>");  // Responde al cliente
}

void handleRelayOff() {
  digitalWrite(relayPin, LOW);  // Apaga el relé
  server.send(200, "text/html", "Relé Apagado. <a href='/'>Volver</a>");  // Responde al cliente
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Asegura que el relé esté apagado al inicio

  // Conecta el ESP32 a la red Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Conectando a Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado a la red Wi-Fi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configura las rutas del servidor web
  server.on("/", handleRoot);
  server.on("/relayOn", handleRelayOn);
  server.on("/relayOff", handleRelayOff);

  server.begin();  // Inicia el servidor
  Serial.println("Servidor web iniciado");
}

void loop() {
  server.handleClient();  // Maneja las solicitudes del cliente
}
