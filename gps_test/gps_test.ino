#include <SoftwareSerial.h>

SoftwareSerial gps(3,4);  // RX, TX Connect TX to 3

void setup() 
{
  Serial.begin(115200);
  gps.begin(9600);

}

void loop() 
{
  if (gps.available())
  {
    Serial.write(gps.read());
  }

}
