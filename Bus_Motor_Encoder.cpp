// Copyright (c) 2015 by Wayne C. Gramlich.  All rights reserved.

#include <Bus_Slave.h>
#include <Bus_Motor_Encoder.h>

Motor_Encoder::Motor_Encoder() {
  Encoder = 0;
  reset();
}

void Motor_Encoder::reset(){
   TargetTicksPerFrame = 0.0;
   // Leave *encoder* field alone:
   PrevEnc = Encoder;
   output = 0;
   PrevInput = 0;
   ITerm = 0;
}

