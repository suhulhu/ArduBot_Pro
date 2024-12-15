// LEDs
#define LED_R 14
#define LED_L 13
#define ON  LOW
#define OFF HIGH

// motor control
#define PWML  4
#define AIN_1 5
#define AIN_2 8
#define PWMR  3
#define BIN_1 10
#define BIN_2 9
#define Forward HIGH
#define spdLimit  4095

// Button
#define BUTTON  12
#define Pressed   LOW
#define Released  HIGH
//unsigned char sButton = 0;  
unsigned int bCount = 0;
unsigned char bCountLimit = 5;   
struct sButton {        // status of Button
   unsigned char pre:4;
   unsigned char now:4;
}; 
struct sButton bStatus;

// IR sensors
#define IR1           A2
#define IR2           A0
#define IR3           A3
#define IR4           A4
#define IR5           A5
#define IR_prompt     A6
#define IR_StartStop  A1
#define IRcontrol     11

// IR Calibration
#define IRmin         5
#define IRmax         4000
unsigned int Calibration_count=0;
unsigned char IRindex[7] = {A1, A2, A0, A3, A4, A5, A6};
int IRsensors[7]={0}, IR_caliValues[7];
int IR_vMax[6]={0, 0, 0, 0, 0, 0}, IR_vMin[6]={900, 900, 900, 900, 900, 900};

// Weighted average
int Lp;

// Line following
#define center  300
int error_new, error_old;
int Kp=10, Kd=0, basePWM = 1200;
