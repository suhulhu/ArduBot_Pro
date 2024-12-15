#include "ArduBotPro.h"

// Global variables
char sLED = HIGH;
unsigned char gIndex=0;
unsigned int IRsensors[7]= {0};
unsigned char IRindex[7] = {A1, A2, A0, A3, A4, A5, A6};
unsigned int Lp;

void setup() {  // put your setup code here, to run once:
  pinsInit();
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

  // read IR sensor values
  readAllIRsensors();
  chkIRvalues();

  // calculate weighted average 計算權重平均
  Lp = LINE_estimation();

  // check Line prediction result
  SerialUSB.println(Lp);

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
      SerialUSB.print(IRsensors[6-index]);Serial.print("\t");}
    else {
      SerialUSB.println(IRsensors[0]);}
  }
}

unsigned int LINE_estimation() {
  float temp_n=0, temp_d=0;
  unsigned char ind;
  unsigned int LPos;

  for (ind=1;ind<6;ind++) {
    temp_n += (float)IRsensors[ind]*(6.0-ind);
    temp_d += (float)IRsensors[ind];
  }
  LPos = (int)(temp_n/temp_d*100.0);
  return(LPos);
}

void pinsInit() {
  // LEDs
  pinMode(LED_R,  OUTPUT);  pinMode(LED_L,  OUTPUT);

  // IR sensors: analog inputs
  pinMode(IR1, INPUT);  pinMode(IR2,   INPUT);
  pinMode(IR3, INPUT);  pinMode(IR4,   INPUT);
  pinMode(IR5, INPUT);  pinMode(IR_prompt, INPUT);
  pinMode(IR_StartStop, INPUT); 
  pinMode(IRcontrol, OUTPUT);

  // Read resolution: 12bits
  analogReadResolution(12);   // ADC: 0~4095  
}
