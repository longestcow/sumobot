#include <Servo.h> 

int line = 2;
int sharp1 = A0, sharp2 = A1;
int dist1,dist2,lineOP;
int servPin1=7,servPin2=8;
int in1 = 6, in2 = 11, in3 = 5, in4 = 3;

int leftC = 0, rightC = 0, lineC = 0;

int speed = 80, slowSpe=speed/3;

int armDistance = 4, oppDistance=10, oppCount=50, lineCThresh = 10;
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
  armDown();
  delay(5000);
}

void loop() {
  float v = analogRead(sharp1)*0.0048828125; 
  dist1 = 13*pow(v, -1);
  v = analogRead(sharp2)*0.0048828125;
  dist2 = 13*pow(v, -1);
  lineOP = digitalRead(line);
  

  if(lineOP==1)
    lineC+=1;
  else lineC=0;

  if(lineC>lineCThresh){
    moveBackward(speed/1.5);
    delay(500);
    lineC=0;
  }
  else{
    if(dist1<=oppDistance) leftC+=1;
    else leftC=0;
    if(dist2<=oppDistance) rightC+=1;
    else rightC=0;
  
    if(leftC>=oppCount && rightC>=oppCount)
      moveForward(speed);
    else {
      if(leftC>rightC)
        turnRight(slowSpe);
      else if(rightC>leftC)
        turnLeft(slowSpe);
      else if(rightC==0 && leftC==0)
        turnRight(slowSpe);
      else // both are equal, opp is likely forward
        moveForward(slowSpe);
    }
    
    if(dist1<=armDistance && dist2<=armDistance && (!isArmUp))
      armUp();
    else if(dist1>armDistance && dist2>armDistance && isArmUp)
      armDown();
  }
  Serial.println(leftC);
  Serial.println(rightC);
  Serial.println("---------");
}



void moveForward(int spe){
  analogWrite(in1, spe);
  analogWrite(in2, 0);
  analogWrite(in3, spe);
  analogWrite(in4, 0);
}
void moveBackward(int spe){
  analogWrite(in1, 0);
  analogWrite(in2, spe);
  analogWrite(in3, 0);
  analogWrite(in4, spe);
}
void moveStop(int spe){
  analogWrite(in1, 0);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 0);
}
void turnRight(int spe){
  analogWrite(in1, spe);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, spe);
}
void turnLeft(int spe){
  analogWrite(in1, 0);
  analogWrite(in2, spe);
  analogWrite(in3, spe);
  analogWrite(in4, 0);
}


void armDown(){
  isArmUp=false;
  servo1.writeMicroseconds(1500);
  servo2.writeMicroseconds(1500);
}
void armUp(){
  isArmUp=true;
  servo1.writeMicroseconds(2000);
  servo2.writeMicroseconds(1000);
}
