#include <Servo.h> 

int line = 2;
int sharp1 = A0, sharp2 = A1;
int dist1,dist2,lineOP;
int servPin1=10,servPin2=11;
int in1 = 9, in2 = 6, in3 = 5, in4 = 3;


int spe = 70;
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
  delay(5000);


  armDown();
}

void loop() {
  
  dist1 = 13*pow(analogRead(sharp1)*0.0048828125, -1);
  dist2 = 13*pow(analogRead(sharp2)*0.0048828125, -1);
  lineOP = digitalRead(line);
  Serial.println(dist1);
//  if(lineOP==1){
//    moveBackward();
//    delay(1000);
//  }
  if(dist1<=10 && dist2<=10){
    if(dist1<=4 && dist2<=4){
      if(!isArmUp)
        armUp();
    }
    else{
      if(isArmUp)
        armDown();
      moveForward();
    }
  }
  else{
    turnRight();
  }
}



void moveForward(){
  analogWrite(in1, spe);
  analogWrite(in2, 0);
  analogWrite(in3, spe);
  analogWrite(in4, 0);
}
void moveBackward(){
  analogWrite(in1, 0);
  analogWrite(in2, spe);
  analogWrite(in3, 0);
  analogWrite(in4, spe);
}
void moveStop(){
  analogWrite(in1, 0);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 0);
}
void turnRight(){
  analogWrite(in1, 40);
  analogWrite(in2, 0);
  analogWrite(in3, 0);
  analogWrite(in4, 40);
}
void armDown(){
  isArmUp=false;
  servo1.write(90);
  servo2.write(90);
}
void armUp(){
  isArmUp=true;
  servo1.write(170);
  servo2.write(10);
}


//--------------------------------
#define in1 6
#define in2 9
#define in3 10
#define in4 11

#define LED_PIN1 A4
#define LED_PIN2 A5
#define LED_PIN3 2

#define SENSOR_PIN_FR A0
#define SENSOR_PIN_FL A1
#define SENSOR_PIN_BR A2
#define SENSOR_PIN_BL A3

#define SOUND_PIN 3

int displaySensorReadings = 0;  //0 - hide,  1-display
int displayDirectionDecisions = 1; //0 - hide,  1 - display

//below are used to manage the 4 Sharp IR sensors and their readings.

#define MAX_SENSING_DISTANCE 40
#define SENSING_THRESHOLD 14

int frontRight = 0;
int frontLeft = 0;
int backRight = 0;
int backLeft = 0;

int objectFR = 0;
int objectFL = 0;
int objectBR = 0;
int objectBL = 0;

int lastDirection = -1;         //0 = last charge movement was forward,   1 = last charge movement was backwards
int searchDirectionFront = -1;  //-1 No information yet
int searchDirectionBack = -1;
//0->only right sensor has seen object most recently (relative to 'front'); clockwise rotation search
//1->only left sensor (relative to front); counterclockwise rotation search

// below used for possible random-walk search when object not in sight
uint32_t walkTimer = 0;
int walkState = 0;
int walkIntervals[] = { 70, 60, 200 };  //turn, move(f/b randomly), stop
int walkDir = 0;                         //0 - FWD   1 - BACK  (for random walk sequence)


//below are used for the 3LED indicators, to control animation while the sumo program is running.

#define LED_ANIMATION_SLOW 250
#define LED_ANIMATION_FAST 140
#define LED_ANIMATION_BOOST 40

uint32_t ledTimer = 0;
int ledAnimationSpeed = LED_ANIMATION_SLOW;
int currentLED = 1;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(SOUND_PIN, OUTPUT);

  startUp();
}

void startUp() {
  for (int i = 0; i < 5; i++) {
    ledsOff();
    ledOn(i % 3 + 1);
    tone(SOUND_PIN, 523, 100);
    delay(1000);
  }
  ledsOff();
}

void ledOn(int ledNum) {
  if (ledNum == 1) digitalWrite(LED_PIN1, HIGH);
  if (ledNum == 2) digitalWrite(LED_PIN2, HIGH);
  if (ledNum == 3) digitalWrite(LED_PIN3, HIGH);
}

void ledOff(int ledNum) {
  if (ledNum == 1) digitalWrite(LED_PIN1, LOW);
  if (ledNum == 2) digitalWrite(LED_PIN2, LOW);
  if (ledNum == 3) digitalWrite(LED_PIN3, LOW);
}

void ledsOff() {
  digitalWrite(LED_PIN1, LOW);
  digitalWrite(LED_PIN2, LOW);
  digitalWrite(LED_PIN3, LOW);
}

void loop() {
  ledPattern();
  //testPattern();
  pollSensors();
  chase();

  delay(10);
}

void pollSensors() {
  float volts = analogRead(SENSOR_PIN_FR) * 0.0048828125;  // value from sensor * (5/1024)
  frontRight = 13 * pow(volts, -1);

  volts = analogRead(SENSOR_PIN_FL) * 0.0048828125;  // value from sensor * (5/1024)
  frontLeft = 13 * pow(volts, -1);

  volts = analogRead(SENSOR_PIN_BR) * 0.0048828125;  // value from sensor * (5/1024)
  backRight = 13 * pow(volts, -1);

  volts = analogRead(SENSOR_PIN_BL) * 0.0048828125;  // value from sensor * (5/1024)
  backLeft = 13 * pow(volts, -1);


  // constrain (set to 0 if above max_distance)
  // if(frontRight > MAX_SENSING_DISTANCE) frontRight = 0;
  // if(frontLeft > MAX_SENSING_DISTANCE) frontLeft = 0;
  // if(backRight > MAX_SENSING_DISTANCE) backRight = 0;
  // if(backLeft > MAX_SENSING_DISTANCE) backLeft = 0;

  //update "Booleans"
  if (frontRight < SENSING_THRESHOLD) {
    objectFR++;
  } else objectFR = 0;

  if (frontLeft < SENSING_THRESHOLD) {
    objectFL++;
  } else objectFL = 0;

  if (backRight < SENSING_THRESHOLD) {
    objectBR++;
  } else objectBR = 0;

  if (backLeft < SENSING_THRESHOLD) {
    objectBL++;
  } else objectBL = 0;


  if (objectFR > 5) {
    searchDirectionFront = 0;  //search CW
  }
  if (objectFL > 5) {
    searchDirectionFront = 1;  //search CCW
  }
  if (objectBR > 5) {
    searchDirectionBack = 1;  //search CCW
  }
  if (objectBL > 5) {
    searchDirectionBack = 0;  //search CW
  }

  // print "Boolean" readings
  // Serial.print(objectFR);
  // Serial.print(",");
  // Serial.print(objectFL);
  // Serial.print(",");
  // Serial.print(objectBL);
  // Serial.print(",");
  // Serial.println(objectBR);


  //constrain to max amounts? This is primarily to help serial plotter be more readable and stop scaling the y axis
  frontRight = constrain(frontRight, 0, 40);
  frontLeft = constrain(frontLeft, 0, 40);
  backRight = constrain(backRight, 0, 40);
  backLeft = constrain(backLeft, 0, 40);


  int closest = min(min(frontLeft, frontRight), min(backLeft, backRight));  // just for LED animation speed
  if (closest < 7) ledAnimationSpeed = LED_ANIMATION_BOOST;
  else if (closest < 11) ledAnimationSpeed = LED_ANIMATION_FAST;
  else ledAnimationSpeed = LED_ANIMATION_SLOW;


  // print out readings
  if (displaySensorReadings == 1) {

    Serial.print(frontRight);
    Serial.print(",");
    Serial.print(frontLeft);
    Serial.print(",");
    Serial.print(backRight);
    Serial.print(",");
    Serial.print(backLeft);
    Serial.print(",");
    Serial.print(0);
    Serial.print(",");
    Serial.println(40);
  }
}

void resetWalk() {
  walkTimer = millis();
  walkState = 0;
}
void chase() {
  Serial.println(walkState);
  if (objectFR > 12 || objectFL > 12) {  //>12 means at least 12 successive sensor detects in a row
    if(displayDirectionDecisions==1)Serial.println("CHASE → FWD");
    forward(255);
    lastDirection = 0;  //fwd
    resetWalk();
  } else if (objectBR > 12 || objectBL > 12) {
    if(displayDirectionDecisions==1)Serial.println("CHASE → BACK");
    backward(255);
    lastDirection = 1;
    resetWalk();
  } else {
    if(displayDirectionDecisions==1)Serial.println("Walk...");
    if (walkState == 0) {
      if (lastDirection == 0) {
        if (searchDirectionFront == 0) {
          pivotRight(60);
        } else {
          pivotLeft(60);
        }
      } else {
        if (searchDirectionBack == 0) {
          pivotRight(60);
        } else {
          pivotLeft(60);
        }
      }
      if (millis() - walkTimer > walkIntervals[0]) {
        walkTimer = millis();
        walkState = 1;  //switch to "F/B movement"
        walkDir = int(random(2));
      }
    } else if (walkState == 1) {  //Small 'walk'
      if (walkDir == 0) {
        forward(100);
      } else {
        backward(100);
      }
      if (millis() - walkTimer > walkIntervals[1]) {
        walkTimer = millis();
        walkState = 2;  //switch to stop
      }
    } else if (walkState == 2) {
      setPins(0, 0, 0, 0);  //stop
      if (millis() - walkTimer > walkIntervals[2]) {
        walkTimer = millis();
        walkState = 0;  //switch to stop
      }
    }
  }
}

void testPattern() {
  //FWD
  Serial.println("FWD");
  setPins(0, 255, 0, 255);
  delay(500);

  //BACK
  Serial.println("BACK");
  setPins(255, 0, 255, 0);
  delay(500);

  //SPIN
  Serial.println("SPIN");
  setPins(255, 0, 0, 255);
  delay(1000);

  //stop
  Serial.println("STOP");
  setPins(0, 0, 0, 0);
  delay(3000);
}

void backward(int power) {
  setPins(0, power, 0, power);
}

void forward(int power) {
  setPins(power, 0, power, 0);
}

void pivotLeft(int power) {  //CCW
  setPins(0, power, power, 0);
}

void pivotRight(int power) {  //CW
  setPins(power, 0, 0, power);
}

void setPins(int a, int b, int c, int d) {
  //Forward Max is: setPins(0,255, 0, 255);

  analogWrite(in1, a);
  analogWrite(in2, b);
  analogWrite(in3, c);
  analogWrite(in4, d);
}

void ledPattern() {
  if (millis() - ledTimer > ledAnimationSpeed) {
    ledsOff();
    ledOn(currentLED);
    currentLED += 1;
    if (currentLED > 3) currentLED = 1;
    ledTimer = millis();
  }
}
