#include <Arduino_CAN.h>
#include <DHT11.h>

#define DHT11PIN 7      //DHT 11 sensor
DHT11 dht11(DHT11PIN);  // create an instance of dht11

static uint32_t const CAN_ID = 0x20;

void setup()
{
  pinMode(DHT11PIN, INPUT);
  
  Serial.begin(115200);
  while (!Serial) { };

  if (!CAN.begin(CanBitRate::BR_250k))
  {
    Serial.println("CAN.begin(...) failed.");
    for (;;) {}
  }
}

void loop()
{
  /* Assemble a CAN message with the format of
   * temperature and humidity 2 of them  [4 byte message ]
   */
  char temperature = (char)dht11.readTemperature();
  char humidity = (char)dht11.readHumidity();
  Serial.print("TEMP: ");
  Serial.print(int(temperature));
  Serial.print("\tHumidity: ");
  Serial.println(int(humidity));
   
  char t = int(temperature);
  char h = int(humidity);
  
  uint8_t const msg_data[] = {t, h, 0,0,0,0,0,0,0,0}; // create a 4 bytes data
  CanMsg const msg(CanStandardId(CAN_ID), sizeof(msg_data), msg_data); 

  /* Transmit the CAN message, capture and display an
   * error core in case of failure.
   */
  if (int const rc = CAN.write(msg); rc < 0)
  {
    Serial.print  ("CAN.write(...) failed with error code ");
    Serial.println(rc);
    for (;;) { }
  }
  /* Only send one message per second. */
  delay(1000);
}
