// ###################################################################################################################
// DRAWING FUNCTIONS
// ###################################################################################################################

// GOTO //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this function moves the pen to x,y with global coordinates no acceleration only velocity //////////////////////////
// get x and y in millimeters - mm ///////////////////////////////////////////////////////////////////////////////////
void goTo(float x, float y){
  float currentX, currentY, spanX, spanY, spanMax, delta, directionX, directionY;

  //convert mm in steps
  x *= PULLEY_X;
  y *= PULLEY_Y;

  //get current position to calcolate direction
  currentX = stepperX.currentPosition();
  currentY = stepperY.currentPosition();

  if (DEBUG){
    Serial.print("currentX: ");
    Serial.println(currentX);
    Serial.print("currentY: ");
    Serial.println(currentY);
    Serial.println(" ");
  }

  //calculate span
  spanX = abs(x - currentX);
  spanY = abs(y - currentY);

  if (DEBUG){
    Serial.print("spanX: ");
    Serial.println(spanX);
    Serial.print("spanY: ");
    Serial.println(spanY);
    Serial.println(" ");
  }

  //return span max used to distribute steps along the wider span
  spanMax = max(spanX, spanY);

  //calculate delta
  if(spanX != 0) delta = abs(spanY / spanX);
  else delta = 0;

  if(DEBUG){
    Serial.print("Delta: ");
    Serial.println(delta);
  }

  //get direction related to current position
  if (x < currentX) directionX = -1.0;
  else directionX = 1.0;

  if (y < currentY) directionY = -1.0;
  else directionY = 1.0;

  if (DEBUG){
    Serial.print("directionX: ");
    Serial.println(directionX);
    Serial.print("directionY: ");
    Serial.println(directionY);
    Serial.println(" ");
  }

  if(delta != 0){
    for(int i=0; i<=spanMax; i++){

      //if movement is distributed on x axis apply delta to y otherwise reverse
      if(spanX >= spanY){
        x = currentX + i * directionX ;
        y = currentY + i * directionY * delta;
      }
      else{
        x = currentX + i * directionX / delta;
        y = currentY + i * directionY;
      }

      //if x,y belong to the surface move where u have to
      if((x>=0) && (x<=SURFACE_WIDTH) && (y>=0) && (y<=SURFACE_HEIGHT)){
        stepperX.moveTo(x);
        stepperY.moveTo(y);

        stepperX.setSpeed(SPEED_MOTOR_X_AXIS * directionX);
        stepperY.setSpeed(SPEED_MOTOR_Y_AXIS * directionY);

        while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0) {
          stepperX.runSpeedToPosition();
          stepperY.runSpeedToPosition();
        }
      }
    }
  }
  else{
  //if x,y belong to the surface move where u have to
  if((x>=0) && (x<=SURFACE_WIDTH) && (y>=0) && (y<=SURFACE_HEIGHT)){
    stepperX.moveTo(x);
    stepperY.moveTo(y);

    stepperX.setSpeed(SPEED_MOTOR_X_AXIS * directionX);
    stepperY.setSpeed(SPEED_MOTOR_Y_AXIS * directionY);

    while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0) {
      stepperX.runSpeedToPosition();
      stepperY.runSpeedToPosition();
      }
    }
  }
  whereAmI();
}


// RUNTO /////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this function is the equivalent of STEPTO with acceleration with GLOBAL coordinates ///////////////////////////////
// get x and y in millimeters - mm ///////////////////////////////////////////////////////////////////////////////////
void runTo(float x, float y) {

  x *= PULLEY_X;
  y *= PULLEY_Y;

  if((x>=0) && (x<=SURFACE_WIDTH) && (y>=0) && (y<=SURFACE_HEIGHT)){
    stepperX.moveTo(x);
    stepperY.moveTo(y);

    while (stepperX.distanceToGo() != 0 || stepperY.distanceToGo() != 0) {
      stepperX.run();
      stepperY.run();
      }
    }
  whereAmI();
}

// ARC /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this function draws an ellipse with x/y starting points and x/y radius //////////////////////////////////////////////
void arc(float centerx, float centery, float radiusx, float radiusy, float start_angle, float end_angle){
  
  //smaller means more accurate but slower
  float resolution = .05;
  //if series based on 3 points distribution
  float start_arcx = centerx + (radiusx * cos(radians(start_angle)));
  float start_arcy = centery + (radiusy * sin(radians(start_angle)));
  float end_arcx = centerx + (radiusx * cos(radians(end_angle)));
  float end_arcy = centery + (radiusy * sin(radians(end_angle)));
  //get current position to calcolate direction
  float currentX = stepperX.currentPosition();
  float currentY = stepperY.currentPosition();
  float start_arcx_step = start_arcx * PULLEY_X;
  float start_arcy_step = start_arcy * PULLEY_Y;
  //use this to store last angle calculated
  float last_angle = start_angle;
  //
  //if actual position != start point, raise pen and move there
  if( (abs(currentX - start_arcx_step) > 1) || abs(currentY - start_arcy_step) > 1){
    penUp();
    goTo(start_arcx, start_arcy);
    }  
  //if start_angle - end_angle < resolution draw a line from start to end
  if(abs(start_angle-end_angle) < resolution){
    penDown();
    goTo(end_arcx, end_arcy);
    }
  //
  //draw arc
  else{
    penDown();    
    //s<=e
    if(start_angle <= end_angle){
      if(DEBUG) Serial.println("start_angle <= end_angle");
        for(float i = start_angle; i <= end_angle; i+=resolution){
          float arcx = centerx + (radiusx * cos(radians(i)));
          float arcy = centery + (radiusy * sin(radians(i)));
          last_angle = i;
          //if it belong to the drawing surface
          if(((arcx* PULLEY_X)>=0) && ((arcx* PULLEY_X)<=SURFACE_WIDTH) && ((arcy* PULLEY_Y)>=0) && ((arcy* PULLEY_Y)<=SURFACE_HEIGHT)){
            goTo(arcx, arcy);
            }      
          }        
      }    
    //s>e
    else{
      if(DEBUG) Serial.println("start_angle > end_angle");
        for(float i = start_angle; i >= end_angle; i-=resolution){
          float arcx = centerx + (radiusx * cos(radians(i)));
          float arcy = centery + (radiusy * sin(radians(i)));
          last_angle = i;
          //if it belong to the drawing surface
          if((arcx>=0) && (arcx<=SURFACE_WIDTH) && (arcy>=0) && (arcy<=SURFACE_HEIGHT)){
            goTo(arcx, arcy);
            }      
          }         
      }
    //if for cycle doesn't finish on the exact end_angle calculate the last arc section
    if(last_angle != end_angle){
      last_angle = end_angle;
      float arcx = centerx + (radiusx * cos(radians(last_angle)));
      float arcy = centery + (radiusy * sin(radians(last_angle)));    
      //if it belong to the drawing surface
      if((arcx>=0) && (arcx<=SURFACE_WIDTH) && (arcy>=0) && (arcy<=SURFACE_HEIGHT)){
        goTo(arcx, arcy);
        } 
      }
    }  
  }
