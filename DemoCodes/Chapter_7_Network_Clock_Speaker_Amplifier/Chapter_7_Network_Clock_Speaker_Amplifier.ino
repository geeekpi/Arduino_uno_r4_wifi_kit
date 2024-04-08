// Include the RTC library
#include "RTC.h"
//Include the NTP library
#include <NTPClient.h>
#include <WiFiS3.h>

#include <WiFiUdp.h>
#include "arduino_secrets.h" 
#include "analogWave.h"
analogWave wave(DAC);

#define NOTE_A4 69
#define FREQ_A4 440

int tonic = 65;

int melody[] = {
  1,3,5,1,
  1,3,5,1,
  5,6,8,
  5,6,8,
  8,10,8,6,5,1,
  8,10,8,6,5,1,
  1, -4, 1,
  1, -4, 1,
};

int rhythm[] {
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 2,
  4, 4, 2, 
  8, 8, 8, 8, 4, 4,
  8, 8, 8, 8, 4, 4,
  4, 4, 2,
  4, 4, 2
};

int noteCounter = 0;
int bpm = 120;
float beatDuration = 60.0 / bpm * 1000;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);

int flag=64;
void alarmCallback(){
  while(flag){
  int currentNote = melody[noteCounter] + tonic;
  float frequncy = FREQ_A4 * pow(2, ((currentNote - NOTE_A4) / 12.0));
  float noteDuration = beatDuration * (4.0 / rhythm[noteCounter]);
  wave.freq(frequncy);
  delay(noteDuration * 0.85);
  wave.stop();
  delay(noteDuration * 0.15);
  noteCounter++;
  noteCounter = noteCounter % 32;
  flag--;
  delay(1);
  if(flag<=1)wave.stop();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectToWiFi(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  printWifiStatus();
}

void setup(){
  Serial.begin(9600);
  while (!Serial);
  wave.sine(10);
  wave.stop();
  connectToWiFi();
  RTC.begin();
  Serial.println("\nStarting connection to server...");
  timeClient.begin();
  timeClient.update();

  // Get the current date and time from an NTP server and convert
  // it to CN +8 by passing the time zone offset in hours.
  // You may change the time zone offset to your local one.
  auto timeZoneOffsetHours = 8;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  // Trigger the alarm every time the minite are zero
  RTCTime alarmTime;
  alarmTime.setMinute(0); /* from 0 to 59 */
  // Make sure to only match on the minite in this example - not on any other parts of the date/time
  AlarmMatch matchTime;
  matchTime.addMatchMinute();
  //sets the alarm callback
  RTC.setAlarmCallback(alarmCallback, alarmTime, matchTime);
}

void loop(){
  delay(1000);//ever 1s loop run once
  
  // Retrieve the date and time from the RTC and print them
  RTCTime currentTime;
  RTC.getTime(currentTime); 
  Serial.println("The RTC was just set to: " + String(currentTime));
}
