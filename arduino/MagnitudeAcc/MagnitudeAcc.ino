/*
  Arduino LSM6DS3 - Simple Accelerometer

  This example reads the acceleration values from the LSM6DS3
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Uno WiFi Rev 2 or Arduino Nano 33 IoT

  created 10 Jul 2019
  by Riccardo Rizzo

  This example code is in the public domain.
*/

#include "Arduino_LSM6DS3.h"

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1)
      ;
  }

  IMU.setAccODR(LSM6DS3::ODR_208_Hz);

}

void loop() {
  float x, y, z;

  // void setAccSen();
  if (Serial.available() > 0) {
    char inChar = Serial.read();    
    if (inChar > '0' and inChar < '5') {
      Serial.print(String(inChar) + ":");
      Serial.println(inChar);
      switch (inChar) {
        case '1':
          IMU.setAccSensitivity(LSM6DS3::FS_2g);
          break;
        case '2':
          IMU.setAccSensitivity(LSM6DS3::FS_4g);
          break;
        case '3':
          IMU.setAccSensitivity(LSM6DS3::FS_8g);
          break;
        case '4':
          IMU.setAccSensitivity(LSM6DS3::FS_16g);
          break;
      }
    }
  }

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    Serial.print("Min:");
    Serial.print(-1.5f);
    Serial.print("\tMax:");
    Serial.print(1.5f);
    Serial.print("\tX:");
    Serial.print(x);
    Serial.print("\tY:");
    Serial.print(y);
    Serial.print("\tZ:");
    Serial.println(z-1.0f);
    // float mag = (x * x) + (y * y) + (z * z);
    // Serial.println(mag);
  }
}
