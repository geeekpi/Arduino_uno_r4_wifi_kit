#include "Arduino_LED_Matrix.h"   // Include the LED_Matrix library
#include "frames.h"               // Include a header file containing frame data

ArduinoLEDMatrix matrix;          // Create an instance of the ArduinoLEDMatrix class

void setup() {
  Serial.begin(115200);           // Initialize serial communication at a baud rate of 115200
  matrix.begin();                 // Initialize the LED matrix
}

void loop() {
  // Load and display the "fullOn" frame on the LED matrix
  matrix.loadFrame(fullOn);
  delay(500);

  // Load and display the "fulloff" frame on the LED matrix
  matrix.loadFrame(fulloff);
  delay(500);

  // Print the current value of millis() to the serial monitor
  Serial.println(millis());
}
