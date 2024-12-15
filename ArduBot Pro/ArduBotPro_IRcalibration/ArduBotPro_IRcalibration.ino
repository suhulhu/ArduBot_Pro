#include "ArduBotPro.h"

// Global variables
char sLED = HIGH;
char ind;

void setup() {  // put your setup code here, to run once:
  bStatus.pre = 0;
  bStatus.now = 0;
  // serial communication
  SerialUSB.println("IR_p \t IR5 \t IR4 \t IR3 \t IR2 \t IR1 \t IR_SS");
  pinsInit();
}

void loop() {   // put your main code here, to run repeatedly:
  stateMachine();
}

void chkButton(){
  unsigned char temp;
  temp = digitalRead(BUTTON);  
  if (temp==LOW & bStatus.now==0) {
    bCount++;
    if (bCount>2) {
      bStatus.now = bStatus.pre+1;
    }
  }
}

void chkButton2(){
  unsigned char temp;
  temp = digitalRead(BUTTON);  
  if (temp==Pressed) {
    if (bCount<6) {
      bCount++;
      if (bCount>5) {
        bStatus.pre = bStatus.now;
        bStatus.now = bStatus.now+1;
      }
    }
  }
  else {
    bCount = 0; // reset only when button is released
  }
}

void stateMachine() {
  switch (bStatus.now) {
    case 0:
      LEDcontrol(bStatus.now);
      chkButton2();
      break;
    case 1:
      LEDcontrol(bStatus.now);
      readAllIR_values();
      chkButton2();
      IR_Max_Min();
      chkIRvalues();
      break;
    case 2:
      if (bStatus.pre==1) {
        CHECK_MaxMin();
        bStatus.pre = 0;
      }
      else {
        LEDcontrol(bStatus.now);
        readAllIR_values();
        IR_calibrations();
        CHECK_IRcalibrations();
        Calibration_count++;
      }
      break;
  }
}

void LEDcontrol(unsigned char status) {
  switch (bStatus.now) {
    case 0:
      digitalWrite(LED_L, OFF);
      digitalWrite(LED_R, OFF);
      break;
    case 1:
      digitalWrite(LED_L, OFF);
      digitalWrite(LED_R, ON);
      break;
    case 2:
      digitalWrite(LED_L, ON);
      digitalWrite(LED_R, OFF);
      break;
    case 3:
      digitalWrite(LED_L, ON);
      digitalWrite(LED_R, ON);
      break;
    default:
      digitalWrite(LED_L, OFF);
      digitalWrite(LED_R, OFF);
      break;
  }
}

void pinsInit() {
  // Button
  pinMode(BUTTON, INPUT);
  
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

void readAllIR_values() {
  unsigned char index;
  digitalWrite(IRcontrol, HIGH);  // turn on IR LEDs
  //delay(1);
  for (index=0; index<50; index++);  // delay for ADC sampling
  for (index=0; index<7; index++) {
    IRsensors[index] = analogRead(IRindex[index]);
  }
  digitalWrite(IRcontrol, LOW);  // turn off IR LEDs
}

// Check IR raw values through UART
void chkIRvalues() {
  unsigned char index;
  for (index=0; index<7; index++) {
    if (index<6) {
      SerialUSB.print(IRsensors[6-index]);SerialUSB.print("\t");}
    else {
      SerialUSB.println(IRsensors[0]);}
  }
}

// Check IR calibrations 
void CHECK_IRcalibrations() {
  unsigned char index;
  for (index=0; index<5; index++) {
    if (index<4) {
      SerialUSB.print(IR_caliValues[5-index]);SerialUSB.print("\t");}
    else {
      SerialUSB.println(IR_caliValues[5-index]);}
  }
}


// Calibrate IR outputs
void IR_calibrations() {
  unsigned char index;
  long temp;
  for (index=1;index<6;index++){
    temp = long (IRmax-IRmin)* long (IRsensors[index]-IR_vMin[index]);
    IR_caliValues[index] = IRmin + temp/(IR_vMax[index]-IR_vMin[index]); 
    if (IR_caliValues[index]<0) 
      IR_caliValues[index] = 0;
  }
}

// Find IR output Max and Min values
void IR_Max_Min() {
  unsigned char index;
  for (index=1;index<6;index++){
    if (IR_vMax[index]<IRsensors[index])
      IR_vMax[index]=IRsensors[index];
    if (IR_vMin[index]>IRsensors[index])
      IR_vMin[index]=IRsensors[index];
  }
}

void CHECK_MaxMin() {
  SerialUSB.println();
  SerialUSB.print(IR_vMax[5]); SerialUSB.print("\t");
  SerialUSB.print(IR_vMax[4]); SerialUSB.print("\t");
  SerialUSB.print(IR_vMax[3]); SerialUSB.print("\t");
  SerialUSB.print(IR_vMax[2]); SerialUSB.print("\t");
  SerialUSB.println(IR_vMax[1]); 
  
  SerialUSB.print(IR_vMin[5]); SerialUSB.print("\t");
  SerialUSB.print(IR_vMin[4]); SerialUSB.print("\t");
  SerialUSB.print(IR_vMin[3]); SerialUSB.print("\t");
  SerialUSB.print(IR_vMin[2]); SerialUSB.print("\t");
  SerialUSB.println(IR_vMin[1]); 
}
