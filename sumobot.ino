#include <Servo.h> 

int line1 = 2, line2 = 4;
int sharp1 = A1, sharp2 = A2;
int dist1,dist2;
int servPin1=5,servPin2=6;
Servo servo1,servo2;
bool isArmUp = false;
void setup() {
  Serial.begin(9600);
  servo1.attach(servPin1);
  servo2.attach(servPin2);
  armDown();
}
//servo1 - down = 90, up = 0
//servo2 - down = 35, up = 130
void loop() {
  dist1 = 13*pow(analogRead(sharp1)*0.0048828125, -1);
  dist2 = 13*pow(analogRead(sharp2)*0.0048828125, -1);

  if(dist1<=10 && dist2<=10){
    if(dist1<=4 && dist2<=4){
      if(!isArmUp)
        armUp();
    }
    else{
      if(isArmUp)
        armDown();
      //pursue
    }
  }



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
