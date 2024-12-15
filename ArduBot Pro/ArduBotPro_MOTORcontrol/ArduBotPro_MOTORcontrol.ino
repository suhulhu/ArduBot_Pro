#include "ArduBotPro_motor.h"

void setup() {  // put your setup code here, to run once:
  // set output pins
  pinMode(LED_R,  OUTPUT);  pinMode(LED_L,  OUTPUT);
  pinMode(PWML,   OUTPUT);  pinMode(PWMR,   OUTPUT);
  pinMode(AIN_1,  OUTPUT);  pinMode(AIN_2,  OUTPUT);
  pinMode(BIN_1,  OUTPUT);  pinMode(BIN_2,  OUTPUT);
  analogReadResolution(12);   // ADC: 0~4095
  analogWriteResolution(12);  // PWM: 0~4095
}

void loop() {   // put your main code here, to run repeatedly:
  sLED = !sLED;
  digitalWrite(LED_R, sLED);
  digitalWrite(LED_L, !sLED);
  delay(250);

  // Test motors
  Motor_control(0,0);
}

void Motor_control(int speed_L, int speed_R) {
  
  // Left motor
  if (speed_L>0) {
    if (speed_L>spdLimit) speed_L = spdLimit;
    // Left motor
    digitalWrite(BIN_1, HIGH);
    digitalWrite(BIN_2, LOW);
    analogWrite(PWML, speed_L);
  }
  else {
    if (speed_L<-spdLimit) speed_L = -spdLimit;
    digitalWrite(BIN_1, LOW);
    digitalWrite(BIN_2, HIGH);
    analogWrite(PWML, -speed_L);
  }

  // Right motor
  if (speed_R>0) {
    if (speed_R>spdLimit) speed_R = spdLimit;
    // Left motor
    digitalWrite(AIN_1, HIGH);
    digitalWrite(AIN_2, LOW);
    analogWrite(PWMR, speed_R);
  }
  else {
    if (speed_R<-spdLimit) speed_R = -spdLimit;
    // Left motor
    digitalWrite(AIN_1, LOW);
    digitalWrite(AIN_2, HIGH);
    analogWrite(PWMR, -speed_R);
  }
}
