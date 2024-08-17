#include <iomanip>
#include <iostream>
#include <windows.h>
#include <string>
#include <queue>
#include <stack>

#define MAZE_MAX_HEIGHT 16
#define MAZE_MAX_WIDTH 16
#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3

/*
Direction   Bit     Value
Top         0       2^0=1
Bottom      1       2^1=2
Left        2       2^2=4
Right       3       2^3=8

 --- Map of Mousse Direction ---
            0
            ^
        2 < + > 3
            v
            1
*/
/*
 --- Table of Wall Code ---
     xx
    x  x     0  (None)
     xx

     __
    x  x     1  (Top)
     xx

     xx
    x  x     2  (Bottom)
     __

     __
    x  x     3  (Top-Bottom)
     --

     xx
    |  x     4  (Left)
     xx

     __
    |  x     5  (Top-Left)
     xx

     xx
    |  x     6  (Bottom-Left)
     --

     __
    x  |     7  (Top-Bottom-Left)
     --

     xx
    x  |     8  (Right)
     xx

     __
    x  |     9  (Top-Right)
     xx

     xx
    x  |     10 (Bottom-Right)
     --

     __
    x  |     11 (Top-Bottom-Right)
     --

     xx
    |  |     12 (Left-Right)
     xx

     __
    |  |     13 (Top-Left-Right)
     xx

     xx
    |  |     14 (Bottom-Left-Right)
     --

     __
    |  |     15 (Top-Bottom-Left-Right)
     --
*/
/*
 --- Table of Color ---
 1.  Black
 2.  Blue
 3.  Green
 4.  Cyan
 5.  Red
 6.  Magenta
 7.  Brown
 8.  Light Gray
 9.  Dark Gray
 10. Light Green
 11. Light Cyan
 12. Light Red
 13. Light Magenta
 14. Yellow
 15. White
*/
struct cell {
    bool hasLeft = false;
    bool hasRight = false;
    bool hasTop = false;
    bool hasBottom = false;
    int value;
};

struct Coordinate {
    int x;
    int y;
} coord;

std::stack<Coordinate> visitedCoord;

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

int maze[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] =
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

bool mazeVisited[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] = {false};
int mouseDirection = 1;
int mouseX = 0, mouseY = 0;
int startX = 0, startY = 0;
int destX = 0, destY = 0;
int pauseTime = 100;
bool isCheckpoint = false;

/*
     Setup:
     1/ void startMazing() (startFloodFill hiện tại): hàm này trả về trọng số của tường cho biến maze[][]
     2/ void startFloodFill(): hàm này sẽ tính trọng số flood fill cho biến mazeWeight
     3/ std::stack<Coordinate> findShortestPath(): trả về 1 stack đường đi ngắn nhất từ start đến dest
     4/ std::stack<Coordinate> reverseStack(std::stack<Coordinate> pathList): đảo ngược stack

     Loop:
     1/ void goTo(std::stack<Coordinate> pathList): cho chuột chạy theo path (tham số pathList)
*/
//-----------------------------------------
void setColor(int color);

void clear_screen();

void resetColor();

void checkpoint();

std::string getCoordString(Coordinate coord);

void printMaze();

void printMazeWithWall();

void printVisitedMaze();

void printStack(std::stack<Coordinate> stack);

void updatePosition(int direction, int x, int y);

void updatePosAndPrintMaze(int direction, int x, int y);

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

Coordinate getLeftCord();

Coordinate getRightCord();

bool isVisited();

bool isCoordVisited(Coordinate coord);

bool onFront(Coordinate coord);

bool onTheLeft(Coordinate coord);

bool onTheRight(Coordinate coord);

bool onBehind(Coordinate coord);

//-----------------------------------------
int main() {
    bool flag = false;
    // while (true) {
    //     // printMaze();
    //     printMazeWithWall();
    //     if (!flag) {
    //         flag = !flag;
    //         turnLeft90();
    //     }
    //     // turnRight90();
    //     goStraight();
    //     Sleep(1000);
    //     clear_screen();
    // }
    printMazeWithWall();
    std::cout << "FRONT: " << hasWallFront() << std::endl;
    std::cout << "LEFT: " << hasWallLeft() << std::endl;
    std::cout << "RIGHT: " << hasWallRight() << std::endl;
    std::cout << "Coordinate FRONT: " << getCoordString(getFrontCord()) << std::endl;
    std::cout << "Coordinate LEFT: " << getCoordString(getLeftCord()) << std::endl;
    std::cout << "Coordinate RIGHT: " << getCoordString(getRightCord()) << std::endl;
    std::cout << "On Front (2,1):" << onFront({2, 1}) << std::endl;
    std::cout << "On The Left (2,1):" << onTheLeft({2, 1}) << std::endl;
    std::cout << "On The Right (2,1):" << onTheRight({2, 1}) << std::endl;
    checkpoint();
    clear_screen();
    startFloodFill();
}

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void clear_screen() {
#if defined(_WIN32)
    system("cls");
#elif defined(__linux__) || defined(__APPLE__)
    system("clear");
#else
    std::cerr << "Unsupported OS" << std::endl;
#endif
}

void resetColor() {
    setColor(7);
}

void checkpoint() {
    if (isCheckpoint) {
        getchar();
    }
}

std::string getCoordString(Coordinate coord) {
    return "(" + std::to_string(coord.x) + ", " + std::to_string(coord.y) + ")";
}

void printMaze() {
    std::cout << std::endl << "---MAZE---------------------------------------------" << std::endl;
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            if (x == mouseX && y == mouseY) {
                setColor(12);
                switch (mouseDirection) {
                    case 0:
                        std::cout << "^  ";
                        break;
                    case 1:
                        std::cout << "<  ";
                        break;
                    case 2:
                        std::cout << ">  ";
                        break;
                    case 3:
                        std::cout << "v  ";
                        break;
                    default:
                        std::cout << std::left << std::setw(3) << mazeWeight[y][x];
                        break;
                }
                resetColor();
            } else {
                std::cout << std::left << std::setw(3) << mazeWeight[y][x];
            }
        }
        std::cout << std::endl;
    }
    std::cout << "----------------------------------------------------" << std::endl;
}

void printMazeWithWall() {
    std::cout << std::endl << "---MAZE---------------------------------------------" << std::endl;
    for (int y = 0; y < 16 * 2 + 1; y++) {
        for (int x = 0; x < 16 * 2 + 1; x++) {
            if (x == 0 || x == 16 * 2 || y == 0 || y == 16 * 2) {
                if (y % 2 == 1) {
                    if (x == 0) {
                        switch (maze[(y - 1) / 2][x]) {
                            case 4:
                            case 5:
                            case 6:
                            case 7:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                                setColor(14);
                                std::cout << "|";
                                resetColor();
                                continue;
                            default:
                                std::cout << " ";
                                continue;
                        }
                    }
                    if (x == 16 * 2) {
                        switch (maze[(y - 1) / 2][x / 2 - 1]) {
                            case 8:
                            case 9:
                            case 10:
                            case 11:
                            case 12:
                            case 13:
                            case 14:
                            case 15:
                                setColor(14);
                                std::cout << "|";
                                resetColor();
                                continue;
                            default:
                                std::cout << " ";
                                continue;
                        }
                    }
                }

                if (x % 2 == 1) {
                    if (y == 0) {
                        switch (maze[y][(x - 1) / 2]) {
                            case 1:
                            case 3:
                            case 5:
                            case 7:
                            case 9:
                            case 11:
                            case 13:
                            case 15:
                                setColor(14);
                                std::cout << "---";
                                resetColor();
                                continue;
                            default:
                                std::cout << "   ";
                                continue;
                        }
                    }
                    if (y == 16 * 2) {
                        switch (maze[y / 2 - 1][(x - 1) / 2]) {
                            case 2:
                            case 3:
                            case 6:
                            case 7:
                            case 10:
                            case 11:
                            case 14:
                            case 15:
                                setColor(14);
                                std::cout << "---";
                                resetColor();
                                continue;
                            default:
                                std::cout << "   ";
                                continue;
                        }
                    }
                    setColor(14);
                    std::cout << "---";
                    resetColor();
                    continue;
                }
                setColor(14);
                std::cout << "+";
                resetColor();
                continue;
            }
            if (x % 2 == 1 && y % 2 == 1) {
                if (mouseX == (x - 1) / 2 && mouseY == (y - 1) / 2) {
                    setColor(10);
                    switch (mouseDirection) {
                        case 0:
                            std::cout << " ^ ";
                            break;
                        case 1:
                            std::cout << " v ";
                            break;
                        case 2:
                            std::cout << " < ";
                            break;
                        case 3:
                            std::cout << " > ";
                            break;
                        default:
                            std::cout << mazeWeight[y][x] << "  ";
                            break;
                    }
                    resetColor();
                } else {
                    if (mazeVisited[(y - 1) / 2][(x - 1) / 2]) {
                        setColor(3);
                    } else {
                        setColor(12);
                    }
                    if (mazeWeight[(y - 1) / 2][(x - 1) / 2] >= 10) {
                        std::cout << std::left << std::setw(3) << mazeWeight[(y - 1) / 2][(x - 1) / 2];
                    } else {
                        std::cout << " " << std::left << std::setw(2) << mazeWeight[(y - 1) / 2][(x - 1) / 2];
                    }
                    resetColor();
                }
            } else {
                if (x % 2 == 0 && y % 2 == 0) {
                    setColor(14);
                    std::cout << "+";
                    resetColor();
                    continue;
                }
                if (x % 2 == 0 && y % 2 == 1) {
                    switch (maze[(y - 1) / 2][(x - 1) / 2]) {
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                        case 15:
                            switch (maze[(y - 1) / 2][(x + 1) / 2]) {
                                case 4:
                                case 5:
                                case 6:
                                case 7:
                                case 12:
                                case 13:
                                case 14:
                                case 15:
                                    setColor(14);
                                    break;
                                default:
                                    setColor(4);
                                    break;
                            }
                            std::cout << "|";
                            resetColor();
                            break;
                        default:
                            std::cout << " ";
                            break;
                    }
                    continue;
                }
                if (x % 2 == 1 && y % 2 == 0) {
                    switch (maze[(y - 2) / 2][(x - 1) / 2]) {
                        case 2:
                        case 3:
                        case 6:
                        case 7:
                        case 10:
                        case 11:
                        case 14:
                        case 15:
                            switch (maze[y / 2][(x - 1) / 2]) {
                                case 1:
                                case 3:
                                case 5:
                                case 7:
                                case 9:
                                case 11:
                                case 13:
                                case 15:
                                    setColor(14);
                                    break;
                                default:
                                    setColor(4);
                                    break;
                            }

                            std::cout << "---";
                            resetColor();
                            break;
                        default:
                            std::cout << "   ";
                            break;
                    }
                }
            }
        }
        std::cout << std::endl;
    }
}

void printVisitedMaze() {
    for (int y = 0; y < MAZE_MAX_HEIGHT; y++) {
        for (int x = 0; x < MAZE_MAX_WIDTH; x++) {
            if (mazeWeight[y][x]) {
                setColor(3);
                std::cout << mazeWeight[y][x] << "  ";
                resetColor();
            } else {
                setColor(5);
                std::cout << mazeWeight[y][x] << "  ";
                resetColor();
            }
        }
    }
}

void printStack(std::stack<Coordinate> stack) {
    std::cout << "Stack: ";
    while (!stack.empty()) {
        std::cout << getCoordString(stack.top()) << " ";
        stack.pop();
    }
    std::cout << std::endl;
}

void updatePosition(int direction, int x, int y) {
    mouseDirection = direction;
    mouseX = x;
    mouseY = y;
    mazeVisited[mouseY][mouseX] = true;
}

void updatePosAndPrintMaze(int direction, int x, int y) {
    updatePosition(direction, x, y);
    printMazeWithWall();
}

//hàm đi thẳng 1 block
void goStraight() {
    switch (mouseDirection) {
        case TOP:
            switch (maze[mouseY][mouseX]) {
                case 1:
                case 3:
                case 5:
                case 7:
                case 9:
                case 11:
                case 13:
                case 15:
                    std::cout << "Error: Wall before !" << std::endl;
                    return;
                default:
                    if (mouseY > 0) {
                        updatePosition(mouseDirection, mouseX, mouseY - 1);
                    } else {
                        std::cout << "Error: out of maze (Y=" << mouseY << ") !" << std::endl;
                    }
                    break;
            }
            break;

        case BOTTOM:
            switch (maze[mouseY][mouseX]) {
                case 2:
                case 3:
                case 6:
                case 7:
                case 10:
                case 11:
                case 14:
                case 15:
                    std::cout << "Error: Wall before !" << std::endl;
                    return;
                default:
                    if (mouseY < MAZE_MAX_HEIGHT - 1) {
                        updatePosition(mouseDirection, mouseX, mouseY + 1);
                    } else {
                        std::cout << "Error: out of maze (Y=" << mouseY << ") !" << std::endl;
                    }
                    break;
            }
            break;

        case LEFT:
            switch (maze[mouseY][mouseX]) {
                case 4:
                case 5:
                case 6:
                case 7:
                case 12:
                case 13:
                case 14:
                case 15:
                    std::cout << "Error: Wall before !" << std::endl;
                    return;
                default:
                    if (mouseX > 0) {
                        updatePosition(mouseDirection, mouseX - 1, mouseY);
                    } else {
                        std::cout << "Error: out of maze (X=" << mouseX << ") !" << std::endl;
                    }
                    break;
            }
            break;
        case RIGHT:
            switch (maze[mouseY][mouseX]) {
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                    std::cout << "Error: Wall before !" << std::endl;
                    return;
                default:
                    if (mouseX < MAZE_MAX_WIDTH - 1) {
                        updatePosition(mouseDirection, mouseX + 1, mouseY);
                    } else {
                        std::cout << "Error: out of maze (X=" << mouseX << ") !" << std::endl;
                    }
                    break;
            }
            break;
        default:
            std::cout << "Error: Wrong Mouse Direction (Direction=" << mouseDirection << ") !" << std::endl;
            break;
    }
    visitedCoord.push({mouseX, mouseY});
    std::cout << "Mouse: Go Straight" << std::endl;
    // Insert Code IoT Here
}

//hàm check sensor đằng trước
bool hasWallFront() {
    switch (mouseDirection) {
        case 0:
            switch (maze[mouseY][mouseX]) {
                case 1:
                case 3:
                case 5:
                case 7:
                case 9:
                case 11:
                case 13:
                case 15:
                    return true;
                default:
                    return false;
            }

        case 1:
            switch (maze[mouseY][mouseX]) {
                case 2:
                case 3:
                case 6:
                case 7:
                case 10:
                case 11:
                case 14:
                case 15:
                    return true;
                default:
                    return false;
            }

        case 2:
            switch (maze[mouseY][mouseX]) {
                case 4:
                case 5:
                case 6:
                case 7:
                case 12:
                case 13:
                case 14:
                case 15:
                    return true;
                default:
                    return false;
            }
        case 3:
            switch (maze[mouseY][mouseX]) {
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
    // Insert Code IoT Here
}

// hàm check sensor bên trái
bool hasWallLeft() {
    switch (mouseDirection) {
        case 0: // facing up
            return maze[mouseY][mouseX] & 4; // check left wall
        case 3: // facing right
            return maze[mouseY][mouseX] & 1; // check top wall
        case 1: // facing down
            return maze[mouseY][mouseX] & 8; // check right wall
        case 2: // facing left
            return maze[mouseY][mouseX] & 2; // check bottom wall
        default:
            return false;
    }
}

// hàm check sensor bên phải
bool hasWallRight() {
    switch (mouseDirection) {
        case 0: // facing up
            return maze[mouseY][mouseX] & 8; // check right wall
        case 3: // facing right
            return maze[mouseY][mouseX] & 2; // check bottom wall
        case 1: // facing down
            return maze[mouseY][mouseX] & 4; // check left wall
        case 2: // facing left
            return maze[mouseY][mouseX] & 1; // check top wall
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
            std::cout << "Error: Wrong Mouse Direction (Direction=" << mouseDirection << ") !" << std::endl;
            break;
    }
    std::cout << "Mouse: Turn Left 90*" << std::endl;
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
            std::cout << "Error: Wrong Mouse Direction (Direction=" << mouseDirection << ") !" << std::endl;
            break;
    }
    std::cout << "Mouse: Turn Right 90*" << std::endl;
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
            std::cout << "Error: Wrong Mouse Direction (Direction=" << mouseDirection << ") !" << std::endl;
            break;
    }
    std::cout << "Mouse: Turn Left 180*" << std::endl;
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
    std::cout << "Mouse: Turn Right 190*" << std::endl;
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
    std::cout << "Mouse: Turn Left 45*" << std::endl;
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
    std::cout << "Mouse: Turn Right 45*" << std::endl;
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
    std::cout << "Mouse: Turn Left 135*" << std::endl;
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
    std::cout << "Mouse: Turn Right 135*" << std::endl;
}

void startFloodFill2() {
    // Khởi tạo vị trí ban đầu của chuột
    updatePosition(0, 0, 0);

    // Queue để xử lý các ô cần duyệt
    std::queue<Coordinate> queue;
    queue.push({0, 0});

    while (!queue.empty()) {
        Coordinate current = queue.front();
        queue.pop();

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
                queue.push(next);
            }
        }
        if (!hasWallRight()) {
            Coordinate next = {current.x + 1, current.y}; // Đi phải
            if (!isCoordVisited(next)) {
                queue.push(next);
            }
        }
        if (!hasWallLeft()) {
            Coordinate next = {current.x - 1, current.y}; // Đi trái
            if (!isCoordVisited(next)) {
                queue.push(next);
            }
        }

        printMazeWithWall(); // In mê cung sau mỗi bước
        Sleep(pauseTime); // Dừng lại để người dùng quan sát
        clear_screen();
    }
}

void goToCoord(int x, int y) {
}

void startFloodFill() {
    std::stack<Coordinate> stackStep;
    stackStep.push(getFrontCord()); // Set initial point

    // Mark the starting point
    mazeWeight[mouseY][mouseX] = 0;
    mazeVisited[mouseY][mouseX] = true;

    while (!stackStep.empty()) {
        std::cout << "Notification: Loading Maze" << std::endl;

        Coordinate next = stackStep.top(); // Peek the top coordinate
        std::cout << "Next Coord: " << getCoordString(next) << std::endl;
        std::cout << "Mouse Coord: " << getCoordString({mouseX, mouseY}) << std::endl;

        // Adjust direction to face the current target
        if (onFront(next)) {
            goStraight();

            // Mark the position as visited after moving
            mazeVisited[mouseY][mouseX] = true;
            stackStep.pop();
        } else if (onTheLeft(next)) {
            turnLeft90();
            continue;
        } else if (onTheRight(next)) {
            turnRight90();
            continue;
        } else if (onBehind(next)) {
            turnRight180();
        } else {
            turnRight180();
            visitedCoord.pop();
            // bool isReturned = false;
            // while (!onTheLeft(next) && !onTheRight(next) && !onFront(next)) {
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
                        clear_screen();
                        printMazeWithWall();
                        printStack(visitedCoord);
                        printStack(stackStep);
                        Sleep(pauseTime);
                        checkpoint();
                    }
                }
                if (onTheLeft(next) && !hasWallLeft()) {
                    turnLeft90();
                    std::cout << "On Left" << std::endl;
                    break;
                }
                if (onTheRight(next) && !hasWallRight()) {
                    turnRight90();
                    std::cout << "On Right" << std::endl;
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

        // Visualization
        printMazeWithWall();
        printStack(stackStep);

        // Simulate a pause
        checkpoint(); // Pause for input
        Sleep(500);
        clear_screen();
    }

    std::cout << "Finished Load Maze" << std::endl;
}

Coordinate getFrontCord() {
    switch (mouseDirection) {
        case 0:
            if (mouseY > 0) {
                return {mouseX, mouseY - 1};
            }
            return {mouseX, -1};

        case 1:
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1};
            }
            return {mouseX, -1};

        case 2:
            if (mouseX > 0) {
                return {mouseX - 1, mouseY};
            }
            return {-1, mouseY};

        case 3:
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY};
            }
            return {-1, mouseY};

        default:
            return {-1, -1};
    }
}

Coordinate getLeftCord() {
    switch (mouseDirection) {
        case 0:
            if (mouseX > 0) {
                return {mouseX - 1, mouseY};
            }
            return {-1, mouseY};

        case 1:
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY};
            }
            return {-1, mouseY};

        case 2:
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1};
            }
            return {mouseX, -1};

        case 3:
            if (mouseY > 0) {
                return {mouseX, mouseY - 1};
            }
            return {mouseX, -1};

        default:
            return {-1, -1};
    }
}

Coordinate getRightCord() {
    switch (mouseDirection) {
        case 0:
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY};
            }
            return {-1, mouseY};

        case 1:
            if (mouseX > 0) {
                return {mouseX - 1, mouseY};
            }
            return {-1, mouseY};

        case 2:
            if (mouseY > 0) {
                return {mouseX, mouseY - 1};
            }
            return {mouseX, -1};

        case 3:
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1};
            }
            return {mouseX, -1};

        default:
            return {-1, -1};
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
