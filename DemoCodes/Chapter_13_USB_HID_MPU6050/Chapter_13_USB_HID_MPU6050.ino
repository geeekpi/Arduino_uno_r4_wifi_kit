#include <Wire.h>
#include "Keyboard.h"
#include <MPU6050.h>

// create MPU object and setting variables
#define OUTPUT_READABLE_ACCELGYRO
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

// button to change keyboard.
const int buttonPin = 4;         // input pin for pushbutton
int previousButtonState = HIGH;  // for checking the state of a pushButton
int counter = 0;


void setup() {
  Serial.begin(9600);

  // make the pushButton pin an input:
  pinMode(buttonPin, INPUT);

  Wire.begin();  // initializing I2C

  // Initialize the MPU6050
  mpu.initialize();
  Serial.println("Testing device connections...");
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // Set keyboard mode
  Keyboard.begin();
}


void loop() {
  // read the pushbutton:
  int buttonState = digitalRead(buttonPin);
  // if the button state has changed,
  if ((buttonState != previousButtonState)
      // and it's currently pressed:
      && (buttonState == HIGH)) {
    // increment the button counter
    counter++;
    // Read data from MPU6050 accelerometer
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    //     int axisx = map(ax, -16000, 16000, -10, 10);
    //     int axisy = map(ay, -16000, 16000, -10, 10);
    Keyboard.println("---------------Click button to grap data-----------");
    Keyboard.println("---------------Accelerator data-----------");
    Keyboard.print("Ax: ");
    Keyboard.print(ax);
    Keyboard.print("\t Ay: ");
    Keyboard.print(ay);
    Keyboard.print("\t Az: ");
    Keyboard.println(az);
    Keyboard.println("---------------Gyroscope data-------------");
    Keyboard.print("Gx: ");
    Keyboard.print(gx);
    Keyboard.print("\t Gy: ");
    Keyboard.print(gy);
    Keyboard.print("\t Gz: ");
    Keyboard.println(gz);
  }
  // save the current button state for comparison next time:
  previousButtonState = buttonState;
}
