#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include <Arduino_CAN.h>
#include <DHT11.h>

#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.3.109";
int port = 1883;

const char topic1[] = "can/temperature";
const char topic2[] = "can/humidity";

void setup() {

  if (!CAN.begin(CanBitRate::BR_250k)) {
    Serial.println("CAN.begin(...) failed.");
    for (;;) {}
  }

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
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  delay(1000);
  if (CAN.available())
  {
    CanMsg const msg = CAN.read();
    Serial.print(msg.data[0]);
    Serial.print("\t");
    Serial.println(msg.data[1]);
  // read data from DHT11 sensor
  int temperature = (int) msg.data[0];
  int humidity = (int) msg.data[1];
  
  mqttClient.beginMessage(topic1);
  mqttClient.print(temperature);
  mqttClient.endMessage();

  mqttClient.beginMessage(topic2);
  mqttClient.print(humidity);
  mqttClient.endMessage();
  }
}
