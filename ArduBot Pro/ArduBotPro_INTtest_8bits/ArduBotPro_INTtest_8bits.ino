/*
test TC4 and TC3 1ms interrupt FOR MKRzero 

If we use TC4 as the interrupt service timer, i.e. GCLK_CLKCTRL_ID_TC4_TC5;
the timing results will be affected by analogWrite(3, xx) or analogWrite(4,xx)

But if we use TC3 as the interrupt service timer, i.e. GCLK_CLKCTRL_ID_TCC2_TC3
the timing results will be correct and not be affected by analogWrite(3, xx) or analogWrite(4,xx)

The only information that I found is in the variant.cpp,
digital pin 3 and 4 with PWM function

 |Pin   |MKR  |PIN |Notes| Peri.A |     Peripheral B      |Perip.C|Perip.D| Peri.E | Peri.F | Periph.G | Periph.H |
 |number|Board|    |     |   EIC  | ADC |  AC | PTC | DAC |SERCOMx|SERCOMx|  TCCx  |  TCCx  |    COM   | AC/GLCK  |
 |      |pin  |    |     |(EXTINT)|(AIN)|(AIN)|     |     |(x/PAD)|(x/PAD)| (x/WO) | (x/WO) |          |          |
 +------------+----+-----+--------+-----+-----+-----+-----+-------+-------+--------+--------+----------+----------+
 | 03   | D3  |PA11|     |   11   | *19 |     | X03 |     | 0/03  | 2/03  |*TCC1/1 | TCC0/3 | I2S/FS0  | GCLK_IO5 |
 | 04   | D4  |PB10|     |  *10   |     |     |     |     |       | 4/02  |* TC5/0 | TCC0/4 | I2S/MCK1 | GCLK_IO4 |

*/
// LEDs
#define LED_R 14
#define LED_L 13

// motor control
#define PWML  4
#define PWMR  3

volatile unsigned int sCount=0;
bool sLED_R = LOW;

void setup() {
  // put your setup code here, to run once:
  // LEDs
  pinMode(LED_R,  OUTPUT);  pinMode(LED_L,  OUTPUT);
  pinMode(PWML,   OUTPUT);  pinMode(PWMR,   OUTPUT);
  setupTimers();
  digitalWrite(LED_L, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// setup 1ms interrupt using timer TC4 (10-bit resolution), and clock source GCLK5
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
//                     GCLK_CLKCTRL_ID_TC4_TC5;     // Feed GCLK5 to TC4 and TC5
                     GCLK_CLKCTRL_ID_TCC2_TC3;     // Feed GCLK5 to TC2 and TC3
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // set TC4 in 8 bits counting mode
//  REG_TC4_CTRLA |= TC_CTRLA_MODE_COUNT8;       // Set the counter to 16-bit mode
//  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);     // Wait for synchronization
//  REG_TC4_COUNT8_PER = 185;
//  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);     // Wait for synchronization

//  NVIC_SetPriority(TC4_IRQn, 0);                  // Set the Nested Vector Interrupt Controller (NVIC) priority for TC4 to 3 (0 highest)
//  NVIC_EnableIRQ(TC4_IRQn);                       // Connect TC4 to Nested Vector Interrupt Controller (NVIC)
//  REG_TC4_INTFLAG |= TC_INTFLAG_OVF;              // Clears the interrupt flag
//  REG_TC4_INTENSET = TC_INTENSET_OVF;
  // Divide the GCLOCK signal by 64 
  
//  REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV256 |     // Set prescaler to 64, 8MHz/64 = 125KHz
//                   TC_CTRLA_ENABLE;               // Enable TC4
//  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization
  
  // set TC3 in 8 bits counting mode
  REG_TC3_CTRLA |= TC_CTRLA_MODE_COUNT8;       // Set the counter to 16-bit mode
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);     // Wait for synchronization
  REG_TC3_COUNT8_PER = 249;
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);     // Wait for synchronization

  NVIC_SetPriority(TC3_IRQn, 0);                  // Set the Nested Vector Interrupt Controller (NVIC) priority for TC2 to 3 (0 highest)
  NVIC_EnableIRQ(TC3_IRQn);                       // Connect TC3 to Nested Vector Interrupt Controller (NVIC)
  REG_TC3_INTFLAG |= TC_INTFLAG_OVF;              // Clears the interrupt flag
  REG_TC3_INTENSET = TC_INTENSET_OVF;

  // Divide the GCLOCK signal by 64 
  REG_TC3_CTRLA |= TC_CTRLA_PRESCALER_DIV64 |     // Set prescaler to 64, 16MHz/256 = 250KHz
                   TC_CTRLA_ENABLE;               // Enable TC3
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization
 }

//void TC4_Handler() {
void TC3_Handler() {
  
//  if (sCount<8) {
//    sCount++;
//  }
//  else {
//    sCount = 0;
    analogWrite(PWML, 100);
    analogWrite(PWMR, 100);
//    pinMode(PWML,INPUT);
//  }
    sLED_R = !sLED_R;
    digitalWrite(LED_R, sLED_R);

  // clear the interrupt flag
//  REG_TC4_INTFLAG |= TC_INTFLAG_OVF; 
  REG_TC3_INTFLAG |= TC_INTFLAG_OVF; 
 }
