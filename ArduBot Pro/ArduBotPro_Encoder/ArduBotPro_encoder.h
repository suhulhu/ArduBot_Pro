// definitions of constants and variables

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
char sLED = HIGH;
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
long count_L=0, count_R=0, Pcount_L=0, Pcount_R=0;  // initialize variables
int Vcount_L=0, Vcount_R=0;
