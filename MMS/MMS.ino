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
#define IR45Left A3  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define IRCenter A2
#define IR45Right A5  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define IRRight A6
#define RIGHT 3  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define LEFT 2
int speed = 40;
long targetPulses = 1855;  // Số xung cần thiết để đi 16.8 cm, cái cũ là 1670
Encoder encoderR(2, 4);
Encoder encoderL(3, 7);
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
  // lcd.init();
  // lcd.backlight();

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(24, OUTPUT);

  pinMode(encodPinR, INPUT_PULLUP);
  pinMode(encodPinL, INPUT_PULLUP);

  encoderR.write(0);
  encoderL.write(0);

  // readDataFromFlash();
}

void loop() {
  // if (hasWallLeft45()){
  //   if (hasWallFront()){
  //     if (hasWallRight45()){
  //       turnRight180();
  //       return;
  //     }
  //     turnRight90();
  //     return;
  //   }
  //   goStraight();
  //   return;
  // }
  // goStraight();
  // if (!hasWallLeft45()) {

  //   turnLeft90();
  // }
  turnRight90();
  delay(1000);
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
  straight();

  // if (getIR45Left() < 5.0 && !hasWallFront())
  // {
  //   back();
  //   delay(20);
  //   stop();
  //   delay(20);
  //   turn(RIGHT);
  //   delay(30);
  //   turn(LEFT);
  //   delay(10);

  // }
  // else if (getIR45Left() > 11.5 && !hasWallFront())
  // {
  //   back();
  //   delay(20);
  //   stop();
  //   delay(20);
  //   turn(LEFT);
  //   delay(30);
  //   turn(RIGHT);
  //   delay(10);
  // }
}

//hàm check sensor đằng trước
bool hasWallFront() {
  if (getIRFront() < 16)
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
  if (getIR45Left() < 14)
    return true;
  return false;
}

bool hasWallRight45() {
  if (getIR45Right() < 14)
    return true;
  return false;
}

void turn(int flag_dir) {
  if (flag_dir == LEFT) {
    analogWrite(R1, 60);
    analogWrite(R2, 0);
    analogWrite(L1, 40);
    analogWrite(L2, 0);
  } else {
    analogWrite(R1, 0);
    analogWrite(R2, 40);
    analogWrite(L1, 0);
    analogWrite(L2, 60);
  }
}

void turn90(int flag_dir, int countEnc) {
  back();
  delay(20);
  stop();
  delay(40);
  encoderR.write(0);
  encoderL.write(0);
  if (flag_dir == LEFT) {

    analogWrite(R1, 60);
    analogWrite(R2, 0);
    analogWrite(L1, 0);
    analogWrite(L2, 60);
    while (encoderL.read() <= countEnc && encoderR.read() <= countEnc) {
    }
    analogWrite(R1, 0);
    analogWrite(R2, 60);
    analogWrite(L1, 60);
    analogWrite(L2, 0);
    delay(20);
  } else if (flag_dir == RIGHT) {
    turn(flag_dir);
    while (encoderL.read() <= countEnc && encoderR.read() <= countEnc) {
      // Serial.println(encLeft.read());
      // Serial.println(encRight.read());
      Serial.println("trai");
    }
    turn(LEFT);
  }
  delay(40);
  stop();
  delay(150);
  // flag_quay = 1;
}

//hàm quay xe về trái 90*
void turnLeft90() {
  turn(LEFT);

  while (getIR45Left() > 6.0) {
    turn(LEFT);
  }
  turn(RIGHT);
  delay(30);
  stop();
  delay(10000);
}

//hàm quay xe về phải 90*
void turnRight90() {
  // Insert Code IoT Here
  turn90(RIGHT, 640);
}

//hàm quay đầu theo bên trái
void turnLeft180() {
  // Insert Code IoT Here
  // turn90(RIGHT, 1360);
  turn90(LEFT, 1300);
}

//hàm quay đầu theo bên phải
void turnRight180() {
  // Insert Code IoT Here
  turn90(LEFT, 650);
}