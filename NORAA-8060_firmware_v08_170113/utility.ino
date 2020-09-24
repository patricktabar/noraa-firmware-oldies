// #######################################################################################
// UTILITY FUNCTIONS
// #######################################################################################


// WHEREAMI //////////////////////////////////////////////////////////////////////////////
// this function verify local and global pen position ////////////////////////////////////
void whereAmI () {
  if (DEBUG){
    Serial.print("currentPositionX: ");
    Serial.println(stepperX.currentPosition());
    Serial.print("currentPositionY: ");
    Serial.println(stepperY.currentPosition());
  }
}


// HOMING ////////////////////////////////////////////////////////////////////////////////
// this function move the pen to home position and set the zero point ////////////////////
void homing(){
  if (DEBUG) Serial.print("HOMING...");
  penUp();

  while (digitalRead(ENDSTOP_PIN_X_AXIS) == HIGH) {
    motorX.onestep(BACKWARD, SINGLE);
    delay(3);
  }

  while (digitalRead(ENDSTOP_PIN_Y_AXIS) == HIGH) {
    motorY.onestep(BACKWARD, SINGLE);
    delay(3);
  }

  //Serial.print("home!");
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);


  if (DEBUG) Serial.print("current position HOME");
}


// SET VELOCITY //////////////////////////////////////////////////////////////////////////
// this function sets x/y velocity ///////////////////////////////////////////////////////
void setNewVelocity(int v){
  v = constrain(v,0,3000);
  stepperX.setMaxSpeed(v);
  stepperY.setMaxSpeed(v);
  if (DEBUG) {
    Serial.print("new speed set to: ");
    Serial.println(v);
    }
  }


// SET ACCELERATION //////////////////////////////////////////////////////////////////////
// this function sets x/y acceleration ///////////////////////////////////////////////////
void setNewAcceleration(int a){
  a = constrain(a,0,700);
  stepperX.setAcceleration(a);
  stepperY.setAcceleration(a);
  if (DEBUG) {
    Serial.print("new acceleration set to: ");
    Serial.println(a);
    }
  }


// SET X/Y STEPPER POSITION //////////////////////////////////////////////////////////////
// this function sets x/y stepper position ///////////////////////////////////////////////
void setStepperX(float p){
  float y = stepperY.currentPosition()/PULLEY_Y;
  p = constrain(p,0,DRAWING_SURFACE_WIDTH_MM);
  goTo(p,y);
  }

void setStepperY(float p){
  float x = stepperX.currentPosition()/PULLEY_X;
  p = constrain(p,0,DRAWING_SURFACE_HEIGHT_MM);
  goTo(x, p);
  }


//// PEN SETUP /////////////////////////////////////////////////////////////////////////////
//// this function check pen while up and down, open or close //////////////////////////////
//set servo Pen Up/Down
void penUp() {
  servo1.write(ANGLE_PEN_UP);
  delay(PEN_DELAY);
  if (DEBUG) Serial.println("pen UP");
}

void penDown() {
  servo1.write(ANGLE_PEN_DOWN);
  delay(PEN_DELAY);
  if (DEBUG) Serial.println("pen DOWN");
}

//set servo Pen Open/Close
void penOpen() {
  servo2.write(ANGLE_PEN_OPEN);
  delay(PEN_DELAY);
  if (DEBUG) Serial.println("pen OPEN");
}

void penClose() {
  servo2.write(ANGLE_PEN_CLOSE);
  delay(PEN_DELAY);
  if (DEBUG) Serial.println("pen CLOSE");
}

void setServo1Angle(int a){
  a = constrain(a,0,170);
  servo1.write(a);
  delay(PEN_DELAY);
  if (DEBUG) {
    Serial.print("servo1 up/down angle set to: ");
    Serial.println(a);
    }
  }

void setServo2Angle(int a){
  a = constrain(a,0,200);
  servo2.write(a);
  delay(PEN_DELAY);
  if (DEBUG) {
    Serial.print("servo2 open/close angle set to: ");
    Serial.println(a);
    }
  }

//// DELAY /////////////////////////////////////////////////////////////////////////////////
//// this function set a delay /////////////////////////////////////////////////////////////
void setDelay(int d){
  delay(d);
  if (DEBUG) {
    Serial.print("delay set to milliseconds: ");
    Serial.println(d);
    }
  }

// CONVERT SLOPE TO ANGLE DEGREE ///////////////////////////////////////////////////////////
// this function calculates degrees angle between 2 points /////////////////////////////////
float slope_to_degree(float xp1, float yp1, float xp2, float yp2){
  float slope,angle;
  //if delta != infinity, calculate slope
  if(xp1 != xp2){
    slope = (yp1 -yp2)/(xp1-xp2);
    //delta Y != 0
    if(yp1 != yp2){
      // x>0
      if(xp1-xp2 > 0) angle = atan(slope)*180/PI;
      // x<0
      if (xp1-xp2 < 0){
        if(yp1-yp2 > 0) angle = (atan(slope) + PI)*180/PI;
        if(yp1-yp2 < 0) angle = (atan(slope) - PI)*180/PI;
        }   
      }
    //if delta Y == 0
    else{
      // x>0
      if(xp1-xp2 > 0) angle = 180;
      // x<0
      if(xp1-xp2 < 0) angle = 0;      
      }
    }
  //delta == infinity
  else{
    slope = 0;
    // y>0 --> 90 degrees
    if(yp1-yp2 > 0) angle = 270;
    // y<0 --> 270 degrees
    if(yp1-yp2 < 0) angle = 90;
    }      
  //return degrees
  return angle;
  }

// PRINT MENU ////////////////////////////////////////////////////////////////////////////
// this function prints menu option at start /////////////////////////////////////////////

void returnMenu(){
  Serial.println("::::::");
  Serial.print("device: ");
  Serial.println(DEVICE_NAME);
  Serial.print("firmware: ");
  Serial.println(DEVICE_FIRMWARE);
  Serial.println("::::::");
  Serial.println("MENU:");
  Serial.println("-settings:");
  Serial.println("V --> VELOCITY (V,val): set new velocity");
  Serial.println("C --> ACCELERATION (C, val): set new acceleration");
  Serial.println("-drawing:");
  Serial.println("H --> HOMING");
  Serial.println("D --> DRAW (D,val,val,val): moves implementing velocity only");
  Serial.println("R --> DRAW RUNNING (R,val,val,val): moves implementing acceleration");
  Serial.println("A --> ARC (A, val, val, val, val, val, val): draw arc through 3 points");
  Serial.println("-steppers:");
  Serial.println("X --> X POSITION (X,val): moves x axis to given position");
  Serial.println("Y --> Y POSITION (Y,val): moves y axis to given position");
  Serial.println("-pen:");
  Serial.println("Z --> Z POSITION (Z,val): sets new Pen Up/Down angle");
  Serial.println("O --> OPEN POSITION (O,val): sets new Pen Up/Down angle");
  Serial.println("-utility:");
  Serial.println("E --> DELAY (E,val): set a delay time(millisec.)");
  Serial.println("G --> DEBUG: switch DEBUG on/off");
  Serial.println("::::::");
  Serial.println("Noraa is waiting for bytes...");
  }
