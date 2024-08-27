#include <Arduino.h>
#include <Encoder.h>
#include <PID_v1.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
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
#define IR45Left A4  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)
#define IR45Right A5  // Sharp IR GP2Y0A41SK0F (4-30cm, analog)


#define MAX_SIZE 256  // Kích thước tối đa của queue

#define MAZE_MAX_HEIGHT 16
#define MAZE_MAX_WIDTH 16

#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3

#define INCREASE 1
#define DECREASE (-1)

typedef struct Coordinate {
  int x;
  int y;
} Coord;

class Stack {
private:
  Coordinate arr[MAX_SIZE];
  int topIndex;

public:
  Stack()
    : topIndex(-1) {}

  bool isEmpty() {
    return topIndex == -1;
  }

  bool isFull() {
    return topIndex >= MAX_SIZE - 1;
  }

  void push(Coordinate value) {
    if (isFull()) {
      return;
    }
    arr[++topIndex] = value;
  }

  void pop() {
    if (isEmpty()) {
      return;
    }
    topIndex--;
  }

  Coordinate top() {
    if (isEmpty()) {
      return { -1, -1 };  // Trả về giá trị không hợp lệ hoặc xử lý lỗi theo yêu cầu
    }
    return arr[topIndex];
  }

  int size() {
    return topIndex + 1;  // Trả về số lượng phần tử hiện tại trong stack
  }
};

class Queue {
private:
  Coordinate arr[MAX_SIZE];
  int frontIndex;
  int rearIndex;
  int currentSize;

public:
  Queue()
    : frontIndex(0), rearIndex(-1), currentSize(0) {}

  bool isEmpty() {
    return currentSize == 0;
  }

  bool isFull() {
    return currentSize == MAX_SIZE;
  }

  void push(Coordinate value) {
    if (isFull()) {
      return;
    }
    rearIndex = (rearIndex + 1) % MAX_SIZE;
    arr[rearIndex] = value;
    currentSize++;
  }

  void pop() {
    if (isEmpty()) {
      return;
    }
    frontIndex = (frontIndex + 1) % MAX_SIZE;
    currentSize--;
  }

  Coordinate top() {
    if (isEmpty()) {
      return { -1, -1 };  // Trả về giá trị không hợp lệ hoặc xử lý lỗi theo yêu cầu
    }
    return arr[frontIndex];
  }

  int size() {
    return currentSize;  // Trả về số lượng phần tử hiện tại trong queue
  }
};

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
Stack visitedCoord;

int mazeWeight[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH];

int maze[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] = {
  { 13, 5, 3, 3, 3, 3, 3, 1, 3, 3, 9, 5, 1, 3, 1, 9 },
  { 12, 12, 5, 9, 5, 1, 9, 6, 1, 9, 12, 12, 6, 9, 12, 12 },
  { 12, 12, 12, 6, 10, 12, 4, 9, 12, 12, 12, 6, 9, 12, 12, 14 },
  { 12, 12, 4, 9, 5, 10, 12, 6, 10, 12, 4, 3, 2, 10, 6, 9 },
  { 4, 2, 10, 12, 6, 9, 6, 3, 9, 6, 2, 3, 3, 3, 3, 8 },
  { 6, 3, 9, 6, 3, 2, 1, 9, 6, 3, 3, 9, 7, 3, 9, 12 },
  { 5, 3, 10, 5, 3, 1, 10, 6, 3, 1, 9, 12, 5, 3, 10, 12 },
  { 12, 5, 9, 4, 9, 6, 9, 5, 1, 10, 12, 12, 4, 3, 3, 10 },
  { 12, 12, 6, 10, 12, 5, 10, 6, 10, 5, 10, 12, 6, 3, 1, 9 },
  { 12, 12, 5, 9, 12, 12, 5, 3, 9, 6, 9, 12, 5, 3, 10, 12 },
  { 12, 4, 10, 4, 10, 6, 10, 5, 2, 9, 4, 8, 4, 3, 3, 10 },
  { 4, 10, 5, 10, 5, 3, 3, 2, 9, 6, 10, 12, 6, 3, 1, 9 },
  { 4, 3, 10, 5, 10, 5, 3, 9, 6, 3, 9, 6, 3, 3, 10, 12 },
  { 12, 5, 9, 12, 5, 10, 13, 12, 5, 9, 12, 5, 3, 3, 9, 12 },
  { 6, 8, 6, 10, 12, 5, 2, 2, 10, 14, 12, 12, 5, 11, 12, 12 },
  { 7, 2, 3, 3, 2, 2, 3, 3, 3, 3, 2, 10, 6, 3, 2, 10 }
};
// Algorithm
bool mazeVisited[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] = { false };
int mouseDirection = 0;
int startX = 15, startY = 15;
int mouseX = startX, mouseY = startY;
int goalX = 0, goalY = 0;
Stack startToGoal;
Stack goalToStart;

void PID(void)       
{    
    int hasRightWall = 10;
    int hasLeftWall = 10;
    if((leftSensor > hasLeftWall && rightSensor > hasRightWall))//has both walls
    {  //ccw direction is positive
        errorP = rightSensor – leftSensor – 63;//63 is the offset between left and right sensor when mouse in the middle of cell
        errorD = errorP – oldErrorP;
    }        
    else if((leftSensor > hasLeftWall))//only has left wall
    {
        errorP = 2 * (leftMiddleValue – leftSensor);
        errorD = errorP – oldErrorP;
    }
    else if((rightSensor > hasRightWall))//only has right wall
    {
        errorP = 2 * (rightSensor – rightMiddleValue);
        errorD = errorP – oldErrorP;
    }
    else if((leftSensor < hasLeftWall && rightSensor <hasRightWall))//no wall, use encoder or gyro
    {
        errorP = 0;//(leftEncoder – rightEncoder*1005/1000)*3;
        errorD = 0;
    }
    totalError = P * errorP + D * errorD;
    oldErrorP = errorP;
    setLeftPwm(leftBaseSpeed – totalError);
    setRightPwm(rightBaseSpeed + totalError);    
}

bool isInStack(Stack stack, Coordinate coord);

Stack reverseStack(Stack stack);

bool compareCoordinate(Coordinate coord1, Coordinate coord2);

void updatePosition(int direction, int x, int y);

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

void turnLeft45();

void turnRight45();

void turnLeft135();

void turnRight135();

void startFloodFill2();

void startFloodFill();

Coordinate getFrontCord();

Coordinate getBackCord();

Coordinate getLeftCord();

Coordinate getRightCord();

bool isVisited();

bool isCoordVisited(Coordinate coord);

bool onFront(Coordinate coord);

bool onTheLeft(Coordinate coord);

bool onTheRight(Coordinate coord);

bool onBehind(Coordinate coord);

bool isGoal(Coordinate coord);

void setGoal(Coordinate coord);

void findGoal();

void initMazeWeight();

void floodFill();

bool hasWallTopByMaze(Coordinate coord);

bool hasWallBottomByMaze(Coordinate coord);

bool hasWallLeftByMaze(Coordinate coord);

bool hasWallRightByMaze(Coordinate coord);

int getCurrentWeight(Coordinate coord);

int getTopWeight(Coordinate coord);

int getBottomWeight(Coordinate coord);

int getLeftWeight(Coordinate coord);

int getRightWeight(Coordinate coord);

Coordinate getTopCoordByCoordinate(Coordinate coord);

Coordinate getBottomCoordByCoordinate(Coordinate coord);

Coordinate getLeftCoordByCoordinate(Coordinate coord);

Coordinate getRightCoordByCoordinate(Coordinate coord);

int getSmallestWeightAround(Coordinate coord);

Stack findShortestPath(Coordinate start, Coordinate goal, int mode);

void followPath(Stack path);

void adjust();

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

  // attachInterrupt(digitalPinToInterrupt(encodPinR), demRight, RISING);
  // attachInterrupt(digitalPinToInterrupt(encodPinL), demLeft, RISING);
  encoderR.write(0);
  encoderL.write(0);
  startFloodFill();
  findGoal();
  initMazeWeight();
  floodFill();
  goalToStart = findShortestPath({ startX, startY }, { goalX, goalY }, DECREASE);
  startToGoal = reverseStack(goalToStart);
  followPath(reverseStack(findShortestPath({ mouseX, mouseY }, { goalX, goalY }, DECREASE)));
}

void loop() {
  digitalWrite(24, HIGH);
  followPath(goalToStart);
  followPath(startToGoal);
  // lcd.setCursor(0, 0);
  // lcd.print("Test");
  // delay(10000);
}

void demLeft() {
  leftCount++;
}

void demRight() {
  rightCount++;
}

void initMotor() {
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);

  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
}

bool isInStack(Stack stack, Coordinate coord) {
  while (!stack.isEmpty()) {
    if (compareCoordinate(stack.top(), coord)) {
      return true;
    }
    stack.pop();
  }
  return false;
}

Stack reverseStack(Stack stack) {
  Stack reverseStack;
  while (!stack.isEmpty()) {
    reverseStack.push(stack.top());
    stack.pop();
  }
  return reverseStack;
}

bool compareCoordinate(Coordinate coord1, Coordinate coord2) {
  return coord1.x == coord2.x && coord1.y == coord2.y;
}

void updatePosition(int direction, int x, int y) {
  mouseDirection = direction;
  mouseX = x;
  mouseY = y;
  mazeVisited[mouseY][mouseX] = true;
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
void adjust() {

  // if (theta_degrees > 15.0 && theta_degrees < 10.0) {
  //   turn(RIGHT);
  //   delay(60);

  // } else if (theta_degrees < -15.0 && theta_degrees > -10.0) {
  //   turn(LEFT);

  //   delay(60);
  // }

  // else if (theta_degrees > 10.0 && theta_degrees < 5.0) {
  //   turn(RIGHT);
  //   delay(40);

  // } else if (theta_degrees < -10.0 && theta_degrees > -5.0) {
  //   turn(LEFT);

  //   delay(40);
  // }

  //   else
  if (hasWallLeft() && hasWallRight()) {
    float theta = atan((getIRRight() - getIRLeft()) / 4.0);
    float theta_degrees = theta * (180.0 / PI);
    if (theta_degrees > 15.0) {
      turn(RIGHT);
      delay(40);
    } else if (theta_degrees < -15.0) {
      turn(LEFT);

      delay(40);
    }
  }
  else if (!hasWallLeft() && hasWallRight())
  {
    float theta = atan((getIRRight() - 6.4) / 4.0);
    float theta_degrees = theta * (180.0 / PI);
    if (theta_degrees > 15.0) {
      turn(RIGHT);
      delay(40);
    } else if (theta_degrees < -15.0) {
      turn(LEFT);

      delay(40);
    }
  }
  else if (hasWallLeft() && !hasWallRight())
  {
    float theta = atan((6.4 - getIRLeft() ) / 4.0);
    float theta_degrees = theta * (180.0 / PI);
    if (theta_degrees > 15.0) {
      turn(RIGHT);
      delay(40);
    } else if (theta_degrees < -15.0) {
      turn(LEFT);

      delay(40);
    }
  }
}
//hàm đi thẳng 1 block
// void goStraight() {
  // switch (mouseDirection) {
  //   case TOP:
  //     // if (!maze[mouseY][mouseX] & 1 && mouseY > 0) {
  //     updatePosition(mouseDirection, mouseX, mouseY - 1);
  //     // }
  //     break;

  //   case BOTTOM:
  //     // if (!maze[mouseY][mouseX] & 2 && mouseY < MAZE_MAX_HEIGHT - 1) {
  //     updatePosition(mouseDirection, mouseX, mouseY + 1);
  //     // }
  //     break;

  //   case LEFT:
  //     // if (!maze[mouseY][mouseX] & 4 && mouseX > 0) {
  //     updatePosition(mouseDirection, mouseX - 1, mouseY);
  //     // }
  //     break;

  //   case RIGHT:
  //     // if (!maze[mouseY][mouseX] & 8 && mouseX < MAZE_MAX_WIDTH - 1) {
  //     updatePosition(mouseDirection, mouseX + 1, mouseY);
  //     // }
  //     break;

  //   default:
  //     break;
  // }
  // visitedCoord.push({ mouseX, mouseY });
  // // Insert Code IoT Here
  // // Đặt lại giá trị ban đầu của encoder
  // encoderR.write(0);
  // encoderL.write(0);

  // // Chạy đến khi đạt đủ số xung
  // while (true) {
  //   long rightPulses = encoderR.read();
  //   long leftPulses = encoderL.read();

  //   if (rightPulses < targetPulses && leftPulses < targetPulses) {
  //     straight();
  //   } else {
  //     back();
  //     delay(20);
  //     stop();
  //     delay(200);
  //     adjust();
  //     break;  // Dừng lại khi đã đạt mục tiêu
  //   }
  // }
// }

void goStraight(){
  int currentVoltage;
int voltageChange = 5;
int D1 = pwmRead(L45sensor) - pwmRead(R45sensor);
delay(20);
int D2 = pwmRead(L45sensor) - pwmRead(R45sensor);

if(D1 == D2)
;
else if(D1 < D2)
{
if(voltageChange < 0)
ChangeLeft(voltageChange);
else
ChangeRight(voltageChange);
}
else if(D1 > D2)
{
if(voltageChange < 0)
ChangeRight(voltageChange);
else
ChangeLeft(voltageChange);
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

float getIRLeft() {
  float left = analogRead(IR45Left) * 0.0048828125;  // Giá trị từ cảm biến * (5V/1024)Thêm một khoảng thời gian ngắn để tránh nhiễu
  IR45LeftValue = 18 * pow(left, -1) - 0.3;          // Giá trị từ cảm biến * (5V/1024)Thêm một khoảng thời gian ngắn để tránh nhiễu
  return IR45LeftValue;
}

float getIR45Right() {
  float right = analogRead(IR45Right) * 0.0048828125;  // Giá trị từ cảm biến * (5V/1024)
  IR45RightValue = 18 * pow(right, -1);                // Giá trị từ cảm biến * (5V/1024)
  return IR45RightValue;
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

int getMazeCode() {
  int code = 0;
  switch (mouseDirection) {
    case TOP:
      if (hasWallFront())
        code = code | 1;
      if (hasWallLeft())
        code = code | 4;
      if (hasWallRight())
        code = code | 8;
      return code;
    case BOTTOM:
      if (hasWallFront())
        code = code | 2;
      if (hasWallLeft())
        code = code | 8;
      if (hasWallRight())
        code = code | 4;
      return code;
    case LEFT:
      if (hasWallFront())
        code = code | 4;
      if (hasWallLeft())
        code = code | 2;
      if (hasWallRight())
        code = code | 1;
      return code;
    case RIGHT:
      if (hasWallFront())
        code = code | 8;
      if (hasWallLeft())
        code = code | 1;
      if (hasWallRight())
        code = code | 2;
      return code;
    default:
      return -1;
  }
  return -1;
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

void turnLeft45() {
  switch (mouseDirection) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}

void turnRight45() {
  switch (mouseDirection) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}

void turnLeft135() {
  switch (mouseDirection) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}

void turnRight135() {
  switch (mouseDirection) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}

void goToCoord(int x, int y) {
}

void startFloodFill() {
  Stack stackStep;
  maze[mouseY][mouseX] = getMazeCode();
  // Add neighboring unvisited coordinates to the stack
  if (!hasWallLeft() && getLeftCord().x != -1 && getLeftCord().y != -1) {
    stackStep.push(getLeftCord());
  }
  if (!hasWallRight() && getRightCord().x != -1 && getRightCord().y != -1) {
    stackStep.push(getRightCord());
  }
  if (!hasWallFront() && getFrontCord().x != -1 && getFrontCord().y != -1) {
    stackStep.push(getFrontCord());
  }


  visitedCoord.push({ mouseX, mouseY });
  // Mark the starting point
  mazeWeight[mouseY][mouseX] = 0;
  mazeVisited[mouseY][mouseX] = true;

  while (!stackStep.isEmpty()) {
    maze[mouseY][mouseX] = getMazeCode();
    Coordinate next = stackStep.top();  // Peek the top coordinate
    if (isCoordVisited(next)) {
      stackStep.pop();
      continue;
    }
    // Adjust direction to face the current target
    if (onFront(next) && !hasWallFront()) {
      goStraight();
      // Mark the position as visited after moving
      maze[mouseY][mouseX] = getMazeCode();
      mazeVisited[mouseY][mouseX] = true;
      stackStep.pop();
    } else if (onTheLeft(next) && !hasWallLeft()) {
      turnLeft90();
      continue;
    } else if (onTheRight(next) && !hasWallRight()) {
      turnRight90();
      continue;
    } else if (onBehind(next)) {
      turnRight180();
    } else {
      turnRight180();
      visitedCoord.pop();
      while (true) {
        if (visitedCoord.top().x != next.x || visitedCoord.top().y != next.y) {
          if (visitedCoord.top().x == mouseX && visitedCoord.top().y == mouseY) {
            visitedCoord.pop();
            continue;
          }
          if (onTheLeft(visitedCoord.top())) {
            turnLeft90();
            continue;
          }
          if (onTheRight(visitedCoord.top())) {
            turnRight90();
            continue;
          }
          if (onBehind(visitedCoord.top())) {
            turnRight180();
            continue;
          }
          if (onFront(visitedCoord.top())) {
            goStraight();
            maze[mouseY][mouseX] = getMazeCode();
            visitedCoord.pop();
            // visitedCoord.pop();
          }
        }
        if (onTheLeft(next) && !hasWallLeft()) {
          turnLeft90();
          break;
        }
        if (onTheRight(next) && !hasWallRight()) {
          turnRight90();
          break;
        }
        if (onFront(next) && !hasWallFront()) {
          break;
        }
      }
      continue;
    }

    maze[mouseY][mouseX] = getMazeCode();
    // Add neighboring unvisited coordinates to the stack
    if (!hasWallLeft() && !isCoordVisited(getLeftCord()) && getLeftCord().x != -1 && getLeftCord().y != -1) {
      stackStep.push(getLeftCord());
    }
    if (!hasWallRight() && !isCoordVisited(getRightCord()) && getRightCord().x != -1 && getRightCord().y != -1) {
      stackStep.push(getRightCord());
    }
    if (!hasWallFront() && !isCoordVisited(getFrontCord()) && getFrontCord().x != -1 && getFrontCord().y != -1) {
      stackStep.push(getFrontCord());
    }

    // if (mouseX == 13 && mouseY == 13) {
    //   digitalWrite(24, HIGH);
    //   delay(1000);
    //   digitalWrite(24, LOW);
    //   delay(2000);
    // }
    // if (mouseX == 11 && mouseY == 12 && maze[mouseY][mouseX] == 6) {
  }
}

Coordinate getFrontCord() {
  switch (mouseDirection) {
    case 0:
      if (mouseY > 0) {
        return { mouseX, mouseY - 1 };
      }
      return { mouseX, -1 };

    case 1:
      if (mouseY < MAZE_MAX_HEIGHT - 1) {
        return { mouseX, mouseY + 1 };
      }
      return { mouseX, -1 };

    case 2:
      if (mouseX > 0) {
        return { mouseX - 1, mouseY };
      }
      return { -1, mouseY };

    case 3:
      if (mouseX < MAZE_MAX_WIDTH - 1) {
        return { mouseX + 1, mouseY };
      }
      return { -1, mouseY };

    default:
      return { -1, -1 };
  }
}

Coordinate getBackCord() {
  switch (mouseDirection) {
    case 0:
      if (mouseY < MAZE_MAX_HEIGHT - 1) {
        return { mouseX, mouseY + 1 };
      }

      return { mouseX, -1 };

    case 1:
      if (mouseY > 0) {
        return { mouseX, mouseY - 1 };
      }
      return { mouseX, -1 };

    case 2:
      if (mouseX < MAZE_MAX_WIDTH - 1) {
        return { mouseX + 1, mouseY };
      }
      return { -1, mouseY };

    case 3:

      if (mouseX > 0) {
        return { mouseX - 1, mouseY };
      }
      return { -1, mouseY };

    default:
      return { -1, -1 };
  }
}

Coordinate getLeftCord() {
  switch (mouseDirection) {
    case 0:
      if (mouseX > 0) {
        return { mouseX - 1, mouseY };
      }
      return { -1, mouseY };

    case 1:
      if (mouseX < MAZE_MAX_WIDTH - 1) {
        return { mouseX + 1, mouseY };
      }
      return { -1, mouseY };

    case 2:
      if (mouseY < MAZE_MAX_HEIGHT - 1) {
        return { mouseX, mouseY + 1 };
      }
      return { mouseX, -1 };

    case 3:
      if (mouseY > 0) {
        return { mouseX, mouseY - 1 };
      }
      return { mouseX, -1 };

    default:
      return { -1, -1 };
  }
}

Coordinate getRightCord() {
  switch (mouseDirection) {
    case 0:
      if (mouseX < MAZE_MAX_WIDTH - 1) {
        return { mouseX + 1, mouseY };
      }
      return { -1, mouseY };

    case 1:
      if (mouseX > 0) {
        return { mouseX - 1, mouseY };
      }
      return { -1, mouseY };

    case 2:
      if (mouseY > 0) {
        return { mouseX, mouseY - 1 };
      }
      return { mouseX, -1 };

    case 3:
      if (mouseY < MAZE_MAX_HEIGHT - 1) {
        return { mouseX, mouseY + 1 };
      }
      return { mouseX, -1 };

    default:
      return { -1, -1 };
  }
}

bool isVisited() {
  return mazeVisited[mouseY][mouseX];
}

bool isCoordVisited(Coordinate coord) {
  if (coord.x == -1 || coord.y == -1 || coord.x == 16 || coord.y == 16) {
    return true;
  }
  return mazeVisited[coord.y][coord.x];
}

bool onFront(Coordinate coord) {
  switch (mouseDirection) {
    case TOP:
      if (coord.x == mouseX && coord.y == mouseY - 1) {
        return true;
      }
      break;

    case BOTTOM:
      if (coord.x == mouseX && coord.y == mouseY + 1) {
        return true;
      }
      break;

    case LEFT:
      if (coord.x == mouseX - 1 && coord.y == mouseY) {
        return true;
      }
      break;

    case RIGHT:
      if (coord.x == mouseX + 1 && coord.y == mouseY) {
        return true;
      }
      break;

    default:
      break;
  }
  return false;
}

bool onTheLeft(Coordinate coord) {
  switch (mouseDirection) {
    case TOP:
      if (coord.x == mouseX - 1 && coord.y == mouseY) {
        return true;
      }
      break;

    case BOTTOM:
      if (coord.x == mouseX + 1 && coord.y == mouseY) {
        return true;
      }
      break;

    case LEFT:
      if (coord.x == mouseX && coord.y == mouseY + 1) {
        return true;
      }
      break;

    case RIGHT:
      if (coord.x == mouseX && coord.y == mouseY - 1) {
        return true;
      }
      break;

    default:
      break;
  }
  return false;
}

bool onTheRight(Coordinate coord) {
  switch (mouseDirection) {
    case TOP:
      if (coord.x == mouseX + 1 && coord.y == mouseY) {
        return true;
      }
      break;

    case BOTTOM:
      if (coord.x == mouseX - 1 && coord.y == mouseY) {
        return true;
      }
      break;

    case LEFT:
      if (coord.x == mouseX && coord.y == mouseY - 1) {
        return true;
      }
      break;

    case RIGHT:
      if (coord.x == mouseX && coord.y == mouseY + 1) {
        return true;
      }
      break;

    default:
      break;
  }
  return false;
}

bool onBehind(Coordinate coord) {
  switch (mouseDirection) {
    case TOP:
      if (coord.x == mouseX && coord.y == mouseY + 1) {
        return true;
      }
      break;

    case BOTTOM:
      if (coord.x == mouseX && coord.y == mouseY - 1) {
        return true;
      }
      break;

    case LEFT:
      if (coord.x == mouseX + 1 && coord.y == mouseY) {
        return true;
      }
      break;

    case RIGHT:
      if (coord.x == mouseX - 1 && coord.y == mouseY) {
        return true;
      }
      break;

    default:
      break;
  }
  return false;
}

bool isGoal(Coordinate coord) {
  /*
     +   +---+            +---+---+             TOP-LEFT:       1 || 4
     |       |                    |             TOP-RIGHT:      9
     +   +   +     OR     +   +   +             BOTTOM-LEFT:    6
     |       |            |       |             BOTTOM-RIGHT:   10
     +---+---+            +---+---+
    */
  if ((maze[coord.y][coord.x] == 1 || maze[coord.y][coord.x] == 4)  //Top-Left
      && maze[coord.y][coord.x + 1] == 9                            //Top-Right
      && maze[coord.y + 1][coord.x] == 6                            //Bottom-Left
      && maze[coord.y + 1][coord.x + 1] == 10)                      //Bottom-Right
  {
    setGoal({ coord.x, coord.y });
    return true;
  }
  /*
     +---+   +            +---+---+             TOP-LEFT:       5
     |       |            |                     TOP-RIGHT:      1 || 8
     +   +   +     OR     +   +   +             BOTTOM-LEFT:    6
     |       |            |       |             BOTTOM-RIGHT:   10
     +---+---+            +---+---+
    */
  if (maze[coord.y][coord.x] == 5                                              //Top-Left
      && (maze[coord.y][coord.x + 1] == 1 || maze[coord.y][coord.x + 1] == 8)  //Top-Right
      && maze[coord.y + 1][coord.x] == 6                                       //Bottom-Left
      && maze[coord.y + 1][coord.x + 1] == 10)                                 //Top-Right
  {
    setGoal({ coord.x + 1, coord.y });
    return true;
  }
  /*
     +---+---+            +---+---+             TOP-LEFT:       5
     |       |            |       |             TOP-RIGHT:      9
     +   +   +     OR     +   +   +             BOTTOM-LEFT:    2 || 4
             |            |       |             BOTTOM-RIGHT:   10
     +---+---+            +   +---+
    */
  if (maze[coord.y][coord.x] == 5                                              //Top-Left
      && maze[coord.y][coord.x + 1] == 9                                       //Top-Right
      && (maze[coord.y + 1][coord.x] == 2 || maze[coord.y + 1][coord.x] == 4)  //Bottom-Left
      && maze[coord.y + 1][coord.x + 1] == 10)                                 //Bottom-Right
  {
    setGoal({ coord.x, coord.y + 1 });
    return true;
  }
  /*
     +---+---+            +---+---+             TOP-LEFT:       5
     |       |            |       |             TOP-RIGHT:      9
     +   +   +     OR     +   +   +             BOTTOM-LEFT:    6
     |                    |       |             BOTTOM-RIGHT:   2 || 8
     +---+---+            +---+   +
    */
  if (maze[coord.y][coord.x] == 5                                                       //Top-Left
      && maze[coord.y + 1][coord.x] == 6                                                //Bottom-Left
      && maze[coord.y][coord.x + 1] == 9                                                //Top-Right
      && (maze[coord.y + 1][coord.x + 1] == 2 || maze[coord.y + 1][coord.x + 1] == 8))  //Bottom-Right
  {
    setGoal({ coord.x + 1, coord.y + 1 });
    return true;
  }
  return false;
}

void setGoal(Coordinate coord) {
  goalX = coord.x;
  goalY = coord.y;
}

void findGoal() {
  for (int y = 0; y < MAZE_MAX_HEIGHT - 1; y++)
    for (int x = 0; x < MAZE_MAX_WIDTH - 1; x++)
      if (isGoal({ x, y }))
        return;
}

void initMazeWeight() {
  // Khởi tạo tất cả các phần tử bằng -1
  for (int i = 0; i < MAZE_MAX_HEIGHT; i++) {
    for (int j = 0; j < MAZE_MAX_WIDTH; j++) {
      mazeWeight[i][j] = -1;
    }
  }
}

void floodFill() {
  Queue queueNext;
  queueNext.push({ goalX, goalY });
  while (!queueNext.isEmpty()) {
    Coordinate current = queueNext.top();
    if (!hasWallTopByMaze(current) && getTopWeight(current) == -1)
      queueNext.push({ current.x, current.y - 1 });
    if (!hasWallBottomByMaze(current) && getBottomWeight(current) == -1)
      queueNext.push({ current.x, current.y + 1 });
    if (!hasWallLeftByMaze(current) && getLeftWeight(current) == -1)
      queueNext.push({ current.x - 1, current.y });
    if (!hasWallRightByMaze(current) && getRightWeight(current) == -1)
      queueNext.push({ current.x + 1, current.y });
    mazeWeight[current.y][current.x] = getSmallestWeightAround(current) + 1;
    queueNext.pop();
  }
}

bool hasWallTopByMaze(Coordinate coord) {
  return maze[coord.y][coord.x] & 1;
}

bool hasWallBottomByMaze(Coordinate coord) {
  return maze[coord.y][coord.x] & 2;
}

bool hasWallLeftByMaze(Coordinate coord) {
  return maze[coord.y][coord.x] & 4;
}

bool hasWallRightByMaze(Coordinate coord) {
  return maze[coord.y][coord.x] & 8;
}

int getCurrentWeight(Coordinate coord) {
  return mazeWeight[coord.y][coord.x];
}

int getTopWeight(Coordinate coord) {
  if (coord.y == 0)
    return -1;
  return mazeWeight[coord.y - 1][coord.x];
}

int getBottomWeight(Coordinate coord) {
  if (coord.y == MAZE_MAX_HEIGHT - 1)
    return -1;
  return mazeWeight[coord.y + 1][coord.x];
}

int getLeftWeight(Coordinate coord) {
  if (coord.x == 0)
    return -1;
  return mazeWeight[coord.y][coord.x - 1];
}

int getRightWeight(Coordinate coord) {
  if (coord.x == MAZE_MAX_WIDTH - 1)
    return -1;
  return mazeWeight[coord.y][coord.x + 1];
}

Coordinate getTopCoordByCoordinate(Coordinate coord) {
  return { coord.x, coord.y - 1 };
}

Coordinate getBottomCoordByCoordinate(Coordinate coord) {
  return { coord.x, coord.y + 1 };
}

Coordinate getLeftCoordByCoordinate(Coordinate coord) {
  return { coord.x - 1, coord.y };
}

Coordinate getRightCoordByCoordinate(Coordinate coord) {
  return { coord.x + 1, coord.y };
}

int getSmallestWeightAround(Coordinate coord) {
  int smallest = MAZE_MAX_WIDTH * MAZE_MAX_HEIGHT;
  int top = getTopWeight(coord);
  int bottom = getBottomWeight(coord);
  int left = getLeftWeight(coord);
  int right = getRightWeight(coord);
  if (!hasWallTopByMaze(coord) && top < smallest && top != -1)
    smallest = top;
  if (!hasWallBottomByMaze(coord) && bottom < smallest && bottom != -1)
    smallest = bottom;
  if (!hasWallLeftByMaze(coord) && left < smallest && left != -1)
    smallest = left;
  if (!hasWallRightByMaze(coord) && right < smallest && right != -1)
    smallest = right;
  if (smallest >= MAZE_MAX_WIDTH * MAZE_MAX_HEIGHT)
    smallest = -1;
  return smallest;
}

Stack findShortestPath(Coordinate start, Coordinate goal, int mode) {
  Stack path;
  path.push(start);
  if (getCurrentWeight(start) > getCurrentWeight(goal)) {
    mode = DECREASE;
  } else {
    mode = INCREASE;
  }
  while (!compareCoordinate(path.top(), goal)) {
    Coordinate current = path.top();
    if (!hasWallTopByMaze(current) && getTopWeight(current) == getCurrentWeight(current) + mode) {
      path.push(getTopCoordByCoordinate(current));
      continue;
    }
    if (!hasWallBottomByMaze(current) && getBottomWeight(current) == getCurrentWeight(current) + mode) {
      path.push(getBottomCoordByCoordinate(current));
      continue;
    }
    if (!hasWallLeftByMaze(current) && getLeftWeight(current) == getCurrentWeight(current) + mode) {
      path.push(getLeftCoordByCoordinate(current));
      continue;
    }
    if (!hasWallRightByMaze(current) && getRightWeight(current) == getCurrentWeight(current) + mode) {
      path.push(getRightCoordByCoordinate(current));
    }
  }
  path.push(goal);
  if (path.isEmpty()) {
    digitalWrite(24, HIGH);
    delay(1000);
    digitalWrite(24, LOW);
    delay(2000);
  }
  return path;  // If no path is found, return the empty stack
}

void followPath(Stack path) {
  while (!path.isEmpty()) {
    Coordinate next = path.top();
    if (compareCoordinate(next, { mouseX, mouseY })) {
      path.pop();
      continue;
    }
    if (compareCoordinate(next, getFrontCord())) {
      goStraight();
      path.pop();
      continue;
    }
    if (compareCoordinate(next, getLeftCord())) {
      turnLeft90();
      goStraight();
      path.pop();
      continue;
    }
    if (compareCoordinate(next, getRightCord())) {
      turnRight90();
      goStraight();
      path.pop();
      continue;
    }
    if (compareCoordinate(next, getBackCord())) {
      turnRight180();
      goStraight();
      path.pop();
      continue;
    }
  }
}
