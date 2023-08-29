#include <Keyboard.h>

typedef enum KeyState { OFF,
                        PRIMED,
                        FIRE };
KeyState currentKeyState;

int ledPin = 13;
int buttonPin = 11;
bool currButtonState = false;
bool prevButtonState = false;
unsigned long lastButtonPressTime = 0;
int bounceTime = 20;

bool manualMode = false;

unsigned long keyPressDuration = 448;
unsigned long autoFireDelay = 6500;


void setup() {
  Keyboard.begin();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {

  readButton();

  if (manualMode) {
    switch (currentKeyState) {
      case OFF:
        digitalWrite(ledPin, LOW);
        break;
      case PRIMED:
        blink();
        break;
      case FIRE:
        fire();
        break;
    }
  } else {
    switch (currentKeyState) {
      case OFF:
        digitalWrite(ledPin, LOW);
        break;
      case PRIMED:
        blink();
        break;
      case FIRE:
        autofire();
        break;
    }
  }
}

//-----------------------------------------------------------------------------

void readButton() {
  bool buttonState = (digitalRead(buttonPin) == LOW);
  unsigned long buttonPressDuration = millis() - lastButtonPressTime;

  if (buttonState != currButtonState and buttonPressDuration > bounceTime) {
    lastButtonPressTime = millis();
    prevButtonState = currButtonState;
    currButtonState = buttonState;
  }

  if (currButtonState and !prevButtonState) {
    onButtonPress();
  }

  prevButtonState = currButtonState;
}

//-----------------------------------------------------------------------------

void onButtonPress() {
  switch (currentKeyState) {
    case OFF:
      currentKeyState = PRIMED;
      Serial.println("PRIMED");
      break;
    case PRIMED:
      currentKeyState = FIRE;
      Serial.println("FIRE");
      break;
    case FIRE:
      if (!manualMode) {
        currentKeyState = OFF;
        Keyboard.releaseAll();
      }
      break;
  }
}

//-----------------------------------------------------------------------------

void blink() {
  static const unsigned long duration = 350;
  static unsigned long timer = 0;
  static bool ledState = true;

  if (millis() - timer > duration) {
    timer = millis();
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}

//-----------------------------------------------------------------------------

void fire() {
  static unsigned long keyPressTime;
  static volatile bool fireLock = false;

  if (!fireLock) {
    keyPressTime = millis();
    fireLock = true;
    digitalWrite(ledPin, HIGH);
    Keyboard.press('x');
  } else {
    if (millis() - keyPressTime > keyPressDuration) {
      Keyboard.release('x');
      fireLock = false;
      currentKeyState = OFF;
      Serial.println("OFF");
    }
  }
}

void autofire() {
  static unsigned long keyPressTime;
  static volatile bool fireLock = false;

  static volatile bool wait = false;
  static unsigned long lastPressTime = 0;

  if (!wait) {
    if (!fireLock) {
      keyPressTime = millis();
      fireLock = true;
      digitalWrite(ledPin, HIGH);
      Keyboard.press('x');
      Serial.println("PRESS");
    } else {
      if (millis() - keyPressTime > keyPressDuration) {
        Keyboard.release('x');
        fireLock = false;
        lastPressTime = millis();
        Serial.println("RELEASE");
        wait = true;
      }
    }
  } else {
    if (millis() - lastPressTime > autoFireDelay) {
      wait = false;
      Serial.println("RESET");
    }
  }
}

//-----------------------------------------------------------------------------