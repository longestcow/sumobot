#include <Servo.h> 

int line = 2;
int sharp1 = A0, sharp2 = A1;
int dist1,dist2;
int servPin1=10,servPin2=11;
int in1 = 6, in2 = 9, in3 = 5, in4 = 3;

Servo servo1,servo2;
bool isArmUp = false;
void setup() {
  Serial.begin(9600);
  servo1.attach(servPin1);
  servo2.attach(servPin2);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  delay(3000);


//  armDown();
}
//servo1 - down = 90, up = 0
//servo2 - down = 35, up = 130
void loop() {
  
  dist1 = 13*pow(analogRead(sharp1)*0.0048828125, -1);
  dist2 = 13*pow(analogRead(sharp2)*0.0048828125, -1);

//
  if(dist1<=10 && dist2<=10){
    if(dist1<=4 && dist2<=4){
//      if(!isArmUp)
//        armUp();
    }
    else{
//      if(isArmUp)
//        armDown();
      moveForward();
    }
  }
  else{
    turnRight();
  }
}



void moveForward(){
  analogWrite(in1, 125);
  analogWrite(in2, 0);
  analogWrite(in3, 125);
  analogWrite(in4, 0);
}
void moveBackward(){
  analogWrite(in1, 0);
  analogWrite(in2, 255);
  analogWrite(in3, 0);
  analogWrite(in4, 255);
}
void moveStop(){
  analogWrite(in1, 0);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 0);
}
void turnRight(){
  analogWrite(in1, 255);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 255);
  
  
  
}
void armDown(){
  isArmUp=false;
  servo1.write(90);
  servo2.write(33);
}
void armUp(){
  isArmUp=true;
  servo1.write(0);
  servo2.write(132);
}
