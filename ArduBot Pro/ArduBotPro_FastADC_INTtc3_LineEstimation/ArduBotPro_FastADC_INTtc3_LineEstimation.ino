// analogWrite in Motor control or something else will affect the time4 interrupt settings !!!
// if Motor_control(1000,-1000) is not executed, the period of interrupt is 3ms.
// But if executed the period is 1ms.  Therefore, TC4 interrupt is not suitable here.

#include "ArduBotPro_encoder.h"

void setup() {  // put your setup code here, to run once:
  AdcBooster();
  // Initialize Peripherals
  Init_Peripherals();

  // Initialize communication
  SerialUSB.begin(115200);

  // setup interrupt timer TC3
  setupTimers();

  // motor control
//  Motor_control(1000,1000);
}

void loop() {   // put your main code here, to run repeatedly:
//  StateMachine(sButton);
  if (sButton==2 && intDone==1) {
    SerialUSB.println(Lp);
//    checkVariables();
//    digitalWrite(LED_R, sLED_R);
//    readAllIR_values();
//    sLED_R = !sLED_R;
//    digitalWrite(LED_R, OFF);
    intDone = 0;
  }
}

// TC3 Interrupt Service Routine
void TC3_Handler() {
//  digitalWrite(LED_R, ON);

  checkButton();
  StateMachine(sButton);

  // Motor encoder
  READ_QEI();
  QEI_filter();
//  digitalWrite(LED_R, OFF);
  
  // check variables
  intDone=1;
    
  // clear the interrupt flag
  REG_TC3_INTFLAG |= TC_INTFLAG_MC0; 
 }

void checkVariables() {
  SerialUSB.print(eMotionR.vNEW);Serial.print("\t");
  SerialUSB.println(Vcount_R*128);
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

// Read Quadrature Encoder values
void READ_QEI() {
  // calulate the speed information of the wheel,  unit:pulses/T_interval
  Vcount_R = count_R - Pcount_R;
  Vcount_L = count_L - Pcount_L;
  // keep previous pulse counts
  Pcount_R = count_R;
  Pcount_L = count_L;
}

// Encoder resolution enhancement algorithm, Pcount_R, Pcount_L: Q24.7
void QEI_filter() {
  // wn = 0.1, zeta = 1, Kp = wn^2 = 0.01, Kv = 2*zeta*wn = 0.2
  // update for velocity estimation
  eMotionR.vNEW = eMotionR.vOLD + EstR_acceleration;
  eMotionL.vNEW = eMotionL.vOLD + EstL_acceleration;
  // update for position estimation
  eMotionR.pNEW = eMotionR.pOLD + eMotionR.vOLD;
  eMotionL.pNEW = eMotionL.pOLD + eMotionL.vOLD;
  // update for estimation error
  eMotionR.pERROR = Pcount_R*128 - eMotionR.pNEW;
  eMotionL.pERROR = Pcount_L*128 - eMotionL.pNEW;
  // update for acceleration estimation
  EstR_acceleration = eMotionR.pERROR/100 - eMotionR.vNEW/5; //pError*Kp - vNew*Kv
  EstL_acceleration = eMotionL.pERROR/100 - eMotionL.vNEW/5; //pError*Kp - vNew*Kv
  // update OLD estimation
  eMotionR.vOLD = eMotionR.vNEW;
  eMotionR.pOLD = eMotionR.pNEW;
  eMotionL.vOLD = eMotionL.vNEW;
  eMotionL.pOLD = eMotionL.pNEW;
}

// Read IR sensor values
void readAllIR_values() {
  unsigned char ind;
  digitalWrite(IRcontrol, HIGH);  // turn on IR LEDs
  for (ind=0; ind<50; ind++);  // delay for ADC sampling
  for (ind=0; ind<7; ind++) {
    IRsensors[ind] = analogRead(IRindex[ind]);
  }
  digitalWrite(IRcontrol, LOW);  // turn off IR LEDs
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
//    else if (IR_caliValues[index]>4000) 
//      IR_caliValues[index] = 4000;
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

void checkButton() {
  if (cCount<2000)  cCount++;
  button = digitalRead(BUTTON);
  if (button==LOW) {                    // button pressed
    if (buttonPressed==LOW) {           // button pressed not yet confirmed
      bPressCount++;
      if (bPressCount>countButton){
        buttonPressed = HIGH;           // button pressed confirmed
        bPressCount = 0;                // reset pressed count value
      }
    }
    else {                              // button pressed confirmed and button not yet released
      bReleaseCount = 0;
      bPressCount=0;
    }
  }
  else {                                // button==HIGH, released
    if (buttonPressed==LOW) {           // button pressed not yet confirmed
      bReleaseCount = 0;
      bPressCount = 0;  
    }
    else {                              // button pressed confirmed
      bReleaseCount++;
      if (bReleaseCount>countButton){   
        buttonPressed = LOW;            // button released confirmed
        bReleaseCount = 0;              // reset released count value
        sButton++;                      // change Button status value
        if (sButton==1) cCount = 0;     // reset cCount
        if (sButton>3) sButton=0;       // the largest value of Button status is 3
      }
    }
  }
}

// State Machine which uses sButton value
void StateMachine(unsigned char value) {
  unsigned char index;
  switch (value) {
    case 0: 
      digitalWrite(LED_R, OFF);
      digitalWrite(LED_L, OFF);
      for (index=0;index<6;index++){
        IR_vMax[index]=0;
        IR_vMin[index]=900;
      }
      break;
    case 1:
      digitalWrite(LED_R, ON);
      digitalWrite(LED_L, OFF);
//      Motor_control(1000,1000);
//      if (cCount==1000) Motor_control(1000,1200);   // not balanced in left and right wheels
//      else if (cCount==1500) Motor_control(0,0);
      readAllIR_values();
      IR_Max_Min();
      // calculate weighted average 計算權重平均
//      Lp = LINE_estimation(IRsensors);
//      Lp = LINE_estimation(IR_caliValues);
//      CHECK_IRvariables();
      break;
    case 2:
      digitalWrite(LED_R, OFF);
      digitalWrite(LED_L, ON);
      readAllIR_values();
      IR_calibrations();
      Lp = LINE_estimation(IR_caliValues);
//      CHECK_IRcalibrations();
      break;
    case 3:
      digitalWrite(LED_R, ON);
      digitalWrite(LED_L, ON);
      break;
  }  
}

// settings for faster ADC speed, for 7 IRs ~ 250us
void AdcBooster()
{
  ADC->CTRLA.bit.ENABLE = 0;                      // Disable ADC
  while(ADC->STATUS.bit.SYNCBUSY==1);             // Wait for synchronization
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV64 |    // Divide Clock by 128, ~31us per ADC sample
                   ADC_CTRLB_RESSEL_12BIT;        // Result on 12 bits
//  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |  // 1 sample
//                     ADC_AVGCTRL_ADJRES(0x00ul);// Adjusting result by 0
  ADC->SAMPCTRL.reg = 0x03;                       // Sampling Time Length = 3
  ADC->CTRLA.bit.ENABLE = 1;                      // Enable ADC
  while(ADC->STATUS.bit.SYNCBUSY==1);             // Wait for synchronization
} // AdcBooster

// Line position estimation
int LINE_estimation(int IRvalues[]) {
  float temp_n=0, temp_d=0;
  unsigned char ind;
  int LPos;

  for (ind=1;ind<6;ind++) {
    temp_n += (float)IRvalues[ind]*(6.0-ind);
    temp_d += (float)IRvalues[ind];
  }
  LPos = (int)(temp_n/temp_d*100.0);
  return(LPos);
}
