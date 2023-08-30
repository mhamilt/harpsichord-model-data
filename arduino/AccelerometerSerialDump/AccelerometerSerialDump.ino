/*
 Store the last N samples from IMU and dump them out when triggered by a reed sensor. 
*/

#include "Arduino_LSM6DS3.h"


const int accBufferSize = 300;
float accBuffer[accBufferSize];
unsigned int accBufferWriteIndex = 0;

void timerCallback() {
  updateAccBuffer();

  if (accBufferWriteIndex == (accBufferSize - 1)) {
    stopTimer();
    dumpBufferToSerial();
  }
}

void updateAccBuffer() {
  float x, y, z;
  IMU.readAcceleration(x, y, z);

  accBuffer[accBufferWriteIndex] = x;

  accBufferWriteIndex++;
  if (accBufferWriteIndex == accBufferSize)
    accBufferWriteIndex = 0;
}

void dumpBufferToSerial() {
  for (int i = 0; i < accBufferSize; i++) {
    unsigned int r = accBufferWriteIndex + 1 + i;
    if (r >= accBufferSize)
      r -= accBufferSize;

    Serial.println(accBuffer[r]);
    delay(1);
  }
}

void onReedTrigger() {
  dumpBufferToSerial();
}

void setup() {
  Serial.begin(115200);  
  setupTimerInterrupt(1.0);
}

void loop() {
  
}

//--------------------------------------------------------------------------------
// Timer Functions

const uint32_t hzToCompMatchReg(const float hz) {
  return (uint32_t(48000000.0f / hz) / 1024) - 1;
}

void setTimerFrequency(float frequencyHz) {
  uint32_t compareValue = hzToCompMatchReg(frequencyHz);

  TcCount32* TC = (TcCount32*)TC3;
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  Serial.println(TC->COUNT.reg);
  Serial.println(TC->CC[0].reg);
  while (TC->STATUS.bit.SYNCBUSY == 1)
    ;
}

void setupTimerInterrupt(float frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3);
  while (GCLK->STATUS.bit.SYNCBUSY == 1)
    ;

  TcCount32* TC = (TcCount32*)TC3;

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1)
    ;

  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT32;
  while (TC->STATUS.bit.SYNCBUSY == 1)
    ;

  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;  // not TC_CTRLA_WAVEGEN_NFRQ
  while (TC->STATUS.bit.SYNCBUSY == 1)
    ;

  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
  while (TC->STATUS.bit.SYNCBUSY == 1)
    ;

  setTimerFrequency(frequencyHz);

  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_EnableIRQ(TC3_IRQn);

  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1)
    ;
}

void TC3_Handler() {
  TcCount32* TC = (TcCount32*)TC3;

  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    timerCallback();
  }
}

void stopTimer() {
  TcCount32* TC = (TcCount32*)TC3;
  TC->CTRLA.reg = 0;
}
