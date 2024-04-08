#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHT11_PIN 2        // Define the pin used to connect the sensor
#define DHTTYPE DHT11      // Define the type of the sensor
const int BUTTON_PIN = 7;  // the number of the pushbutton pin

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 column and 2 rows
DHT dht11(DHT11_PIN, DHT11);         // Create a DHT object

int lastButtonState = 0;     // the previous state of button
int currentButtonState = 0;  // the current state of button
int state = 0;

void setup() {
  dht11.begin();                      // initialize the sensor
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // set arduino pin to input pull-up mode
  lcd.init();       // initialize the lcd
  lcd.clear();      // clear the LCD display
  lcd.backlight();  // Make sure backlight is on
  currentButtonState = digitalRead(BUTTON_PIN);
}

void loop() {
  delay(500);                 //Refresh the temperature and humidity data every 0.5s
  lastButtonState = currentButtonState;          // save the last state
  currentButtonState = digitalRead(BUTTON_PIN);  // read new state
  
  // read humidity
  float humi = dht11.readHumidity();
  // read temperature as Celsius
  float tempC = dht11.readTemperature();
  // read temperature as Fahrenheit
  float tempF = dht11.readTemperature(true);

  if (isnan(humi) || isnan(tempC) || isnan(tempF)) {
    lcd.setCursor(0, 0);  //put "Failed" in LCD1602 (0,0)
    lcd.print("Failed");
  }
  if (state == 0) {
      display_tempC(tempC, humi);    //display temperature and humidity
    } else {
      display_tempF(tempF, humi);   //display Fahrenheit temperature and humidity
    }
  if (currentButtonState == 0 && lastButtonState == 1) {     //when the button is pushed
    state = !state;                       //change state and display another temperature
  }
}

void display_tempF(float temp, float humi) {           
  lcd.clear();          //clear the last display, and update the LCDdisplay
  lcd.setCursor(0, 0);  // start to print at the first row
  lcd.print("Fahr: ");
  lcd.print(temp);       // print the Fahrenheit temperature °F
  lcd.print((char)223);  // print ° character
  lcd.print("F");

  lcd.setCursor(0, 1);  // start to print at the second row
  lcd.print("Humi: ");
  lcd.print(humi);  // print the humidity
  lcd.print("%");
}
void display_tempC(float temp, float humi) {          
  lcd.clear();           //clear the last display, and update the LCDdisplay
  lcd.setCursor(0, 0);  // start to print at the first row
  lcd.print("Temp: ");
  lcd.print(temp);       // print the temperature °C
  lcd.print((char)223);  // print ° character
  lcd.print("C");

  lcd.setCursor(0, 1);  // start to print at the second row
  lcd.print("Humi: ");
  lcd.print(humi);  // print the humidity
  lcd.print("%");
}
