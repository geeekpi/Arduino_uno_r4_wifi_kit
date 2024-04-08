// Include the RTC library
#include "RTC.h"
//Include the NTP library
#include <NTPClient.h>
#if defined(ARDUINO_PORTENTA_C33)
#include <WiFiC3.h>
#elif defined(ARDUINO_UNOWIFIR4)
#include <WiFiS3.h>
#endif

#include <WiFiUdp.h>
#include "arduino_secrets.h" 
#include <LiquidCrystal_I2C.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

int length = 28; // the number of  notes
char notes[] = "GGAGcB GGAGdc GGxecBA yyecdc";
int beats[] = {2,2,8,8,8,16,1,2,2,8,8,8,16,1,2,2,8,8,8,8,16,1,2,2,8,8,8,16};
int  tempo = 200;// time delay between notes 

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int wifiStatus = WL_IDLE_STATUS;
int buzzer = 8; 
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);

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

  pinMode(buzzer, OUTPUT);

  lcd.init();        // initialize the lcd
  lcd.clear();       // clear the LCD display
  lcd.backlight();   // Make sure backlight is on
  connectToWiFi();
  RTC.begin();
  Serial.println("\nStarting connection to server...");
  timeClient.begin();
  timeClient.update();

  // Get the current date and time from an NTP server and convert
  // it to UTC +8 by passing the time zone offset in hours.
  // You may change the time zone offset to your local one.
  auto timeZoneOffsetHours = 8;
  auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
  Serial.print("Unix time = ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);

  // Trigger the alarm every time the minute are zero
  RTCTime alarmTime;
  alarmTime.setMinute(0);

  // Make sure to only match on the minute in this example - not on any other parts of the date/time
  AlarmMatch matchTime;
  matchTime.addMatchMinute();

  //sets the alarm callback
  RTC.setAlarmCallback(alarmCallback, alarmTime, matchTime);
}

void playTone(int tone, int duration)  {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
   digitalWrite(buzzer,  HIGH);
   delayMicroseconds(tone);
   digitalWrite(buzzer, LOW);
   delayMicroseconds(tone);
  }
}
void playNote(char note, int duration) {
  char names[] = {'C', 'D', 'E', 'F', 'G',  'A', 'B',           

                 'c', 'd', 'e', 'f', 'g', 'a', 'b',

                 'x', 'y' };

  int tones[] = { 1915, 1700, 1519, 1432, 1275,  1136, 1014,

                 956,  834,  765,  593,  468,  346,  224,

                 655 , 715 };

  int SPEE = 5;

  // play the tone corresponding  to the note name

  for (int i = 0; i < 17; i++) {

   if (names[i] ==  note) {
    int newduration = duration/SPEE;
     playTone(tones[i], newduration);
   }
  }
}

void loop(){
  delay(1000);//ever 1s loop run once

  // Retrieve the date and time from the RTC and print them
  RTCTime currentTime;
  RTC.getTime(currentTime); 
  Serial.println("The RTC was just set to: " + String(currentTime));
  
  lcd.setCursor(0, 0);  // start to print at the first row
  lcd.print(currentTime.getYear());
  lcd.print("-");                          
  lcd.print(Month2int(currentTime.getMonth()));
  lcd.print("-");
  lcd.print(currentTime.getDayOfMonth());

  lcd.setCursor(0, 1);   // start to print at the second row
  lcd.print(currentTime.getHour());
  lcd.print(":");
  lcd.print(currentTime.getMinutes());
  lcd.print(":");
  lcd.print(currentTime.getSeconds());
}

void alarmCallback(){
  for (int i = 0; i < length; i++) {
   if (notes[i] == ' ') {
      delay(beats[i] * tempo); // delay between notes
   } else {
     playNote(notes[i],   beats[i] * tempo);
   }
   // time delay between notes
   delay(tempo);
  }
}
