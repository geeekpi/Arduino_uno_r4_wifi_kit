#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include <DHT11.h>
#include <SD.h> 
#include <SPI.h>

#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)
/* 
The circuit:
   DHT11 sensors on digital pin 7 
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 
*/

#define DHT11PIN 7
DHT11 dht11(DHT11PIN);
// define cs pin
const int chipSelect = 10;   // pin 10 - CS pin

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.3.109";
int port = 1883;

const char topic1[] = "can/temperature";
const char topic2[] = "can/humidity";

void setup() {
  pinMode(DHT11PIN, INPUT);
  // init serial port
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
   
  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(1000);
  }
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());
  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  mqttClient.setId("uno_R4_01");

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword("jacky", "mypassword");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1) {
      if (!mqttClient.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
      };
      delay(1000);
    }
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  delay(1000);
 
  // read data from dht11 
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();
  Serial.print("Temp: "); 
  Serial.print(temperature);
  Serial.print("\t humidity: ");
  Serial.println(humidity);
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    Serial.println("data logger begin...");
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.println(humidity);
    dataFile.close();
    Serial.println("data logger end...");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
   
  mqttClient.beginMessage(topic1);
  mqttClient.print(temperature);
  mqttClient.endMessage();

  mqttClient.beginMessage(topic2);
  mqttClient.print(humidity);
  mqttClient.endMessage();
}
