/*
  LDR Key
*/

const int windowSize = 10;
unsigned long movingWindow[windowSize] = {0};
int i = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  Serial.print(0);
  Serial.print(' ');
  Serial.print(1024);
  Serial.print(' ');
  
  movingWindow[i++] = sensorValue;
  if (i == windowSize)  
    i = 0;
  
  unsigned long avg = 0;

  for (int j = 0; j < windowSize; j++)
  {
    avg += movingWindow[j];
  }
  
  Serial.println(avg / windowSize);

  if (sensorValue > 378)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);
  delay(1);  // delay in between reads for stability
}
