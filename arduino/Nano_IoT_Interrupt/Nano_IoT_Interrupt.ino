/*
  Example of setting the TC3 Interrupt Timer on Arduino Nano 33 IoT
*/
float time = 0.0f;
float lastTime = 0.0f;
bool isLEDOn = 0;
int LED_PIN = 13;

void timerCallback() {
  float currentTime = (float(millis()) / 1000.0f);
  time += currentTime - lastTime;
  Serial.println(time);
  lastTime = currentTime;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  setupTimerInterrupt(2.0);
}

void loop() {
  digitalWrite(LED_PIN, isLEDOn);
  isLEDOn = !isLEDOn;
  delay(1000);
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