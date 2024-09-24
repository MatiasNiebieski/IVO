#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Librería para manejar JSON

// Configuración del sensor DHT
#define DHTPIN 4      // Pin de conexión del DHT11
#define DHTTYPE DHT11 // Tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Credenciales WiFi
const char* ssid = "IoTB";
const char* password = "inventaronelVAR";

// API de clima (RapidAPI)
const char* apiEndpoint = "https://clima-argentina.p.rapidapi.com/clima/lavoz";
const char* apiKey = "6b8d376642msh478a7d25a335229p10b0d3jsn30d1f69d2486";
const char* apiHost = "clima-argentina.p.rapidapi.com";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Conectar al WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado al WiFi");
  server.begin();
}

void loop() {
  // Leer los datos del DHT11
  float tempLocal = dht.readTemperature();
  float humLocal = dht.readHumidity();
  
  if (isnan(tempLocal) || isnan(humLocal)) {
    Serial.println("Error al leer del sensor DHT!");
    return;
  }

  // Conectar al cliente de la API del clima
  String tempBA = "--";
  String humBA = "--";
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(apiEndpoint);
    http.addHeader("x-rapidapi-key", apiKey);
    http.addHeader("x-rapidapi-host", apiHost);
    
    int httpResponseCode = http.GET();
    
    if (httpResponseCode == 200) {
      String payload = http.getString();
      Serial.println(payload); // Para ver la respuesta completa de la API

      // Usar ArduinoJson para analizar la respuesta JSON
      const size_t capacity = JSON_OBJECT_SIZE(3) + 160;
      DynamicJsonDocument doc(capacity);
      deserializeJson(doc, payload);

      // Extraer la temperatura y humedad de la respuesta
      tempBA = String(doc["clima"]["temperatura"]) + " °C";
      humBA = String(doc["clima"]["humedad"]) + " %";
      
    } else {
      Serial.println("Error en la petición HTTP");
    }
    http.end();
  }

  // Esperar por un cliente
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Cliente conectado");

    // Enviar el contenido de la web
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<html><body><h1>Datos del sensor DHT11 y clima de Buenos Aires</h1>");
    client.println("<p><strong>Temperatura Local:</strong> " + String(tempLocal) + " °C</p>");
    client.println("<p><strong>Humedad Local:</strong> " + String(humLocal) + " %</p>");
    client.println("<h2>Datos de Buenos Aires (API)</h2>");
    client.println("<p><strong>Temperatura en Buenos Aires:</strong> " + tempBA + "</p>");
    client.println("<p><strong>Humedad en Buenos Aires:</strong> " + humBA + "</p>");
    client.println("</body></html>");
    client.stop();
  }
}
