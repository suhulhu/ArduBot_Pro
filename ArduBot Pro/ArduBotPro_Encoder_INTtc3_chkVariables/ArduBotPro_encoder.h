// interrupt 
unsigned char intDone = 0;
unsigned int  countInt = 0;

// motor control
#define PWML  4
#define AIN_1 5
#define AIN_2 8
#define PWMR  3
#define BIN_1 10
#define BIN_2 9
#define Forward HIGH
#define spdLimit  4095

// LED control
#define LED_R 14
#define LED_L 13
#define ON  LOW
#define OFF HIGH
volatile char sLED_R = HIGH;
unsigned int countLED = 0;

// Encoder
#define LQEIA 6
#define LQEIB 7
#define RQEIA 1
#define RQEIB 0
long countL=0, countR=0;
struct Motion_status {
  long  pOLD;
  long  pNEW;
  long  pERROR;
  long  vOLD;
  long  vNEW;
};
Motion_status eMotionR, eMotionL;
long EstR_acceleration = 0, EstL_acceleration;
volatile long count_L=0, count_R=0, Pcount_L=0, Pcount_R=0;  // initialize variables
volatile int Vcount_L=0, Vcount_R=0;
void Encoder_LA() {
  if (digitalRead(LQEIA)==digitalRead(LQEIB)) {count_L--;}
  else {count_L++;}
}
void Encoder_LB() {
  if (digitalRead(LQEIA)==digitalRead(LQEIB)) {count_L++;}
  else {count_L--;}
}
void Encoder_RA() {
  if (digitalRead(RQEIA)==digitalRead(RQEIB)) {count_R--;}
  else {count_R++;}
}
void Encoder_RB() {
  if (digitalRead(RQEIA)==digitalRead(RQEIB)) {count_R++;}
  else {count_R--;}
}

// Initialize Peripherals
void Init_Peripherals() {
  // set output pins
  pinMode(LED_R,  OUTPUT);  pinMode(LED_L,  OUTPUT);
  pinMode(PWML,   OUTPUT);  pinMode(PWMR,   OUTPUT);
  pinMode(AIN_1,  OUTPUT);  pinMode(AIN_2,  OUTPUT);
  pinMode(BIN_1,  OUTPUT);  pinMode(BIN_2,  OUTPUT);

  // set Resolution
  analogReadResolution(12);   // ADC: 0~4095
  analogWriteResolution(12);  // PWM: 0~4095

  // Left wheel encoder pins and interrupts
  pinMode(LQEIA, INPUT);  pinMode(LQEIB, INPUT);
  attachInterrupt(digitalPinToInterrupt(LQEIA), Encoder_LA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LQEIB), Encoder_LB, CHANGE);
  
  // Right wheel encoder pins and interrupts
  pinMode(RQEIA, INPUT);  pinMode(RQEIB, INPUT);
  attachInterrupt(digitalPinToInterrupt(RQEIA), Encoder_RA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RQEIB), Encoder_RB, CHANGE);
}
// setup 1ms interrupt using timer TC3, and clock source GCLK5
void setupTimers()
{
  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 when odd division factor used
                     GCLK_GENCTRL_GENEN |         // Enable GCLK generator
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(5);          // Select GCLK5
//  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |          // Divide the 48MHz clock source by divisor N=3: 48MHz/3=16MHz
                    GCLK_GENDIV_ID(5);            // Select Generic Clock (GCLK) 5
//  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Feed GCLK5 to TCC2 and TC3
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK
                     GCLK_CLKCTRL_GEN_GCLK5 |     // Select GCLK5
                     GCLK_CLKCTRL_ID_TCC2_TC3;    // Feed GCLK5 to TCC2 and TC3
//                     GCLK_CLKCTRL_ID_TC4_TC5;     // Feed GCLK5 to TC4 and TC5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // set TC3 in 16 bits counting mode, Divide the GCLOCK signal by 16
  REG_TC3_CTRLA |= TC_CTRLA_MODE_COUNT16 |        // Set the counter to 16-bit mode
                   TC_CTRLA_PRESCALER_DIV16 |     // Set prescaler to 16, 16MHz/16 = 1MHz
                   TC_CTRLA_WAVEGEN_MFRQ |        // Set Match Frequency mode, cc0 would be period value
                   TC_CTRLA_ENABLE;               // Enable TC3
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY);       // Wait for synchronization

  // timer TC3 counts up to CC0 value to generate 1ms interrupt,
  // this determines the frequency of the interrupt operation: Freq = 48Mhz/(N*CC0*Prescaler)
  REG_TC3_COUNT16_CC0 = 1000;                    // Set the CC0 (period) register to 1000 for 1MHz clock
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY);     // Wait for synchronization

  NVIC_SetPriority(TC3_IRQn, 0);                // Set the Nested Vector Interrupt Controller (NVIC) priority for TC3 to 3 (0 highest)
  NVIC_EnableIRQ(TC3_IRQn);                     // Connect TC3 to Nested Vector Interrupt Controller (NVIC)
  REG_TC3_INTFLAG |= TC_INTFLAG_MC0;            // Clears the interrupt flag
  REG_TC3_INTENSET = TC_INTENSET_MC0;
 }
