#include <Arduino.h>

#define MAX_SIZE 128  // Kích thước tối đa của queue

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
int mouseDirection = 3;
int mouseX = 15, mouseY = 15;
int startX = mouseX, startY = mouseY;
int goalX = 0, goalY = 0;
Stack startToGoal;
Stack goalToStart;

bool isInStack(Stack stack, Coordinate coord);

Stack reverseStack(Stack stack);

bool compareCoordinate(Coordinate coord1, Coordinate coord2);

void updatePosition(int direction, int x, int y);

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

void setup() {
  Serial.begin(115200);
  startFloodFill();
  findGoal();
  initMazeWeight();
  floodFill();
  goalToStart = findShortestPath({ startX, startY }, { goalX, goalY }, DECREASE);
  startToGoal = reverseStack(goalToStart);
  followPath(reverseStack(findShortestPath({ mouseX, mouseY }, { goalX, goalY }, DECREASE)));
}

void loop() {
  followPath(goalToStart);
  followPath(startToGoal);
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

//hàm đi thẳng 1 block
void goStraight() {
  switch (mouseDirection) {
    case TOP:
      if (!maze[mouseY][mouseX] & 1 && mouseY > 0) {
        updatePosition(mouseDirection, mouseX, mouseY - 1);
      }
      break;

    case BOTTOM:
      if (!maze[mouseY][mouseX] & 2 && mouseY < MAZE_MAX_HEIGHT - 1) {
        updatePosition(mouseDirection, mouseX, mouseY + 1);
      }
      break;

    case LEFT:
      if (!maze[mouseY][mouseX] & 4 && mouseX > 0) {
        updatePosition(mouseDirection, mouseX - 1, mouseY);
      }
      break;

    case RIGHT:
      if (!maze[mouseY][mouseX] & 8 && mouseX < MAZE_MAX_WIDTH - 1) {
        updatePosition(mouseDirection, mouseX + 1, mouseY);
      }
      break;

    default:
      break;
  }
  visitedCoord.push({ mouseX, mouseY });
  // Insert Code IoT Here
  
}

//hàm check sensor đằng trước
bool hasWallFront() {
  switch (mouseDirection) {
    case TOP:
      return maze[mouseY][mouseX] & 1;
    case BOTTOM:
      return maze[mouseY][mouseX] & 2;
    case LEFT:
      return maze[mouseY][mouseX] & 4;
    case RIGHT:
      return maze[mouseY][mouseX] & 8;
    default:
      return false;
  }
  // Insert Code IoT Here
}

// hàm check sensor bên trái
bool hasWallLeft() {
  if (sensor){
    return true;
  }
  return false;
}

// hàm check sensor bên phải
bool hasWallRight() {
  switch (mouseDirection) {
    case 0:                             // facing up
      return maze[mouseY][mouseX] & 8;  // check right wall
    case 3:                             // facing right
      return maze[mouseY][mouseX] & 2;  // check bottom wall
    case 1:                             // facing down
      return maze[mouseY][mouseX] & 4;  // check left wall
    case 2:                             // facing left
      return maze[mouseY][mouseX] & 1;  // check top wall
    default:
      return false;
  }
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
    Coordinate next = stackStep.top();  // Peek the top coordinate
    if (isCoordVisited(next)) {
      stackStep.pop();
      continue;
    }
    // Adjust direction to face the current target
    if (onFront(next) && !hasWallFront()) {
      goStraight();

      // Mark the position as visited after moving
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
