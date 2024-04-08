#include <LiquidCrystal_I2C.h> 
#include "DHT.h"
#define DHT11_PIN 2  // Define the pin used to connect the sensor
#define DHTTYPE DHT11  // Define the type of the sensor

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
DHT dht11(DHT11_PIN, DHT11);  // Create a DHT object

void setup()
{
  dht11.begin();     // initialize the sensor

  lcd.init();        // initialize the lcd
  lcd.clear();       // clear the LCD display
  lcd.backlight();   // Make sure backlight is on
}

void loop()
{
  // wait a few seconds between measurements.
  delay(2000);

  // read humidity
  float humi  = dht11.readHumidity();
  // read temperature as Celsius
  float tempC = dht11.readTemperature();
  lcd.clear();

  // check if any reads failed
  if (isnan(humi) || isnan(tempC)) {
    lcd.setCursor(0, 0);   //put "Failed" in LCD1602 (0,0)
    lcd.print("Failed");
  } else {
    lcd.setCursor(0, 0);  // start to print at the first row
    lcd.print("Temp: ");
    lcd.print(tempC);     // print the temperature
    lcd.print((char)223); // print ° character
    lcd.print("C");

    lcd.setCursor(0, 1);  // start to print at the second row
    lcd.print("Humi: ");
    lcd.print(humi);      // print the humidity
    lcd.print("%");
  }
}
