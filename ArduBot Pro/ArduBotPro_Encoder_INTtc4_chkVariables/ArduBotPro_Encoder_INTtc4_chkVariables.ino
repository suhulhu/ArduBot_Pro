// analogWrite in Motor control or something else will affect the time4 interrupt settings !!!
// if Motor_control(1000,-1000) is not executed, the period of interrupt is 3ms.
// But if executed the period is 1ms.  Therefore, TC4 interrupt is not suitable here.

#include "ArduBotPro_encoder.h"

void setup() {  // put your setup code here, to run once:
  // Initialize Peripherals
  Init_Peripherals();

  // Initialize communication
  SerialUSB.begin(115200);

  // setup interrupt timer TC4
  setupTimers();

  // motor control
  Motor_control(1000,-1000);
}

void loop() {   // put your main code here, to run repeatedly:
  if (countInt<5000) {
    if (intDone==3) {
      checkVariables();
      countInt++;
      intDone = 0;
    }
  }
}

// TC4 Interrupt Service Routine
void TC4_Handler() {
//  digitalWrite(LED_R, ON);
    sLED_R = !sLED_R;
    digitalWrite(LED_R, sLED_R);

  // Motor encoder
  READ_QEI();
  QEI_filter();
//  digitalWrite(LED_R, OFF);
    
  // clear the interrupt flag
  REG_TC4_INTFLAG |= TC_INTFLAG_MC0; 

  // check variables
  intDone++;
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
