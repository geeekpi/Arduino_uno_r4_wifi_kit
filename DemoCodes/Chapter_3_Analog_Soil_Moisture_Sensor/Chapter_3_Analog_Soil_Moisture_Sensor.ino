#define AO_PIN A0    // Arduino's pin connected to AO pin of the rain sensor

void setup() {
  // initialize serial communication
  Serial.begin(9600);
  analogReadResolution(14); //default to 10-bit resolution  change to 14-bit resolution
}

void loop() {
  //read analog from A0 pin
  int moistureValue = analogRead(AO_PIN);

  Serial.println(moistureValue);  // print out the analog value
  delay(1000);                // pause for 1 sec to avoid reading sensors frequently to prolong the sensor lifetime
}
