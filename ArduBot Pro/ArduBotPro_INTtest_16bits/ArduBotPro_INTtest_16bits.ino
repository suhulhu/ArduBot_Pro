/*
test TC4 1ms interrupt FOR MKRzero 

Note the timer TCC0 has only 4 channels (0-3 and 4-7 are the same), 
while TCC1 and TCC2 each have 2, giving you 8 channels in total.

*/
// LEDs
#define LED_R 14
#define LED_L 13

// motor control
#define PWML  4
#define PWMR  3

unsigned int sCount=0;
bool sLED_R = LOW;

void setup() {
  // put your setup code here, to run once:
  // LEDs
  pinMode(LED_R,  OUTPUT);  pinMode(LED_L,  OUTPUT);
  pinMode(PWML,   OUTPUT);  pinMode(PWMR,   OUTPUT);
  // set Resolution
  analogReadResolution(12);   // ADC: 0~4095
  analogWriteResolution(12);  // PWM: 0~4095

  setupTimers();
  digitalWrite(LED_L, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// setup 1ms interrupt using timer TC4, and clock source GCLK5
void setupTimers()
{
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |          // Divide the 48MHz clock source by divisor N=3: 48MHz/3=16MHz
                    GCLK_GENDIV_ID(5);            // Select Generic Clock (GCLK) 5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 when odd division factor used
                     GCLK_GENCTRL_GENEN |         // Enable GCLK generator
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(5);          // Select GCLK5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Feed GCLK5 to TC4 and TC5
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK
                     GCLK_CLKCTRL_GEN_GCLK5 |     // Select GCLK5
                     GCLK_CLKCTRL_ID_TCC2_TC3;    // Feed GCLK5 to TCC2 and TC3
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // set TC3 in 16 bits counting mode, Divide the GCLOCK signal by 16
  REG_TC3_CTRLA |= TC_CTRLA_MODE_COUNT16 |        // Set the counter to 16-bit mode
                   TC_CTRLA_PRESCALER_DIV16 |     // Set prescaler to 16, 16MHz/16 = 1MHz
                   TC_CTRLA_WAVEGEN_MFRQ |        // Set Match Frequency mode, cc0 would be period value
                   TC_CTRLA_ENABLE;               // Enable TC3
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY);       // Wait for synchronization

  // timer TC3 counts up to CC0 value to generate 1ms interrupt,
  // this determines the frequency of the interrupt operation: Freq = 48Mhz/(N*CC0*Prescaler)
  REG_TC3_COUNT16_CC0 = 999;                    // Set the CC0 (period) register to 1000 for 1MHz clock
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY);     // Wait for synchronization

  NVIC_SetPriority(TC3_IRQn, 0);                // Set the Nested Vector Interrupt Controller (NVIC) priority for TC4 to 3 (0 highest)
  NVIC_EnableIRQ(TC3_IRQn);                     // Connect TC3 to Nested Vector Interrupt Controller (NVIC)
  REG_TC3_INTFLAG |= TC_INTFLAG_OVF;            // Clears the interrupt flag
  REG_TC3_INTENSET = TC_INTENSET_OVF;           // Enable TC3 OVF interrupt
 }

// TC3 Interrupt Service Routine
void TC3_Handler() {
    sLED_R = !sLED_R;
    digitalWrite(LED_R, sLED_R);
//    analogWrite(PWML, 100);
//    analogWrite(PWMR, 100);
    
    // clear the interrupt flag
    REG_TC3_INTFLAG |= TC_INTFLAG_OVF; 
 }
