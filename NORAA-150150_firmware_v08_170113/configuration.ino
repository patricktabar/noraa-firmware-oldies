// #######################################################################################
// LOAD BASIC CONFIGURATION
// #######################################################################################

void loadConfig(){
  // turn on servo1 on pin 10---------------------------------------------------> SERVO1
  servo1.attach(SERVO_1_UP_DOWN_PIN);
  penUp();
  if (DEBUG) Serial.println("servo attached and raised");

  // turn on servo2 on pin 9--------------------------------------------------> SERVO2
  servo2.attach(SERVO_2_OPEN_CLOSE_PIN);
  penOpen();
  if (DEBUG) Serial.println("servo attached and open");
  
  // set stepperX stepperY speed and acceleration----------------------------> STEPPERS
  stepperX.setMaxSpeed(SPEED_MOTOR_X_AXIS);
  //Serial.println("stepperX speed set");
  stepperX.setAcceleration(ACCELERATION_MOTOR_X_AXIS);
  //Serial.println("stepperX acceleration set");
  
  stepperY.setMaxSpeed(SPEED_MOTOR_Y_AXIS);
  //Serial.println("stepperY speed set");
  stepperY.setAcceleration(ACCELERATION_MOTOR_Y_AXIS);
  //Serial.println("stepperY acceleration set");
  
  // set limit switch as input------------------------------------------> LIMIT SWITCHES                                                                      
  pinMode(ENDSTOP_PIN_X_AXIS, INPUT);
  pinMode(ENDSTOP_PIN_Y_AXIS, INPUT);  
  }
