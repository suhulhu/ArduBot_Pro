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
  }
}

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
