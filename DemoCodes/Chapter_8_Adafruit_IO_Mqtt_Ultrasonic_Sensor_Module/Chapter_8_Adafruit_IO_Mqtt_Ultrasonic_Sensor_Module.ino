#include "WiFiS3.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "...your SSID..."
#define WLAN_PASS       "...your password..."

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883           // use 8883 for SSL
#define AIO_USERNAME    "...your AIO username (see 
https://accounts.adafruit.com)..."
#define AIO_KEY         "...your AIO key..."

int trigPin = 9;    // TRIG pin
int echoPin = 8;    // ECHO pin

float distance_cm,duration_us;uint32_t flag;
// Create an WiFiClient class to connect to the MQTT server.
WiFiClient wifiClient;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&wifiClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish distance = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/distance");

void setup() {
  // Create serial connection and wait for it to become available.
  Serial.begin(9600);
  // configure the trigger pin to output mode
  pinMode(trigPin, OUTPUT);
  // configure the echo pin to input mode
  pinMode(echoPin, INPUT);

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
  delay(2000);
  MQTT_connect();

  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(echoPin, HIGH);
  // calculate the distance
  distance_cm = 0.017 * duration_us;

  if(distance_cm > 30) flag = 0;
  else flag = 1;

  Serial.print(F("\nSending distance val "));
  if (! distance.publish(flag)) {  
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
