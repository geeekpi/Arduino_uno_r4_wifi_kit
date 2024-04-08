#include "DHT.h"
#define DHT11_PIN 2    // Define the pin used to connect the sensor

DHT dht11(DHT11_PIN, DHT11);  // Create a DHT object

void setup() {
  // initialize the sensor
  Serial.begin(9600);
  dht11.begin();  // Initialize the DHT sensor
}

void loop() {
  // wait a few seconds between measurements.
  delay(1000);
  // read humidity
  float humi  = dht11.readHumidity();
  // read temperature as Celsius
  float tempC = dht11.readTemperature();
  // read temperature as Fahrenheit
  float tempF = dht11.readTemperature(true);
  // check if any reads failed
  if (isnan(humi) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT11 sensor!");
  } else {
    // Print the humidity, temperature to the serial monitor
    Serial.print(" Humidity: ");
    Serial.print(humi);
    Serial.print("%");
    Serial.print("   "); 
    Serial.print(" Temperature: ");
    Serial.print(tempC);
    Serial.print("°C");
    Serial.print("   "); 
    Serial.print(" Fahrenheit: ");
    Serial.print(tempF);
    Serial.println("°F");
  }
}
