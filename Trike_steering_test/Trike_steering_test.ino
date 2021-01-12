int stMoveToLimit(int);
void stDriveActuator(int, int);

#define EXTEND 1
#define RETRACT -1
#define STOP 0
#define MARGIN 10  //allowable margin of err between actual position and setpoint


//uses DROK dual H-bridge driver board
int stIN1 = 10;       
int stIN2 = 11;        //in4
int stENA1 = 9;        //enable pin (pwm)
int stSensorPin = A0;  //brake fb
int stSensorVal;        //brake fb value 0 - 1023


int y_JoyPin = A2;      //this is the drive/brake joystick Y axis
int x_JoyPin = A3;      //this is the steering joystick X axis
int y_JoyVal, x_JoyVal;
   

int stMinReading;    //actual maximum fb value 0-1023
int stMaxReading;    //actual minimum fb value 0-1023
int stSpeed=200;
int stSetpoint=50;
long unsigned prev=0;

void setup() {
  Serial.begin(9600);
  pinMode(stIN1, OUTPUT);        //these two pins control direction of actuator
  pinMode(stIN2, OUTPUT);        //for the brake actuator
  pinMode(stENA1, OUTPUT);
  pinMode(stSensorPin, INPUT);  //fb pin from brake actuator
  digitalWrite(stENA1, LOW);      //initially turn off brake actuator

Serial.print("Extending Actuator to maximum ... ");
delay(500);
stMoveToLimit(EXTEND);  //get brake drive actuator extend limit
delay(2000);            //give it time to get there
stMaxReading=analogRead(stSensorPin);
Serial.print("stMaxReading is "); Serial.println(stMaxReading);

Serial.print("Retracting Actuator to minimum ... ");
delay(500);
stMoveToLimit(RETRACT);
delay(2000); 
stMinReading = analogRead(stSensorPin); //get brake drive actuator retract limit
Serial.print("stMinReading is "); Serial.println(stMinReading);

prev=millis();

}

void loop(){

x_JoyVal =analogRead(x_JoyPin);      //joystick position 0-1023 
stSetpoint = map(x_JoyVal, 0, 1023, stMinReading, stMaxReading);
 
  if((millis()- prev) > 2000){  //prints out info every two seconds
        Serial.print("SP = "); Serial.println(stSetpoint); 
        stSensorVal=analogRead(stSensorPin);
        Serial.print("Steering = "); Serial.println(stSensorVal); 
        prev=millis();
  }
   
    stSensorVal=analogRead(stSensorPin);                                        //read brSensor; divide by two to scale sensor fb value to joystick value
   
    if( (abs(stSetpoint - stSensorVal)) < MARGIN ){
        stDriveActuator(STOP, stSpeed);
    } 
    else if(stSetpoint < stSensorVal) {
          stDriveActuator(RETRACT, stSpeed);
    }
    else if(stSetpoint > stSensorVal) {
        stDriveActuator(EXTEND, stSpeed);
    }
}
   


void stDriveActuator(int Direction, int Speed){  //this is for the steering acuator only
  switch(Direction){
    case 1:      //extension
     
      digitalWrite(stIN2, 1);
      digitalWrite(stIN1, 0);
      analogWrite(stENA1, Speed);
      break;
 
    case 0:      //stopping
      digitalWrite(stIN2, 0);
      digitalWrite(stIN1, 0);
      analogWrite(stENA1,  0);
      break;

    case -1:      //retraction
      digitalWrite(stIN2, 0);
      digitalWrite(stIN1, 1);
      analogWrite(stENA1, Speed);
      break;
  }
}

/* finds the limit of the actuator feedback signal 
inputs: Direction values: EXTEND -1  RETRACT  1  */
int stMoveToLimit(int Direction){
 
  int prevReading=0;
  int currReading=0;
 
  do{
    prevReading = currReading;
    stDriveActuator(Direction, 200);
    delay(100);                            //may have to change this value
    currReading = analogRead(stSensorPin);
 
  }while(prevReading != currReading);
 
  return currReading;
}
