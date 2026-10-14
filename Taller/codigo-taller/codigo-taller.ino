#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ======================================================
// CONFIGURACIÓN DE SENSORES
// ======================================================

// (Estudiante)
// Definir los pines y objetos necesarios para los sensores.

// Definimos el pin digital donde conectamos el sensor y el tipo de sensor
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const int PIN_LDR = 34;  // Solo input

// ======================================================
// CONFIGURACIÓN WIFI
// ======================================================

const char* WIFI_SSID = "ARTEFACTOS";
const char* WIFI_PASSWORD = "s^3sQ#9teskVgnb";


// ======================================================
// CONFIGURACIÓN MQTT
// ======================================================


const char* MQTT_SERVER = "10.110.0.22"; //  -- 192.168.0.15
const int MQTT_PORT = 1883;

// Topics utilizados por el dashboard React.
const char* TOPIC_TEMPERATURA = "clima/esp32/temperatura";
const char* TOPIC_HUMEDAD = "clima/esp32/humedad";
const char* TOPIC_LUZ = "clima/esp32/luz";


WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Enviar una lectura cada 2 segundos.
const unsigned long INTERVALO_ENVIO = 2000;

unsigned long ultimoEnvio = 0;

void conectarWiFi() {

  Serial.print("Conectando a WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");

  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Conectando a Mosquitto...");

    // Crear un Client ID diferente para este ESP32.
    String clientId = "ESP32-";
    clientId += String(
      (uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF),
      HEX);

    if (mqttClient.connect(clientId.c_str())) {

      Serial.println(" conectado");

    } else {

      Serial.print(" error, rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Reintentando en 2 segundos...");

      delay(2000);
    }
  }
}

// ======================================================
// Funciones para los sensores
// ======================================================

int medirLuz() {
  int valorADC = analogRead(PIN_LDR);
  //Serial.print("LDR ADC: ");
  //Serial.print(valorADC);
  return map(valorADC, 0, 4095, 0, 100);
}

// ======================================================
// LEER Y PUBLICAR SENSORES
// ======================================================

void publicarSensores() {

  // 1. Obtener la temperatura.
  float temperatura = dht.readTemperature();

  // 2. Obtener la humedad.
  float humedad = dht.readHumidity();

  // 3. Obtener la lectura del sensor de iluminación.
  int luz = medirLuz();

  // 4. Preparar los valores para poder publicarlos mediante MQTT.
  String tempStr = String(temperatura, 1);
  String humStr = String(humedad, 1);
  String luzStr = String(luz);

  // 5. Publicar cada dato en su topic correspondiente:

  // TOPIC_TEMPERATURA
  mqttClient.publish(TOPIC_TEMPERATURA, tempStr.c_str());
  // TOPIC_HUMEDAD
  mqttClient.publish(TOPIC_HUMEDAD, humStr.c_str());
  // TOPIC_LUZ
  mqttClient.publish(TOPIC_LUZ, luzStr.c_str());

  // 6. Mostrar las mediciones en el Monitor Serial.
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");

  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println(" %");

  Serial.print("Luz: ");
  Serial.print(luz);
  Serial.println(" %");
  Serial.println("-------------------------");
}

void setup() {

  Serial.begin(115200);
  // (Estudiante)
  // Inicializar los sensores y configurar los pines necesarios.
  dht.begin();
  //analogSetPinAttenuation(PIN_LDR, ADC_11db);
  conectarWiFi();

  mqttClient.setServer(
    MQTT_SERVER,
    MQTT_PORT);
}

void loop() {

  // Si se pierde WiFi, volver a conectarse.
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  // Si se pierde MQTT, volver a conectarse.
  if (!mqttClient.connected()) {
    conectarMQTT();
  }

  mqttClient.loop();


  // Publicar sensores cada 2 segundos.
  if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {

    ultimoEnvio = millis();

    publicarSensores();
  }
}
