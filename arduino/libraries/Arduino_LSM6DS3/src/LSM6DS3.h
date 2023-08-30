/*
  This file is part of the Arduino_LSM6DS3 library.
  Copyright (c) 2019 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#define LSM6DS3_ADDRESS 0x6A

#define LSM6DS3_WHO_AM_I_REG 0X0F
#define LSM6DS3_CTRL1_XL 0X10
#define LSM6DS3_CTRL2_G 0X11

#define LSM6DS3_STATUS_REG 0X1E

#define LSM6DS3_CTRL6_C 0X15
#define LSM6DS3_CTRL7_G 0X16
#define LSM6DS3_CTRL8_XL 0X17

#define LSM6DS3_OUTX_L_G 0X22
#define LSM6DS3_OUTX_H_G 0X23
#define LSM6DS3_OUTY_L_G 0X24
#define LSM6DS3_OUTY_H_G 0X25
#define LSM6DS3_OUTZ_L_G 0X26
#define LSM6DS3_OUTZ_H_G 0X27

#define LSM6DS3_OUTX_L_XL 0X28
#define LSM6DS3_OUTX_H_XL 0X29
#define LSM6DS3_OUTY_L_XL 0X2A
#define LSM6DS3_OUTY_H_XL 0X2B
#define LSM6DS3_OUTZ_L_XL 0X2C
#define LSM6DS3_OUTZ_H_XL 0X2D

class LSM6DS3 {
public:
  enum ODR_XL {
    ODR_Power_down = 0x0,
    ODR_12_5_Hz = 0x10,
    ODR_26_Hz = 0x20,
    ODR_52_Hz = 0x30,
    ODR_104_Hz = 0x40,
    ODR_208_Hz = 0x50,
    ODR_416_Hz = 0x60,
    ODR_833_Hz = 0x70,
    ODR_1_66_kHz = 0x80,
    ODR_3_33_kHz = 0x90,
    ODR_6_66_kHz = 0xa0,
  };

  enum FS_XL {
    FS_2g = 0x0,
    FS_16g = 0x4,
    FS_4g = 0x8,
    FS_8g = 0xc,
  };

  enum BW_XL {
    BW_400Hz = 0x0,
    BW_200Hz = 0x1,
    BW_100Hz = 0x2,
    BW_50Hz = 0x3,
  };

public:
  LSM6DS3(TwoWire &wire, uint8_t slaveAddress);
  LSM6DS3(SPIClass &spi, int csPin, int irqPin);
  virtual ~LSM6DS3();

  int begin();
  void end();

  // Accelerometer
  void setAccelerometerSettings(uint8_t accBitMaskSettings);
  void setAccSensitivity(LSM6DS3::FS_XL scale);
  void setAccODR(LSM6DS3::ODR_XL odr);
  virtual int readAcceleration(float &x, float &y, float &z);  // Results are in g (earth gravity).
  virtual float accelerationSampleRate();                      // Sampling rate of the sensor.
  virtual int accelerationAvailable();                         // Check for available data from accelerometer

  // Gyroscope
  virtual int readGyroscope(float &x, float &y, float &z);  // Results are in degrees/second.
  virtual float gyroscopeSampleRate();                      // Sampling rate of the sensor.
  virtual int gyroscopeAvailable();                         // Check for available data from gyroscope

protected:
  int readRegister(uint8_t address);
  int readRegisters(uint8_t address, uint8_t *data, size_t length);
  int writeRegister(uint8_t address, uint8_t value);

private:
  TwoWire *_wire;
  SPIClass *_spi;
  uint8_t _slaveAddress;
  int _csPin;
  int _irqPin;

  uint8_t accFS  = LSM6DS3::FS_2g;
  uint8_t accODR = LSM6DS3::ODR_416_Hz;
  uint8_t accBW  = LSM6DS3::BW_100Hz;
  SPISettings _spiSettings;
};

extern LSM6DS3 IMU_LSM6DS3;
#undef IMU
#define IMU IMU_LSM6DS3