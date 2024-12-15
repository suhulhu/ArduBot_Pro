#include "ArduBotPro.h"

// Global variables
char sLED = HIGH;
unsigned char gIndex=0;
unsigned int IRsensors[7]= {0};
unsigned char IRindex[7] = {A1, A2, A0, A3, A4, A5, A6};

void setup() {  // put your setup code here, to run once:
  // serial communication
  // Serial.begin(9600);
  
  // set output pins
  // LEDs
  pinMode(LED_R,  OUTPUT);  pinMode(LED_L,  OUTPUT);

  // IR sensors: analog inputs
  pinMode(IR1, INPUT);  pinMode(IR2,   INPUT);
  pinMode(IR3, INPUT);  pinMode(IR4,   INPUT);
  pinMode(IR5, INPUT);  pinMode(IR_prompt, INPUT);
  pinMode(IR_StartStop, INPUT); 
  pinMode(IRcontrol, OUTPUT);
  analogReadResolution(12);   // ADC: 0~4095
}

void loop() {   // put your main code here, to run repeatedly:
  if (gIndex>100) {
    sLED = !sLED;
    digitalWrite(LED_R, !sLED);
    digitalWrite(LED_L, sLED); 
    gIndex = 0;   
  }
  else {
    gIndex++;
  }

  // read and check IR sensor values
  readAllIRsensors();
  chkIRvalues();
}

void readAllIRsensors() {
  unsigned char index;
  digitalWrite(IRcontrol, HIGH);  // turn on IR LEDs
  //delay(1);
  for (index=0; index<50; index++);  // delay for ADC sampling
  for (index=0; index<7; index++) {
    IRsensors[index] = analogRead(IRindex[index]);
  }
  digitalWrite(IRcontrol, LOW);  // turn off IR LEDs
}

void chkIRvalues() {
  unsigned char index;
  for (index=0; index<7; index++) {
    if (index<6) {
      SerialUSB.print(IRsensors[6-index]);SerialUSB.print("\t");}
    else {
      SerialUSB.println(IRsensors[6-index]);}
  }
}
