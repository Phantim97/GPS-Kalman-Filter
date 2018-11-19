#include "TinyGPS++.h"
TinyGPSPlus tinyGPS; // derives GPS data from module

#include <SoftwareSerial.h>
#define GPS_TX 3
#define GPS_RX 4
#define GPS_BAUD 9600

SoftwareSerial myGPS(GPS_TX, GPS_RX);

void setup()
{
  Serial.begin(9600);
  myGPS.begin(GPS_BAUD);
}

void loop()
{
  printGPSInfo();

  smartDelay(1000); 
}

void printGPSInfo()
{
  // Print latitude, longitude, altitude in feet, course, speed, date, time,
  // and the number of visible satellites.
  Serial.print("Lat: "); Serial.println(tinyGPS.location.lat(), 6);
  Serial.print("Long: "); Serial.println(tinyGPS.location.lng(), 6);
  Serial.print("Alt: "); Serial.println(tinyGPS.altitude.feet());
  Serial.print("Course: "); Serial.println(tinyGPS.course.deg());
  Serial.print("Speed: "); Serial.println(tinyGPS.speed.mph());
  Serial.print("Time: "); printTime();
  Serial.print("Sats: "); Serial.println(tinyGPS.satellites.value());
  Serial.println();
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    // If data has come in from the GPS module
    while (myGPS.available())
      tinyGPS.encode(myGPS.read()); // Send it to the encode function
  } while (millis() - start < ms);
}

void printTime()
{
  Serial.print(tinyGPS.time.hour());
  Serial.print(":");
  if (tinyGPS.time.minute() < 10) Serial.print('0');
  Serial.print(tinyGPS.time.minute());
  Serial.print(":");
  if (tinyGPS.time.second() < 10) Serial.print('0');
  Serial.println(tinyGPS.time.second());
}
