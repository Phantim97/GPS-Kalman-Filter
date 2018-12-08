#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

union
{
  float f;
  byte fbytes[4];
} u;

float sensorData[12];
byte sensorByteData[394];

TinyGPSPlus tinyGPS; // derives GPS data from module

#define GPS_TX 3
#define GPS_RX 4
#define GPS_BAUD 115200
#define MS 1000


const int MPU_addr=0x68;  // I2C address of the MPU-6050
#define MPU6050_ACCEL_FS_2 0x00
#define MPU6050_ACCEL_FS_4 0x01
#define MPU6050_ACCEL_FS_8 0x02
#define MPU6050_ACCEL_FS_16 0x03
bool blinkState = false;

SoftwareSerial myGPS(GPS_TX, GPS_RX);

unsigned long timer;
uint16_t Tmp,AcX1,AcY1,AcZ1=0;
void setup()
{
  Serial.begin(115200);
  myGPS.begin(GPS_BAUD);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  int setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
}

void calcGPSInfo()
{
  // Print latitude, longitude, altitude in feet, course, speed, date, time,
  // and the number of visible satellites.
  sensorData[0] = (tinyGPS.location.lat()/1.0);
  sensorData[1] = (tinyGPS.location.lng()/1.0);
  sensorData[2] = tinyGPS.altitude.feet();
  sensorData[3] = tinyGPS.course.deg();
  sensorData[4] = tinyGPS.speed.mph();
  sensorData[5] = (tinyGPS.satellites.value()/1.0);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    incMPUData();
    // If data has come in from the GPS module
    while (myGPS.available())
      tinyGPS.encode(myGPS.read()); // Send it to the encode function
  } while (millis() - start < ms);
}

void incMPUData(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  
  sensorData[6]+=(Wire.read()<<8|Wire.read())-sensorData[6];  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  sensorData[7]+=(Wire.read()<<8|Wire.read())-sensorData[7];  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  sensorData[8]+=(Wire.read()<<8|Wire.read())-sensorData[8];  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  sensorData[9]+=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  sensorData[10]+=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  sensorData[11]+=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  if(AcX1==0 && AcY1==0 && AcZ1==0 && sensorData[8]){
    calibrate();
  }
}
void calibrate(){
  AcX1=sensorData[6];
  AcY1=sensorData[7];
  AcZ1=sensorData[8];
}
void calcMPUData(){
  //acc*mph/s conversion/ g's to m/s^2 /max g limint /#of incs
  sensorData[6] = (sensorData[6]*2.236936/9.81/16384.0/MS);
  sensorData[7] = (sensorData[7]*2.236936/9.81/16384.0/MS);
  sensorData[8] = (sensorData[8]*2.236936/9.81/16384.0/MS);
  //Serial.print(" "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  sensorData[9] = (sensorData[9]/131.0)/MS;
  sensorData[10] = (sensorData[10]/131.0)/MS;
  sensorData[11] = (sensorData[11]/131.0)/MS;
  blinkState = !blinkState;
}


//Insert a proper send/recv here
void dataSend()
{
  Serial.write(0x90);
  delay(10);
  for (int i = 0; i < 12; i++)
  {
    u.f = sensorData[i];
    for (byte j = 0; j < 4; j++)
    {
      Serial.write(u.fbytes[j]);
      //delay(10);
    }
    sensorData[i] = 0;
  }
  
  Serial.write(0x10);
  delay(10);
}

void loop()
{
  calcGPSInfo();
  calcMPUData();
  dataSend();
  smartDelay(MS);
  Serial.flush(); 
}
