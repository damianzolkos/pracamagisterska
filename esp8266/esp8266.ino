#include <Arduino.h>
#include "SdsDustSensor.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "DHTesp.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiManager.h>
WiFiManager wifiManager;

const char* serverAddress = "http://stacjapogodowa.prz.edu.pl/backend/receive.php"; // adres skryptu odbierającego dane

unsigned long zapamietanyCzas2 = 0;
uint8_t trybGrzalki, poziomWilgotnosci;

int airPeriod = 30;

SdsDustSensor sds(Serial);
DHTesp dht;

#define SEALEVELPRESSURE_HPA (1013.25)
#define grzalka D6
Adafruit_BME280 bme;
float pm25, pm10;
float temperatureBME, humidityBME, pressureBME, altitudeBME;
float temperatureDHT, humidityDHT;

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("IP w trybie AccesPoint: ");
  Serial.println(WiFi.softAPIP().toString());
}

void setup() {
  Serial.begin(9600);
  sds.begin();
  pinMode(grzalka, OUTPUT);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("Czujnik warunków pogodowych");

  Serial.println(sds.setActiveReportingMode().toString());                // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setCustomWorkingPeriod(30).toString());              // sensor sends data every 3 minutes
  dht.setup(D5, DHTesp::DHT22);                                           // Connect DHT sensor to GPIO 17

  bme.begin(0x76);
}

void loop() {
  delay(dht.getMinimumSamplingPeriod());

  humidityDHT = dht.getHumidity();
  temperatureDHT = dht.getTemperature();
  temperatureBME = bme.readTemperature();
  humidityBME = bme.readHumidity();
  pressureBME = bme.readPressure() / 100.0F;
  altitudeBME = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.println("");
  Serial.println("");
    Serial.println("----------------------------------------------------------------------");
  Serial.print("POMIAR ID: ");
  Serial.println(millis());
    Serial.println("----------------------------------------------------------------------");
  Serial.print("Temperatura BME: ");
  Serial.print(temperatureBME);
  Serial.println("");
  Serial.print("Wilgotnosc BME: ");
  Serial.print(humidityBME);
  Serial.println("");
  Serial.print("Ciśnienie BME: ");
  Serial.print(pressureBME);
  Serial.println("");
    Serial.println("----------------------------------------------------------------------");
  Serial.print("Wilgotnosc DHT: ");
  Serial.print(humidityDHT);
  Serial.println("");
  Serial.print("Temperatura DHT: ");
  Serial.print(temperatureDHT);
  Serial.println("");
  Serial.print("Stan grzalki: ");
  if (humidityDHT >= 55) {
    digitalWrite(grzalka, HIGH);
    Serial.print("ON");
  } else {
    digitalWrite(grzalka, LOW);
    Serial.print("OFF");
  }
  Serial.println("");
    Serial.println("----------------------------------------------------------------------");

  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    pm25 = pm.pm25;
    pm10 = pm.pm10;

    Serial.print("PM 2,5: ");
    Serial.print(pm25);
    Serial.println(" ug/m3 ");
    Serial.print("PM 10: ");
    Serial.print(pm10);
    Serial.println(" ug/m3 ");
    Serial.println("----------------------------------------------------------------------");

    String response;
    DynamicJsonDocument doc(1024);

    doc["temp"] = temperatureBME;
    doc["hum"] = humidityBME;
    doc["press"] = pressureBME;
    doc["pm25"] = pm25;
    doc["pm10"] = pm10;

    serializeJson(doc, response);

    HTTPClient http;                                              // Declare object of class HTTPClient
    http.begin(serverAddress);                                    // Specify request destination
    http.addHeader("Content-Type", "application/json");           // Specify content-type header
    http.addHeader("Access-Control-Allow-Origin", "*");
    int httpCode = http.POST(response);                           // Send the request
    String payload = http.getString();                            // Get the response payload
    http.end();                                                   // Close connection
  } else {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
    Serial.println("----------------------------------------------------------------------");
  }


  delay(5000);
}
