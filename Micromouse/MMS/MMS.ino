#include <Arduino.h>
#define MAZE_MAX_HEIGHT 16
#define MAZE_MAX_WIDTH 16
#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3

struct Cell {
    bool hasLeft = false;
    bool hasRight = false;
    bool hasTop = false;
    bool hasBottom = false;
    int value;
};

struct Coordinate {
    int x;
    int y;
};
int mazeWeight[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int mazeArr[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] =
{
    {13, 5, 3, 3, 3, 3, 3, 1, 3, 3, 9, 5, 1, 3, 1, 9},
    {12, 12, 5, 9, 5, 1, 9, 6, 1, 9, 12, 12, 6, 9, 12, 12},
    {12, 12, 12, 6, 10, 12, 4, 9, 12, 12, 12, 6, 9, 12, 12, 14},
    {12, 12, 4, 9, 5, 10, 12, 6, 10, 12, 4, 3, 2, 10, 6, 9},
    {4, 2, 10, 12, 6, 9, 6, 3, 9, 6, 2, 3, 3, 3, 3, 8},
    {6, 3, 9, 6, 3, 2, 1, 9, 6, 3, 3, 9, 7, 3, 9, 12},
    {5, 3, 10, 5, 3, 1, 10, 6, 3, 1, 9, 12, 5, 3, 10, 12},
    {12, 5, 9, 4, 9, 6, 9, 5, 1, 10, 12, 12, 4, 3, 3, 10},
    {12, 12, 6, 10, 12, 5, 10, 6, 10, 5, 10, 12, 6, 3, 1, 9},
    {12, 12, 5, 9, 12, 12, 5, 3, 9, 6, 9, 12, 5, 3, 10, 12},
    {12, 4, 10, 4, 10, 6, 10, 5, 2, 9, 4, 8, 4, 3, 3, 10},
    {4, 10, 5, 10, 5, 3, 3, 2, 9, 6, 10, 12, 6, 3, 1, 9},
    {4, 3, 10, 5, 10, 5, 3, 9, 6, 3, 9, 6, 3, 3, 10, 12},
    {12, 5, 9, 12, 5, 10, 13, 12, 5, 9, 12, 5, 3, 3, 9, 12},
    {6, 8, 6, 10, 12, 5, 2, 2, 10, 14, 12, 12, 5, 11, 12, 12},
    {7, 2, 3, 3, 2, 2, 3, 3, 3, 3, 2, 10, 6, 3, 2, 10}
};
struct Cell maze[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH];
Coordinate coord;

Coordinate visitedCoord[MAZE_MAX_HEIGHT * MAZE_MAX_WIDTH];
int stackTop = -1;

// Hàm để đẩy tọa độ vào ngăn xếp
void push(Coordinate coord) {
    if (stackTop < MAZE_MAX_HEIGHT * MAZE_MAX_WIDTH - 1) {
        visitedCoord[++stackTop] = coord;
    }
}

// Hàm để lấy tọa độ ra khỏi ngăn xếp
Coordinate pop() {
    if (stackTop >= 0) {
        return visitedCoord[stackTop--];
    } else {
        // Trả về giá trị mặc định nếu ngăn xếp rỗng
        return {-1, -1};
    }
}
bool mazeVisited[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] = {false};
int mouseDirection = 1;
int mouseX = 0, mouseY = 0;
int startX = 0, startY = 0;
int destX = 0, destY = 0;
int pauseTime = 100;
bool isCheckpoint = false;
//Function here
void updatePosition(int direction, int x, int y) {
    mouseDirection = direction;
    mouseX = x;
    mouseY = y;
    mazeVisited[mouseY][mouseX] = true;
}
void updatePosAndPrintMaze(int direction, int x, int y) {
    updatePosition(direction, x, y);
}
void goStraight() {
    switch (mouseDirection) {
        case TOP:
            switch (maze[mouseY][mouseX].value) { // Sử dụng giá trị của ô trong maze
                case 1:
                case 3:
                case 5:
                case 7:
                case 9:
                case 11:
                case 13:
                case 15:
                    Serial.println("Error: Wall before !");
                    return;
                default:
                    if (mouseY > 0) {
                        updatePosition(mouseDirection, mouseX, mouseY - 1);
                    } else {
                        Serial.print("Error: out of maze (Y=");
                        Serial.print(mouseY);
                        Serial.println(") !");
                    }
                    break;
            }
            break;

        case BOTTOM:
            switch (maze[mouseY][mouseX].value) {
                case 2:
                case 3:
                case 6:
                case 7:
                case 10:
                case 11:
                case 14:
                case 15:
                    Serial.println("Error: Wall before !");
                    return;
                default:
                    if (mouseY < MAZE_MAX_HEIGHT - 1) {
                        updatePosition(mouseDirection, mouseX, mouseY + 1);
                    } else {
                        Serial.print("Error: out of maze (Y=");
                        Serial.print(mouseY);
                        Serial.println(") !");
                    }
                    break;
            }
            break;

        case LEFT:
            switch (maze[mouseY][mouseX].value) {
                case 4:
                case 5:
                case 6:
                case 7:
                case 12:
                case 13:
                case 14:
                case 15:
                    Serial.println("Error: Wall before !");
                    return;
                default:
                    if (mouseX > 0) {
                        updatePosition(mouseDirection, mouseX - 1, mouseY);
                    } else {
                        Serial.print("Error: out of maze (X=");
                        Serial.print(mouseX);
                        Serial.println(") !");
                    }
                    break;
            }
            break;

        case RIGHT:
            switch (maze[mouseY][mouseX].value) {
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                    Serial.println("Error: Wall before !");
                    return;
                default:
                    if (mouseX < MAZE_MAX_WIDTH - 1) {
                        updatePosition(mouseDirection, mouseX + 1, mouseY);
                    } else {
                        Serial.print("Error: out of maze (X=");
                        Serial.print(mouseX);
                        Serial.println(") !");
                    }
                    break;
            }
            break;

        default:
            Serial.print("Error: Wrong Mouse Direction (Direction=");
            Serial.print(mouseDirection);
            Serial.println(") !");
            break;
    }

    // Đẩy tọa độ vào ngăn xếp
    push({mouseX, mouseY});

    Serial.println("Mouse: Go Straight");

    // Insert Code IoT Here
}
bool hasWallFront() {
    switch (mouseDirection) {
        case 0: // TOP (đối mặt phía trên)
            return maze[mouseY][mouseX].hasTop;
        
        case 1: // BOTTOM (đối mặt phía dưới)
            return maze[mouseY][mouseX].hasBottom;

        case 2: // LEFT (đối mặt bên trái)
            return maze[mouseY][mouseX].hasLeft;

        case 3: // RIGHT (đối mặt bên phải)
            return maze[mouseY][mouseX].hasRight;

        default:
            return false;
    }
}
bool hasWallLeft() {
    switch (mouseDirection) {
        case 0: // facing up (TOP)
            return maze[mouseY][mouseX].hasLeft; // check left wall
        case 3: // facing right (RIGHT)
            return maze[mouseY][mouseX].hasTop; // check top wall
        case 1: // facing down (BOTTOM)
            return maze[mouseY][mouseX].hasRight; // check right wall
        case 2: // facing left (LEFT)
            return maze[mouseY][mouseX].hasBottom; // check bottom wall
        default:
            return false;
    }
}
bool hasWallRight() {
    switch (mouseDirection) {
        case 0: // facing up (TOP)
            return maze[mouseY][mouseX].hasRight; // kiểm tra tường phải
        case 3: // facing right (RIGHT)
            return maze[mouseY][mouseX].hasBottom; // kiểm tra tường dưới
        case 1: // facing down (BOTTOM)
            return maze[mouseY][mouseX].hasLeft; // kiểm tra tường trái
        case 2: // facing left (LEFT)
            return maze[mouseY][mouseX].hasTop; // kiểm tra tường trên
        default:
            return false;
    }
}
void turnLeft90() {
    switch (mouseDirection) {
        case 0: // Nếu đang hướng lên (TOP)
            mouseDirection = 2; // Quay sang trái 90 độ sẽ hướng sang trái (LEFT)
            break;
        case 1: // Nếu đang hướng xuống (BOTTOM)
            mouseDirection = 3; // Quay sang trái 90 độ sẽ hướng sang phải (RIGHT)
            break;
        case 2: // Nếu đang hướng trái (LEFT)
            mouseDirection = 1; // Quay sang trái 90 độ sẽ hướng xuống (BOTTOM)
            break;
        case 3: // Nếu đang hướng phải (RIGHT)
            mouseDirection = 0; // Quay sang trái 90 độ sẽ hướng lên (TOP)
            break;
        default:
            Serial.print("Error: Wrong Mouse Direction (Direction=");
            Serial.print(mouseDirection);
            Serial.println(") !");
            return;
    }
    Serial.println("Mouse: Turn Left 90*");
    // Insert Code IoT Here
}
void turnRight90() {
    switch (mouseDirection) {
        case 0: // Nếu đang hướng lên (TOP)
            mouseDirection = 3; // Quay sang phải 90 độ sẽ hướng sang phải (RIGHT)
            break;
        case 1: // Nếu đang hướng xuống (BOTTOM)
            mouseDirection = 2; // Quay sang phải 90 độ sẽ hướng sang trái (LEFT)
            break;
        case 2: // Nếu đang hướng trái (LEFT)
            mouseDirection = 0; // Quay sang phải 90 độ sẽ hướng lên (TOP)
            break;
        case 3: // Nếu đang hướng phải (RIGHT)
            mouseDirection = 1; // Quay sang phải 90 độ sẽ hướng xuống (BOTTOM)
            break;
        default:
            Serial.print("Error: Wrong Mouse Direction (Direction=");
            Serial.print(mouseDirection);
            Serial.println(") !");
            return;
    }
    Serial.println("Mouse: Turn Right 90*");
    // Insert Code IoT Here
}
void turnLeft180() {
    switch (mouseDirection) {
        case 0: // Nếu đang hướng lên (TOP)
            mouseDirection = 1; // Quay 180 độ sẽ hướng xuống (BOTTOM)
            break;
        case 1: // Nếu đang hướng xuống (BOTTOM)
            mouseDirection = 0; // Quay 180 độ sẽ hướng lên (TOP)
            break;
        case 2: // Nếu đang hướng trái (LEFT)
            mouseDirection = 3; // Quay 180 độ sẽ hướng phải (RIGHT)
            break;
        case 3: // Nếu đang hướng phải (RIGHT)
            mouseDirection = 2; // Quay 180 độ sẽ hướng trái (LEFT)
            break;
        default:
            Serial.print("Error: Wrong Mouse Direction (Direction=");
            Serial.print(mouseDirection);
            Serial.println(") !");
            return;
    }
    Serial.println("Mouse: Turn Left 180*");
    // Insert Code IoT Here
}
void turnRight180() {
    switch (mouseDirection) {
        case 0: // Nếu đang hướng lên (TOP)
            mouseDirection = 1; // Quay 180 độ sẽ hướng xuống (BOTTOM)
            break;
        case 1: // Nếu đang hướng xuống (BOTTOM)
            mouseDirection = 0; // Quay 180 độ sẽ hướng lên (TOP)
            break;
        case 2: // Nếu đang hướng trái (LEFT)
            mouseDirection = 3; // Quay 180 độ sẽ hướng phải (RIGHT)
            break;
        case 3: // Nếu đang hướng phải (RIGHT)
            mouseDirection = 2; // Quay 180 độ sẽ hướng trái (LEFT)
            break;
        default:
            Serial.print("Error: Wrong Mouse Direction (Direction=");
            Serial.print(mouseDirection);
            Serial.println(") !");
            return;
    }
    Serial.println("Mouse: Turn Right 180*");
    // Insert Code IoT Here
}
bool isCoordVisited(Coordinate coord) {
    return mazeVisited[coord.y][coord.x];
}
Coordinate queue[MAZE_MAX_HEIGHT * MAZE_MAX_WIDTH];
int queueStart = 0;
int queueEnd = 0;

void enqueue(Coordinate coord) {
    queue[queueEnd++] = coord;
    if (queueEnd >= MAZE_MAX_HEIGHT * MAZE_MAX_WIDTH) {
        queueEnd = 0; // Vòng lại khi đến cuối
    }
}

Coordinate dequeue() {
    Coordinate coord = queue[queueStart++];
    if (queueStart >= MAZE_MAX_HEIGHT * MAZE_MAX_WIDTH) {
        queueStart = 0; // Vòng lại khi đến cuối
    }
    return coord;
}

bool isQueueEmpty() {
    return queueStart == queueEnd;
}

void startFloodFill2() {
    // Khởi tạo vị trí ban đầu của chuột
    updatePosition(0, 0, 0);

    // Khởi tạo hàng đợi
    enqueue({0, 0});

    while (!isQueueEmpty()) {
        Coordinate current = dequeue();

        // Kiểm tra nếu ô đã được thăm
        if (mazeVisited[current.y][current.x]) {
            continue;
        }

        // Đánh dấu ô đã được thăm
        mazeVisited[current.y][current.x] = true;
        mazeWeight[current.y][current.x] = 1; // Ví dụ đánh dấu 1 cho đã thăm

        // Kiểm tra các hướng xung quanh
        if (!hasWallFront()) {
            Coordinate next = {current.x, current.y - 1}; // Đi lên
            if (!isCoordVisited(next)) {
                enqueue(next);
            }
        }
        if (!hasWallRight()) {
            Coordinate next = {current.x + 1, current.y}; // Đi phải
            if (!isCoordVisited(next)) {
                enqueue(next);
            }
        }
        if (!hasWallLeft()) {
            Coordinate next = {current.x - 1, current.y}; // Đi trái
            if (!isCoordVisited(next)) {
                enqueue(next);
            }
        }

        delay(pauseTime); // Dừng lại để người dùng quan sát
    }
}
Coordinate getFrontCord() {
    switch (mouseDirection) {
        case 0: // Nếu đang hướng lên (TOP)
            if (mouseY > 0) {
                return {mouseX, mouseY - 1}; // Đi lên
            }
            return {mouseX, -1}; // Nếu đi ra ngoài biên trên của mê cung

        case 1: // Nếu đang hướng xuống (BOTTOM)
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1}; // Đi xuống
            }
            return {mouseX, -1}; // Nếu đi ra ngoài biên dưới của mê cung

        case 2: // Nếu đang hướng trái (LEFT)
            if (mouseX > 0) {
                return {mouseX - 1, mouseY}; // Đi trái
            }
            return {-1, mouseY}; // Nếu đi ra ngoài biên trái của mê cung

        case 3: // Nếu đang hướng phải (RIGHT)
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY}; // Đi phải
            }
            return {-1, mouseY}; // Nếu đi ra ngoài biên phải của mê cung

        default:
            return {-1, -1}; // Giá trị không hợp lệ
    }
}
Coordinate getLeftCord() {
    switch (mouseDirection) {
        case 0: // Nếu đang hướng lên (TOP)
            if (mouseX > 0) {
                return {mouseX - 1, mouseY}; // Đi trái
            }
            return {-1, mouseY}; // Nếu đi ra ngoài biên trái của mê cung

        case 1: // Nếu đang hướng xuống (BOTTOM)
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY}; // Đi phải
            }
            return {-1, mouseY}; // Nếu đi ra ngoài biên phải của mê cung

        case 2: // Nếu đang hướng trái (LEFT)
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1}; // Đi xuống
            }
            return {mouseX, -1}; // Nếu đi ra ngoài biên dưới của mê cung

        case 3: // Nếu đang hướng phải (RIGHT)
            if (mouseY > 0) {
                return {mouseX, mouseY - 1}; // Đi lên
            }
            return {mouseX, -1}; // Nếu đi ra ngoài biên trên của mê cung

        default:
            return {-1, -1}; // Giá trị không hợp lệ
    }
}
Coordinate getRightCord() {
    switch (mouseDirection) {
        case 0: // Nếu đang hướng lên (TOP)
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY}; // Đi phải
            }
            return {-1, mouseY}; // Nếu đi ra ngoài biên phải của mê cung

        case 1: // Nếu đang hướng xuống (BOTTOM)
            if (mouseX > 0) {
                return {mouseX - 1, mouseY}; // Đi trái
            }
            return {-1, mouseY}; // Nếu đi ra ngoài biên trái của mê cung

        case 2: // Nếu đang hướng trái (LEFT)
            if (mouseY > 0) {
                return {mouseX, mouseY - 1}; // Đi lên
            }
            return {mouseX, -1}; // Nếu đi ra ngoài biên trên của mê cung

        case 3: // Nếu đang hướng phải (RIGHT)
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1}; // Đi xuống
            }
            return {mouseX, -1}; // Nếu đi ra ngoài biên dưới của mê cung

        default:
            return {-1, -1}; // Giá trị không hợp lệ
    }
}
bool onFront(Coordinate coord) {
    switch (mouseDirection) {
        case TOP: // Nếu đối tượng đang hướng lên (TOP)
            if (coord.x == mouseX && coord.y == mouseY - 1) {
                return true; // Nếu tọa độ nằm ngay phía trước (phía trên)
            }
            break;

        case BOTTOM: // Nếu đối tượng đang hướng xuống (BOTTOM)
            if (coord.x == mouseX && coord.y == mouseY + 1) {
                return true; // Nếu tọa độ nằm ngay phía trước (phía dưới)
            }
            break;

        case LEFT: // Nếu đối tượng đang hướng trái (LEFT)
            if (coord.x == mouseX - 1 && coord.y == mouseY) {
                return true; // Nếu tọa độ nằm ngay phía trước (bên trái)
            }
            break;

        case RIGHT: // Nếu đối tượng đang hướng phải (RIGHT)
            if (coord.x == mouseX + 1 && coord.y == mouseY) {
                return true; // Nếu tọa độ nằm ngay phía trước (bên phải)
            }
            break;

        default:
            break;
    }
    return false; // Nếu tọa độ không nằm phía trước
}
bool onTheLeft(Coordinate coord) {
    switch (mouseDirection) {
        case TOP: // Nếu đối tượng đang hướng lên (TOP)
            if (coord.x == mouseX - 1 && coord.y == mouseY) {
                return true; // Tọa độ nằm bên trái (trái của hướng TOP)
            }
            break;

        case BOTTOM: // Nếu đối tượng đang hướng xuống (BOTTOM)
            if (coord.x == mouseX + 1 && coord.y == mouseY) {
                return true; // Tọa độ nằm bên trái (trái của hướng BOTTOM)
            }
            break;

        case LEFT: // Nếu đối tượng đang hướng trái (LEFT)
            if (coord.x == mouseX && coord.y == mouseY + 1) {
                return true; // Tọa độ nằm bên trái (trái của hướng LEFT)
            }
            break;

        case RIGHT: // Nếu đối tượng đang hướng phải (RIGHT)
            if (coord.x == mouseX && coord.y == mouseY - 1) {
                return true; // Tọa độ nằm bên trái (trái của hướng RIGHT)
            }
            break;

        default:
            break;
    }
    return false;
}
bool onTheRight(Coordinate coord) {
    switch (mouseDirection) {
        case TOP: // Nếu đối tượng đang hướng lên (TOP)
            if (coord.x == mouseX + 1 && coord.y == mouseY) {
                return true; // Tọa độ nằm bên phải (phải của hướng TOP)
            }
            break;

        case BOTTOM: // Nếu đối tượng đang hướng xuống (BOTTOM)
            if (coord.x == mouseX - 1 && coord.y == mouseY) {
                return true; // Tọa độ nằm bên phải (phải của hướng BOTTOM)
            }
            break;

        case LEFT: // Nếu đối tượng đang hướng trái (LEFT)
            if (coord.x == mouseX && coord.y == mouseY - 1) {
                return true; // Tọa độ nằm bên phải (phải của hướng LEFT)
            }
            break;

        case RIGHT: // Nếu đối tượng đang hướng phải (RIGHT)
            if (coord.x == mouseX && coord.y == mouseY + 1) {
                return true; // Tọa độ nằm bên phải (phải của hướng RIGHT)
            }
            break;

        default:
            break;
    }
    return false;
}
bool onBehind(Coordinate coord) {
    switch (mouseDirection) {
        case TOP: // Nếu đối tượng đang hướng lên (TOP)
            if (coord.x == mouseX && coord.y == mouseY + 1) {
                return true; // Tọa độ nằm phía sau (phía dưới của hướng TOP)
            }
            break;

        case BOTTOM: // Nếu đối tượng đang hướng xuống (BOTTOM)
            if (coord.x == mouseX && coord.y == mouseY - 1) {
                return true; // Tọa độ nằm phía sau (phía trên của hướng BOTTOM)
            }
            break;

        case LEFT: // Nếu đối tượng đang hướng trái (LEFT)
            if (coord.x == mouseX + 1 && coord.y == mouseY) {
                return true; // Tọa độ nằm phía sau (phải của hướng LEFT)
            }
            break;

        case RIGHT: // Nếu đối tượng đang hướng phải (RIGHT)
            if (coord.x == mouseX - 1 && coord.y == mouseY) {
                return true; // Tọa độ nằm phía sau (trái của hướng RIGHT)
            }
            break;

        default:
            break;
    }
    return false;
}

void setup() {
    Serial.begin(115200);
    // Cài đặt ban đầu
    maze[0][0].hasTop = true;    // Tạo tường trên tại (0,0)
    maze[0][0].hasRight = true;  // Tạo tường phải tại (0,0)
    maze[1][0].hasBottom = true; // Tạo tường dưới tại (1,0)
    maze[0][1].hasLeft = true;   // Tạo tường trái tại (0,1)
}

void loop() {
    // Code xử lý chính
    Serial.print("Direction: ");
    Serial.println(mouseDirection);
    if (hasWallFront()) {
        Serial.println("Wall in front!");
    } else {
        Serial.println("No wall in front.");
    }

    // Thay đổi hướng di chuyển để kiểm tra các trường hợp khác nhau
    delay(2000);
    mouseDirection = (mouseDirection + 1) % 4; // Thay đổi hướng (TOP -> RIGHT -> BOTTOM -> LEFT -> TOP)
}
