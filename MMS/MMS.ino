#include <Arduino.h>
#include <Encoder.h>
#include <Wire.h>

#define encodPinR 2
#define encodPinL 3
#define R1 5
#define R2 6
#define L1 9
#define L2 11



#define IRLeft A4    // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define IR45Left A1  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define IRCenter A2
#define IR45Right A5  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define IRRight A7


#define RIGHT (-1)  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define LEFT 1


int speed = 30;
int drift_min = 25;
int drift_max = 50;

long targetPulses = 1855;  // Số xung cần thiết để đi 16.8 cm, cái cũ là 1670
Encoder encoderR(2, 4);
Encoder encoderL(7, 3);



float IRLeftValue = 0;
float IRRightValue = 0;
float IR45LeftValue = 0;
float IR45RightValue = 0;
float IRCenterValue = 0;

int flag = 0;
int flag_distance = 0;
int flag_quay = 0;
volatile int leftCount = 0;
volatile int rightCount = 0;

void straight();

void stop();

void goStraight();

bool hasWallFront();

bool hasWallLeft();

bool hasWallRight();

void turnLeft90();

void turnRight90();

void turnLeft180();

void turnRight180();

void setup() {
  Serial.begin(9600);
  // Đặt giá trị khởi tạo của encoder về 0
  initMotor();
  pinMode(24, OUTPUT);
  digitalWrite(24, HIGH);

  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);
  encoderR.write(0);
  encoderL.write(0);
}

float getIRFront() {
  float center = analogRead(IRCenter) * 0.0048828125;  // Giá trị từ cảm biến * (5V/1024)
  IRCenterValue = 18 * pow(center, -1);                // Giá trị từ cảm biến * (5V/1024)
  return IRCenterValue;
}

float getIRLeft() {
  float left = analogRead(IRLeft) * 0.0048828125;  // Giá trị từ cảm biến * (5V/1024)Thêm một khoảng thời gian ngắn để tránh nhiễu
  IRLeftValue = 18 * pow(left, -1) - 0.3;          // Giá trị từ cảm biến * (5V/1024)Thêm một khoảng thời gian ngắn để tránh nhiễu
  return IRLeftValue;
}

float getIRRight() {
  float right = analogRead(IRRight) * 0.0048828125;  // Giá trị từ cảm biến * (5V/1024)
  IRRightValue = 18 * pow(right, -1);                // Giá trị từ cảm biến * (5V/1024)
  return IRRightValue;
}

float getIR45Left() {
  float left = analogRead(IR45Left) * 0.0048828125;  // Giá trị từ cảm biến * (5V/1024)Thêm một khoảng thời gian ngắn để tránh nhiễu
  IR45LeftValue = 18 * pow(left, -1) - 0.3;          // Giá trị từ cảm biến * (5V/1024)Thêm một khoảng thời gian ngắn để tránh nhiễu
  return IR45LeftValue;
}

float getIR45Right() {
  float right = analogRead(IR45Right) * 0.0048828125;  // Giá trị từ cảm biến * (5V/1024)
  IR45RightValue = 18 * pow(right, -1);                // Giá trị từ cảm biến * (5V/1024)
  return IR45RightValue;
}

void goStraight() {

  if (getIR45Left() < 4.0 && !hasWallFront()) {
    back();
    delay(10);
    stop();
    delay(20);
    turnBalance(RIGHT);
    delay(25);
    turnBalance(LEFT);
    delay(10);

  } else if (getIR45Left() > 8.5 && !hasWallFront()) {
    back();
    delay(10);
    stop();
    delay(20);
    turnBalance(LEFT);
    delay(25);
    turnBalance(RIGHT);
    delay(10);
  }
  if (getIRFront() < 6) {
    stop();
  } else {
    straight();
  }
}

//hàm check sensor đằng trước
bool hasWallFront() {
  if (getIRFront() < 8)
    return true;
  return false;
}

// hàm check sensor bên trái
bool hasWallLeft() {
  if (getIRLeft() < 10)
    return true;
  return false;
}

// hàm check sensor bên phải
bool hasWallRight() {
  if (getIRRight() < 10)
    return true;
  return false;
}

bool hasWallLeft45() {
  if (getIR45Left() < 16)
    return true;
  return false;
}

bool hasWallRight45() {
  if (getIR45Right() < 16)
    return true;
  return false;
}

void turnBalance(int flag_dir) {
  if (flag_dir == LEFT) {
    analogWrite(R1, 40);
    analogWrite(R2, 0);
    analogWrite(L1, 0);
    analogWrite(L2, 40);
  } else {
    analogWrite(R1, 0);
    analogWrite(R2, 40);
    analogWrite(L1, 40);
    analogWrite(L2, 0);
  }
}

void initMotor() {
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);

  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
}

void straight() {
  analogWrite(R1, speed);
  analogWrite(R2, 0);
  analogWrite(L1, speed);
  analogWrite(L2, 0);
}
void reverse() {
  back();
  delay(20);
  stop();
  delay(40);
}
void stop() {
  analogWrite(R1, 0);
  analogWrite(R2, 0);
  analogWrite(L1, 0);
  analogWrite(L2, 0);
}

void back() {
  analogWrite(R1, 0);
  analogWrite(R2, speed);
  analogWrite(L1, 0);
  analogWrite(L2, speed);
}
void drift(int flag_dir) {
  if (flag_dir == LEFT) {
    analogWrite(R1, drift_max);
    analogWrite(R2, 0);
    analogWrite(L1, drift_min);
    analogWrite(L2, 0);
  } else {
    analogWrite(R1, drift_min);
    analogWrite(R2, 0);
    analogWrite(L1, drift_max);
    analogWrite(L2, 0);
  }
}
void turn(int flag_dir) {
  drift(flag_dir);
  delay(150);
  while (getIR45Left() > 8) {
    drift(flag_dir);
    delay(20);
  }
  drift(-1 * flag_dir);
  delay(50);
  // if (!hasWallLeft()) {
  //   while (getIRFront() > 12) {
  //     straight();
  //   }
  // }
  back();
  delay(40);
  stop();
  delay(20);
}
void turn90(int flag_dir, int countEnc) {
  reverse();
  encoderR.write(0);
  encoderL.write(0);
  if (flag_dir == LEFT) {

    while (encoderL.read() <= countEnc && encoderR.read() <= countEnc) {
      turnBalance(flag_dir);
    }
    turnBalance(RIGHT);

  } else if (flag_dir == RIGHT) {
    digitalWrite(24, LOW);
    while (encoderL.read() <= countEnc && encoderR.read() <= countEnc) {
      // Serial.println(encLeft.read());
      // Serial.prencRightintln(.read());
      turnBalance(flag_dir);
    }
    turnBalance(LEFT);
    digitalWrite(24, HIGH);
  }
  delay(10);
  straight();
  delay(200);
  reverse();
}
// void loop() {

//   if (!hasWallLeft()) {
//     // reverse();
//     straight();
//     delay(150);
//     speed = 40;
//     turn90(LEFT, 620);
//     straight();
//     delay(150);
//     return;
//   }
//   if (!hasWallRight() && getIRFront() < 8) {
//     // reverse();
//     straight();
//     delay(150);
//     speed = 40;
//     turn90(RIGHT, 620);
//     straight();
//     delay(150);
//     return;
//   }

//   speed = 80;
//   goStraight();
// }
void trai() {
  
  straight();
  delay(200);
  while(!hasWallLeft())
  {
analogWrite(R1, 60);
  analogWrite(R2, 0);
  analogWrite(L1, 30);
  analogWrite(L2, 0);
  }
analogWrite(R1, 30);
  analogWrite(R2, 0);
  analogWrite(L1, 60);
  analogWrite(L2, 0);
  delay(20);
  stop();
  delay(10000);
 

}

void teststraight() {
  float temp = getIR45Left();

  if (temp <= 4) {

    digitalWrite(24, LOW);
    analogWrite(R1, 0);
    analogWrite(R2, 40);
    analogWrite(L1, 40);
    analogWrite(L2, 0);
    delay(20);
    analogWrite(R1, 40);
    analogWrite(R2, 0);
    analogWrite(L1, 0);
    analogWrite(L2, 40);
    delay(10);
  } else if (temp >= 7.0) {
    digitalWrite(24, LOW);

    analogWrite(R1, 40);
    analogWrite(R2, 0);
    analogWrite(L1, 0);
    analogWrite(L2, 40);
    delay(20);
    analogWrite(R1, 0);
    analogWrite(R2, 40);
    analogWrite(L1, 40);
    analogWrite(L2, 0);
    delay(10);
  } else {
    digitalWrite(24, HIGH);
    analogWrite(R1, 40);
    analogWrite(R2, 0);
    analogWrite(L1, 40);
    analogWrite(L2, 0);
  }
}

void loop() {
    digitalWrite(23, HIGH);

  if(getIR45Left() < 9)
{
    digitalWrite(23, LOW);

}
 if (!hasWallLeft45()) {
    trai();
    return ;
  }
  if (getIR45Left() < 8) {
    teststraight();
    return;
  } 
 
  if (hasWallLeft45() &&  getIRFront() <= 6)
  {
    turn90(RIGHT,650);
    return ;
  }
}