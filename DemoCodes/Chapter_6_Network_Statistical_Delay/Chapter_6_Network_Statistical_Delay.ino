#include "WiFiS3.h"
#include "arduino_secrets.h" 
#define LED_PIN 2
#define LED2_PIN 3
#define LED3_PIN 4
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
char server[] = "www.github.com";    // name address for github (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
unsigned long myTime1; 
unsigned long myTime2;
unsigned long myTime3;
/* ------------------------------------------------------ */
void setup() {
/* ------------------------------------------------------ */  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(LED_PIN, OUTPUT);      // set the LED pin mode
  pinMode(LED2_PIN, OUTPUT);      // set the LED2 pin mode
  pinMode(LED3_PIN, OUTPUT);      // set the LED3 pin mode
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
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
     
    // wait 10 seconds for connection:
    delay(10000);
  }
  
  printWifiStatus();
  Serial.print("Time1: ");
  myTime1 = millis();
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:

  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.github.com");
    client.println("Connection: close");
    client.println();
  }
}

/* just wrap the received data up to 80 columns in the serial print*/
/* ------------------------------------------------------ */
void read_response() {
/* ------------------------------------------------------ */  
  uint32_t received_data_num = 0;
  while (client.available()) {
    /* actual data reception */
    char c = client.read();
    /* print data to serial port */
    Serial.print(c);
    /* wrap data to 80 columns*/
    received_data_num++;
    if(received_data_num % 80 == 0) { 
      Serial.println();
    }
  }  
}

/* -------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------- */ 
  read_response();

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    Serial.print("Time2: ");
    myTime2 = millis();
    myTime3 = myTime2 - myTime1;
    if(myTime3 < 3000)digitalWrite(LED_PIN,HIGH);
    else if(myTime3 < 7000)digitalWrite(LED2_PIN,HIGH);
    else digitalWrite(LED3_PIN,HIGH);

    // do nothing forevermore:
    while (true);
  }
}

/* ---------------------------------------------------- */
void printWifiStatus() {
/* ---------------------------------------------------- */ 
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
