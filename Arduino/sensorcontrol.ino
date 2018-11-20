#include<Wire.h>
#include <SoftwareSerial.h>
#include"TinyGPS++.h"

TinyGPSPlus tinyGPS; // derives GPS data from module

#define GPS_TX 3
#define GPS_RX 4
#define GPS_BAUD 9600

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
#define MPU6050_ACCEL_FS_2 0x00
#define MPU6050_ACCEL_FS_4 0x01
#define MPU6050_ACCEL_FS_8 0x02
#define MPU6050_ACCEL_FS_16 0x03
bool blinkState = false;

SoftwareSerial myGPS(GPS_TX, GPS_RX);

unsigned long timer;

void setup()
{
  Serial.begin(9600);
  myGPS.begin(GPS_BAUD);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  int setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
}
oid printGPSInfo()
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

void printMPUData(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.print("AcX = "); Serial.print(AcX/16384.0);
  Serial.print(" | AcY = "); Serial.print(AcY/16384.0);
  Serial.print(" | AcZ = "); Serial.print(AcZ/16384.0);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  Serial.print(" | GyX = "); Serial.print(GyX/ 131.0);
  Serial.print(" | GyY = "); Serial.print(GyY/ 131.0);
  Serial.print(" | GyZ = "); Serial.println(GyZ/ 131.0);
      blinkState = !blinkState;
}

void loop()
{
  printGPSInfo();
  printMPUData();
  smartDelay(1000); 
}
