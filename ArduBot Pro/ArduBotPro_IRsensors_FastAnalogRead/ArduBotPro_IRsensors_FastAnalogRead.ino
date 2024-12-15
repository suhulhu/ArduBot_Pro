#include "ArduBotPro.h"

// Global variables
char sLED = HIGH;
unsigned char gIndex=0;
unsigned int IRsensors[7]= {0};
unsigned char IRindex[7] = {A1, A2, A0, A3, A4, A5, A6};

void setup() {  // put your setup code here, to run once:
  // serial communication
  // Serial.begin(9600);

  // Fast Analog Read setup
  AdcBooster();
  
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
//  if (gIndex>100) {
    sLED = !sLED;
//    digitalWrite(LED_R, !sLED);
//    digitalWrite(LED_L, sLED); 
//    gIndex = 0;   
//  }
//  else {
//    gIndex++;
//  }

  // read and check IR sensor values
  digitalWrite(LED_R, !sLED);
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

void AdcBooster()
{
  ADC->CTRLA.bit.ENABLE = 0;                       // Disable ADC
  while(ADC->STATUS.bit.SYNCBUSY==1);              // Wait for synchronization
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV64 |     // Divide Clock by 64.
                   ADC_CTRLB_RESSEL_12BIT;         // Result on 12 bits
//  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |   // 1 sample
//                     ADC_AVGCTRL_ADJRES(0x00ul); // Adjusting result by 0
  ADC->SAMPCTRL.reg = 0x03;                        // Sampling Time Length = 0
  ADC->CTRLA.bit.ENABLE = 1;                       // Enable ADC
  while(ADC->STATUS.bit.SYNCBUSY==1);              // Wait for synchronization
} // AdcBooster
