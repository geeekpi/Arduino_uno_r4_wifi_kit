#include "WiFiS3.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#define DHT11_PIN 2    
DHT dht11(DHT11_PIN, DHT11);  // Create a DHT object

#define WLAN_SSID       "...your SSID..."
#define WLAN_PASS       "...your password..."

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883           // use 8883 for SSL
#define AIO_USERNAME    "...your AIO username (see 
https://accounts.adafruit.com)..."
#define AIO_KEY         "...your AIO key..."

// Create an WiFiClient class to connect to the MQTT server.
WiFiClient wifiClient;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&wifiClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");

void setup() {
  // Create serial connection and wait for it to become available.
  Serial.begin(9600);
  dht11.begin();  // Initialize the DHT sensor
  while (!Serial) {
    ; 
  }

  // Connect to WiFi
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(WLAN_SSID);
  while (WiFi.begin(WLAN_SSID, WLAN_PASS) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println("You're connected to the network");
  Serial.println();
  Serial.println(WiFi.localIP());

}

void loop() {
  delay(5000);
  MQTT_connect();
  float humi  = dht11.readHumidity();
  // read temperature as Celsius
  float tempC = dht11.readTemperature();
  // read temperature as Fahrenheit
  
  Serial.print(F("\nSending temperature val "));
  if (! temperature.publish(tempC)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending humidity val "));
  if (! humidity.publish(humi)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
