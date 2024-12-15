// LEDs
#define LED_R 14
#define LED_L 13
#define ON  LOW
#define OFF HIGH

// Button
#define BUTTON  12
#define Pressed   LOW
#define Released  HIGH
//unsigned char sButton = 0;  
unsigned int bCount = 0;   
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

// IR sensors
#define Pts_checked   200
#define Pts_results   Pts_checked*2
#define IRmin         5
#define IRmax         4000
unsigned int Calibration_count=0;
unsigned char IRindex[7] = {A1, A2, A0, A3, A4, A5, A6};
int IRsensors[7]={0}, IR_caliValues[7];
int IR_vMax[6]={0, 0, 0, 0, 0, 0}, IR_vMin[6]={900, 900, 900, 900, 900, 900};
