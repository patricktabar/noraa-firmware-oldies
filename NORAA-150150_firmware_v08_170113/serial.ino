// #######################################################################################
// SERIAL PARSER FUNCTIONS
// #######################################################################################

//-------------------------------------------------------------------------------> GET CMD
void getCommand(){

  if (Serial.available() > 0){
      //read incoming byte
      INCOMING_BYTE = Serial.read();
      //append byte to string buffer
      BUFFER += INCOMING_BYTE;

      //check if buffer fits max size
      if (BUFFER.length() > MAX_BUFFER_LENGTH){
        Serial.println("-- error: buffer over quota --");
        clearBuffer();
        askForMore();                      //send 'S' to receive new command
      }

      //parse buffer and ask for more
      if (INCOMING_BYTE == '\n' || INCOMING_BYTE == '\r'){
        parseBuffer();
        clearBuffer();
        askForMore();
        setLastCommandTime();
      }
  }
}

//empty buffer---------------------------------------------------------------> EMPTY BUFFER
void clearBuffer(){
   BUFFER = "";
}

//ask for more-------------------------------------------------------------------> ASK MORE
void askForMore(){
  // send s to receive more stuff
  if(DEBUG) Serial.println("ask for more...");
  Serial.write('S');
  Serial.flush();
}

//check timeout--------------------------------------------------------------------> LAST_COMMAND_TIME
void checkTimeOut(){
  if(millis() - LAST_COMMAND_TIME > timeOut) {
    Serial.write('>');
    Serial.flush();
    setLastCommandTime();
    if(DEBUG) Serial.println(millis() - LAST_COMMAND_TIME);
    }
  }

void setLastCommandTime(){
  LAST_COMMAND_TIME = millis();
  }

//parse buffer----------------------------------------------------------------> PARSE BUFFER
void parseBuffer(){
  BUFFER.toUpperCase();

  /*
  parse command format: char(header) , float(xVal), float(yVal), float(zVal), float(uVal), float(vVal), float(wVal) - es. D,100.0000,200.0000,25.0

  header code:

  H --> Homing: calls homing() function

  V --> Velocity: set x/y velocity using setNewVelocity(int) function
  C --> Acceleration: set x/y acceleration using setNewAcceleration(int) function

  D --> Draw: calls goTo(float,float) function that implements velocity and not acceleration
  R --> Run: calls runTo(float, float) function that implements both velocity and acceleration

  A --> Arc: draw an arc(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y)

  X --> X Axis Stepper: move x axis to given position using goTo(float)
  Y --> Y Axis Stepper: move y axis to given position using goTo(float)

  Z --> set Pen Up/Down angle
  O --> set Pen Open/Close angle

  E --> set delay time
  G --> switch DEBUG on/off

  // default: send 'A' in order to repeat last command
  */

  //header values
  char header = BUFFER.charAt(0);

  //parsing commas
  int firstComma = BUFFER.indexOf(',');
  int secondComma = BUFFER.indexOf(',', firstComma+1);
  int thirdComma = BUFFER.indexOf(',', secondComma+1);
  int fourthComma = BUFFER.indexOf(',', thirdComma+1);
  int fifthComma = BUFFER.indexOf(',', fourthComma+1);
  int sixthComma = BUFFER.indexOf(',', fifthComma+1);

  //x,y,z & i,j,k values
  String firstValue = BUFFER.substring(firstComma+1, secondComma);
  String secondValue = BUFFER.substring(secondComma+1, thirdComma);
  String thirdValue = BUFFER.substring(thirdComma+1, fourthComma);
  String fourthValue = BUFFER.substring(fourthComma+1, fifthComma);
  String fifthValue = BUFFER.substring(fifthComma+1, sixthComma);
  String sixthValue = BUFFER.substring(sixthComma+1);

  //x,y,z & i,j,k values to float
  float xVal = firstValue.toFloat();
  float yVal = secondValue.toFloat();
  float zVal = thirdValue.toFloat();
  float iVal = fourthValue.toFloat();
  float jVal = fifthValue.toFloat();
  float kVal = sixthValue.toFloat();

  switch (header){

    case 'H':
      // goes home
      homing();
    break;

    case 'V':
      //set velocity to
      setNewVelocity(int(xVal));
    break;

    case 'C':
      //set acceleration to
      setNewAcceleration(int(xVal));
    break;

    case 'D':
      // gotTo x,y,z  draw straight line
      setServo1Angle(int(zVal));
      goTo(xVal,yVal);
    break;

    case 'R':
      //runTo x,y,z  with acceleration
      setServo1Angle(int(zVal));
      runTo(xVal,yVal);
    break;

    case 'A':
      //draw arc
      if(DEBUG){
        Serial.print("A,");
        Serial.print(xVal);
        Serial.print(',');
        Serial.print(yVal);
        Serial.print(',');
        Serial.print(zVal);
        Serial.print(',');
        Serial.print(iVal);
        Serial.print(',');
        Serial.print(jVal);
        Serial.print(',');
        Serial.println(kVal);
        }
      arc(xVal, yVal, zVal, iVal, jVal, kVal);         
    break;

    case 'S':
      //draw spline - quadratic bezier

    break;

    case 'X':
      //move x stepper to
      setStepperX(xVal);
    break;

    case 'Y':
      //move x stepper to
      setStepperY(xVal);
    break;

    case 'Z':
      //servo1 angle set to
      setServo1Angle(int(xVal));
    break;

    case 'O':
      //servo2 angle set to
      setServo2Angle(int(xVal));
    break;

    case 'E':
      //delay set to
      setDelay(int(xVal));
    break;

    case 'G':
      //switch DEBUG on/off
      if (DEBUG) {
        Serial.println("DEBUG OFF - ZERO VERBOSITY LEVEL");
        DEBUG = false;
        VERBOSITY = 0;        
        }
      else {
        Serial.print("DEBUG ON");
        DEBUG = true;
        VERBOSITY = int(xVal);
        returnMenu();        
        }      
    break;
  }
}
