#include <ArduinoMqttClient.h>
#include <WiFiS3.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include <string.h>
#include <stdlib.h>
#include <DHT11.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Servo.h> 

#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

ArduinoLEDMatrix matrix;  // create an instance for led matrix.

// create an instance of BMP280 sensor, it will create an object called bmp280
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp280;

#define DHT11PIN 7      //DHT 11 sensor
DHT11 dht11(DHT11PIN);  // create an instance of dht11

#define servopin 3   // Pin 3 to servo signal pin
Servo myservo;  // create servo object to control a servo 
int pos = 0; // variable to store the servo position 

// bitmap frame buffer.
byte frame[8][12] = {
  { 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.3.109";
int port = 1883;

const char topic1[] = "uno/temp_in1";
const char topic2[] = "uno/humidity";
const char topic3[] = "uno/temp_in2";
const char topic4[] = "uno/pressure";
const char topic5[] = "uno/altitude";
const char topic6[] = "home/bedroom/switch1/set";
const char topic7[] = "home/bedroom/switch2/set";

// define buzzer pin
const int buzzer = 9;

// LED matrix drawText function
void drawText(const char text[]) {
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);

  // add the text
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}


void setup() {
  myservo.attach(servopin);  // attaches the servo on pin 3 to the servo object 
  myservo.write(pos);
   
  delay(100);
  pinMode(DHT11PIN, INPUT);
  delay(100);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);  // turn off buzzer

  // init matrix and setting renderbitmap.
  delay(100);
  matrix.begin();
  matrix.renderBitmap(frame, 8, 12);

  // init bmp280
  if (!bmp280.begin(BMP280_I2C_ADDRESS)){
    Serial.println("Could not find a valid BMP280 sensor, check wiring!!!");
    while(1);
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

  Serial.println("Sending available to topic!");

  // send message, the Print interface can be used to set the message contents
  mqttClient.beginMessage("home/bedroom/switch1/available");
  mqttClient.print("online");
  mqttClient.endMessage();
  delay(100);
  mqttClient.beginMessage("home/bedroom/switch2/available");
  mqttClient.print("online");
  mqttClient.endMessage();

  Serial.println();
  delay(100);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe("home/bedroom/switch1/set");
  mqttClient.subscribe("home/bedroom/switch2/set");

}

void onMqttMessage(int messageSize) {
  char command[3];
  int i = 0;
  if (mqttClient.messageTopic().equals("home/bedroom/switch1/set")) {
    while (mqttClient.available()) {
      command[i++] = (char)mqttClient.read();
    }
    if (command[0] == 'O' && command[1] == 'N') {
      Serial.println("Buzzer on");
      digitalWrite(buzzer, HIGH);
      drawText(" Buzzer ON!  ");
    } else if (command[0] == 'O' && command[1] == 'F' && command[2] == 'F') {
      Serial.println("Buzzer off");
      digitalWrite(buzzer, LOW);
      drawText(" Buzzer OFF!  ");
    }
  } 
  if (mqttClient.messageTopic().equals("home/bedroom/switch2/set")) {
    while (mqttClient.available()) {
      command[i++] = (char)mqttClient.read();
    }
    if (command[0] == '0' && command[1] == 'A') {
      Serial.println("Servo ON");
      myservo.write(90);
      drawText("  Servo ON!  ");
    } else if (command[0] == '1' && command[1] == 'F' && command[2] == 'F') {
      Serial.println("Servo OFF");
      myservo.write(0);
      drawText(" Servo OFF!  ");
    }
  }
}


int delay_cnt = 0;

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  delay(1);
  delay_cnt++;
  if (delay_cnt == 100) {
    // read data from DHT11 sensor
    float humidity = dht11.readHumidity();
    float temperature = dht11.readTemperature();
    Serial.print(temperature);
    Serial.print("\t");
    Serial.print(humidity);
    Serial.print("\t");
    mqttClient.beginMessage(topic1);
    mqttClient.print(temperature);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    mqttClient.print(humidity);
    mqttClient.endMessage();
  } else if (delay_cnt == 200) {
    // read temperature, pressure and altitude from BMP280 sensor
    float bmp_temp = bmp280.readTemperature();                    //
    float bmp_pressure = bmp280.readPressure() / 100.0;         //Read Pressure in Pa and conversion to inches of mercury;
    float bmp_altitude = bmp280.readAltitude(1013.25);  //Calculating the Altitude, the "1013.25"  is the pressure in(hPa) at sea level at day in your region
    Serial.print(bmp_temp);
    Serial.print("\t");
    Serial.print(bmp_pressure);
    Serial.print("\t");
    Serial.print(bmp_altitude);
    Serial.print("\t\t");
    mqttClient.beginMessage(topic3);
    mqttClient.print(bmp_temp);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic4);
    mqttClient.print(bmp_pressure);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic5);
    mqttClient.print(bmp_altitude);
    mqttClient.endMessage();
  } else if (delay_cnt == 300) {
    delay_cnt = 0;
  }
}
