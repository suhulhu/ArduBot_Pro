// definitions of constants and variables
#define LED_R 14
#define LED_L 13
char sLED = HIGH;

void setup() {  // put your setup code here, to run once:
  // set output pins
  pinMode(LED_R, OUTPUT);
  pinMode(LED_L, OUTPUT);
}

void loop() {   // put your main code here, to run repeatedly:
  sLED = !sLED;
  digitalWrite(LED_R, !sLED);
  digitalWrite(LED_L, sLED);
  delay(250);
}
