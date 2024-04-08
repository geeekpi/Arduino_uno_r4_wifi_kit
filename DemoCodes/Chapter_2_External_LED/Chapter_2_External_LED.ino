const int LED_PIN = 2 ; //out pin
// the setup function runs once when you press reset or power the board.
void setup() {
  // initialize digital pin 2 as an output.
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  //flashing the LED 
  digitalWrite(LED_PIN, LOW);  //turn on the LED
  delay(500);                 //delay 500ms

  digitalWrite(LED_PIN, HIGH);  //turn off the LED
  delay(500); 						//delay 500ms
}
