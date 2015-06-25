#include <Servo.h>

// Servo definitions
Servo myServo;
#define servoDegrees 70
#define servoStepDelay 20
int servoPos = 0;

int ServoPin = 12;



void reset_servo() {
  myServo.attach(ServoPin);
  myServo.write(servoDegrees);
  delay(500);
  myServo.detach();
}

void open_door() {
  myServo.attach(ServoPin);
  //myServo.write(0);
  for (; servoPos > 0; servoPos-=1) 
  {
     myServo.write(servoPos);
     delay(servoStepDelay);
  }
  myServo.detach();
  bDoorOpened = true;
}

void close_door() {
  myServo.attach(ServoPin);
  //myServo.write(servoDegrees);
  for (; servoPos <= servoDegrees; servoPos+=1) 
  {
     myServo.write(servoPos);
     delay(servoStepDelay);
  }
  myServo.detach();
  bDoorOpened = false;
}
