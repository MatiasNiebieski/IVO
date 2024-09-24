#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>

// Configuración del sensor DHT
#define DHTPIN 4      // Pin de conexión del DHT11
#define DHTTYPE DHT11 // Tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Credenciales WiFi
const char* ssid = "IoTB";

const char* password = "inventaronelVAR";
const url = 'https://clima-argentina.p.rapidapi.com/clima/lavoz';
 const options = {
     method: 'GET',
     headers: {
         'x-rapidapi-key': '6b8d376642msh478a7d25a335229p10b0d3jsn30d1f69d2486',
         'x-rapidapi-host': 'clima-argentina.p.rapidapi.com'
     }
 };
 
 async function getWeatherData() {
     try {
         const response = await fetch(url, options);
         const result = await response.json(); // Cambia esto a .json() para obtener un objeto JSON
         return result;
     } catch (error) {
         console.error(error);
         return null; // Retorna null en caso de error
     }
 }


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
    int httpResponseCode = http.GET();
    
    if (httpResponseCode == 200) {
      String payload = http.getString();
      // Extraer datos JSON (puedes usar una librería como ArduinoJson para esto)
      int tempIndex = payload.indexOf("\"temp\":");
      int humIndex = payload.indexOf("\"humidity\":");
      
      if (tempIndex != -1 && humIndex != -1) {
        tempBA = payload.substring(tempIndex + 7, payload.indexOf(",", tempIndex)) + " °C";
        humBA = payload.substring(humIndex + 10, payload.indexOf(",", humIndex)) + " %";
      }
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
