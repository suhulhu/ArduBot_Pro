// Using TC3 as source of interrupt with 1ms period (PR value = 1000)
// AdcBooster() is used to set up faster ADC for IR readings, ~ 30us for one IR sensor
// sButton = 1 for IR calibration, and the ArduBot Pro will run by itself for 0.5s(~10cm)
// sButton = 2 (state) begins to follow the line
// Vc and \Omega controls are tested

#include "ArduBotPro_encoder.h"
#include "ArduBotPro_Motion.h"
#include "ArduBotPro_IRsensors.h"

void setup() {  // put your setup code here, to run once:
  // Initialize faster ADC settings
  AdcBooster();
  
  // Initialize Peripherals
  Init_Peripherals();

  // setup interrupt timer TC3
  setupTimers();

  // Initialize communication
  SerialUSB.begin(115200);
}

void loop() {   // put your main code here, to run repeatedly:
  if (sButton==1 && intDone==1) {
    SerialUSB.println(Lp);
//    digitalWrite(LED_R, sLED_R);
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

//  digitalWrite(LED_R, OFF);
  
  // check variables
  intDone=1;
    
  // clear the interrupt flag
  REG_TC3_INTFLAG |= TC_INTFLAG_MC0; 
 }

// used to check velocity estimation variables
void checkVariables() {
  SerialUSB.print(eMotionR.vNEW);Serial.print("\t");
  SerialUSB.println(Vcount_R*128);
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
        if (sButton==1) cCount = 0;     // reset 
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
      if (cCount==1000) Motor_control(1000,1200);   // not balanced in left and right wheels
      else if (cCount==1500) Motor_control(0,0);
      readAllIR_values();
      IR_Max_Min();
      // calculate weighted average 計算權重平均
//      Lp = LINE_estimation(IRsensors);
      break;
    case 2:
      // status LED
      digitalWrite(LED_R, OFF);
      digitalWrite(LED_L, ON);

      // IR sensor value processing
      readAllIR_values();
      IR_calibrations();
      
      // calculate weighted average 計算權重平均
      Lp = LINE_estimation(IR_caliValues);
      
      // Motor encoder and filtering
      READ_QEI();
      QEI_filter();
      
      LINE_following();
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

// Line following with voltage control
void LINE_following() {
  int deltaPWM, spd_L, spd_R;
  
  error_new = center - Lp;
  deltaPWM = Kp*error_new + Kd*(error_new - error_old);
  error_old = error_new;
  spd_L = basePWM - deltaPWM;
  spd_R = basePWM + deltaPWM;
  Motor_control(spd_L, spd_R);
}
