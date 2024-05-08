/*
   Author: Bart Boogmans
   Contributors: OpenAI ChatGPT

   Description: Arduino script controlling two servos using the Servo library.
                It includes a sequence of four steps to be repeated, with different PWM signals sent to each servo.
                The script also prints the current stage through the serial connection.
*/

#include <Servo.h>

Servo servo1;
Servo servo2;
int stage = 0;

void setup() {
  servo1.attach(12); // Servo 1 connected to pin 12
  servo2.attach(13); // Servo 2 connected to pin 13

  Serial.begin(9600); // Initialize the serial connection
}

void loop() {

  // Read the serial input
  if (Serial.available()) {
    char input = Serial.read();
    // If the input is 'a', go to the next stage
    if (input == 'a') {
      stage = (stage + 1) % 4;
    }
  }

  switch (stage) {
    case 0:
      // Stage 0: No PWM signal sent
      servo1.writeMicroseconds(0);
      servo2.writeMicroseconds(0);
      Serial.println("Stage 0");
      delay(10);
      break;
    case 1:
      // Stage 1: 1500Hz
      servo1.writeMicroseconds(1500);
      servo2.writeMicroseconds(1500);
      Serial.println("Stage 1");
      delay(10);
      break;
    case 2:
      // Stage 2: 2000Hz
      servo1.writeMicroseconds(2000);
      servo2.writeMicroseconds(2000);
      Serial.println("Stage 2");
      delay(10);
      break;
    case 3:
      // Stage 3: 1000Hz
      servo1.writeMicroseconds(1000);
      servo2.writeMicroseconds(1000);
      Serial.println("Stage 3");
      delay(10);
      break;
  }
}
