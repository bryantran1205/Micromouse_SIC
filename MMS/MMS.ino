#include <Arduino.h>
#include <Encoder.h>
#include <PID_v1.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FlashStorage.h>
// LiquidCrystal_I2C lcd(0X27, 16, 2);

#define encodPinR 2
#define encodPinL 3
#define R1 5
#define R2 6
#define L1 9
#define L2 11

#define IRLeft A2  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define IRCenter A3
#define IRRight A6  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)

#define MAX_SIZE 256  // Kích thước tối đa của queue

#define MAZE_MAX_HEIGHT 16
#define MAZE_MAX_WIDTH 16

#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3

#define INCREASE 1
#define DECREASE (-1)

typedef struct MazeData {
  int maze[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH];
  int mazeWeight[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH];
  int goalX;
  int goalY;
  int startX;
  int startY;
  int mouseDirection;
} MazeData;

// Tạo vùng lưu trữ trong Flash
FlashStorage(flash_store, MazeData);

int speed = 40;
long targetPulses = 1855;  // Số xung cần thiết để đi 16.8 cm, cái cũ là 1670
Encoder encoderR(encodPinR, encodPinR);
Encoder encoderL(encodPinL, encodPinL);
float IRLeftValue = 0;
float IRRightValue = 0;
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

  readDataFromFlash();
}

void loop() {
  if (hasWallLeft()) {
    if (hasWallFront()) {
      straight();
      return;
    }
    turnRight90();
    return;
  }
  turnLeft90();
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
//hàm đi thẳng 1 block
void goStraight() {
  // Insert Code IoT Here
  // Đặt lại giá trị ban đầu của encoder
  encoderR.write(0);
  encoderL.write(0);

  // Chạy đến khi đạt đủ số xung
  while (true) {
    long rightPulses = encoderR.read();
    long leftPulses = encoderL.read();

    if (rightPulses < targetPulses && leftPulses < targetPulses) {
      straight();
    } else {
      back();
      delay(20);
      stop();
      delay(100);
      adjust();
      break;  // Dừng lại khi đã đạt mục tiêu
    }
  }
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
  return 18 * pow(left, -1);                         // Giá trị từ cảm biến * (5V/1024)Thêm một khoảng thời gian ngắn để tránh nhiễu
}

float getIR45Right() {
  float right = analogRead(IR45Right) * 0.0048828125;  // Giá trị từ cảm biến * (5V/1024)
  return 18 * pow(right, -1);                          // Giá trị từ cảm biến * (5V/1024)
}

//hàm check sensor đằng trước
bool hasWallFront() {
  if (getIRFront() < 10)
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

void turn(int flag_dir) {
  if (flag_dir == LEFT) {
    analogWrite(R1, speed);
    analogWrite(R2, 0);
    analogWrite(L1, 0);
    analogWrite(L2, speed);
  } else {
    analogWrite(R1, 0);
    analogWrite(R2, speed);
    analogWrite(L1, speed);
    analogWrite(L2, 0);
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

    turn(flag_dir);
    while (encoderL.read() <= countEnc && encoderR.read() <= countEnc) {
      // Serial.println(encLeft.read());
      // Serial.println(encRight.read());
      Serial.println("trai");
    }
    turn(RIGHT);

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

  switch (mouseDirection) {
    case 0:
      mouseDirection = 2;
      break;
    case 1:
      mouseDirection = 3;
      break;
    case 2:
      mouseDirection = 1;
      break;
    case 3:
      mouseDirection = 0;
      break;
    default:
      break;
  }
  // Insert Code IoT Here
  turn90(LEFT, 640);
}

//hàm quay xe về phải 90*
void turnRight90() {
  switch (mouseDirection) {
    case 0:
      mouseDirection = 3;
      break;
    case 1:
      mouseDirection = 2;
      break;
    case 2:
      mouseDirection = 0;
      break;
    case 3:
      mouseDirection = 1;
      break;
    default:
      break;
  }
  // Insert Code IoT Here
  turn90(RIGHT, 640);
}

//hàm quay đầu theo bên trái
void turnLeft180() {
  switch (mouseDirection) {
    case 0:
      mouseDirection = 1;
      break;
    case 1:
      mouseDirection = 0;
      break;
    case 2:
      mouseDirection = 3;
      break;
    case 3:
      mouseDirection = 2;
      break;
    default:
      break;
  }
  // Insert Code IoT Here
  turn90(RIGHT, 1360);
}

//hàm quay đầu theo bên phải
void turnRight180() {
  switch (mouseDirection) {
    case 0:
      mouseDirection = 1;
      break;
    case 1:
      mouseDirection = 0;
      break;
    case 2:
      mouseDirection = 3;
      break;
    case 3:
      mouseDirection = 2;
      break;
    default:
      break;
  }
  // Insert Code IoT Here
  turn90(RIGHT, 1360);
}