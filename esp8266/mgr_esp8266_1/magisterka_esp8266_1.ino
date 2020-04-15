#include "SdsDustSensor.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "DHTesp.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

const char* ssid = "krosoft_domowy_3615";
const char* password = "internecik55"; 

int airPeriod = 5;

SdsDustSensor sds(Serial);
DHTesp dht;

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;
float pm25, pm10;
float temperatureBME, humidityBME, pressureBME, altitudeBME;
float temperatureDHT, humidityDHT;

void setup() {
  Serial.begin(9600);
  sds.begin();

  Serial.println("");
  WiFi.begin(ssid, password);                                     // WiFi connection
  Serial.print("Waiting for connection...");
  while (WiFi.status() != WL_CONNECTED) {                         // Wait for the WiFI connection completion
    Serial.print(".");
    delay(500);
  }
  Serial.println("");

  Serial.println(sds.setActiveReportingMode().toString());                // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setCustomWorkingPeriod(airPeriod).toString());       // sensor sends data every 3 minutes
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

  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    
    pm25 = pm.pm25;
    pm10 = pm.pm10;

    String response;
    DynamicJsonDocument doc(1024);

    JsonObject air  = doc.createNestedObject("air");
    air["pm25"] = pm25;
    air["pm10"] = pm10;
    JsonObject dht22  = doc.createNestedObject("dht22");
    dht22["temperature"] = temperatureDHT;
    dht22["humidity"] = humidityDHT;
    JsonObject bme  = doc.createNestedObject("bme");
    bme["temperature"] = temperatureBME;
    bme["humidity"] = humidityBME;
    bme["pressure"] = pressureBME;
    bme["altitude"] = altitudeBME;

    HTTPClient http;                                              // Declare object of class HTTPClient
    http.begin("http://192.168.1.105:1880/smog");                 // Specify request destination
    http.addHeader("Content-Type", "text/plain");                 // Specify content-type header

    serializeJson(doc, response);
    int httpCode = http.POST(response);                           // Send the request
    String payload = http.getString();                            // Get the response payload
    // Serial.println(httpCode);                                  // Print HTTP return code
    http.end();                                                   // Close connection

  } else {
    // Serial.print("Could not read values from sensor, reason: ");
    // Serial.println(pm.statusToString());
  }
  delay(2000);
}
