#include <iomanip>
#include <iostream>
#include <windows.h>
#include <string>
#include <queue>
#include <stack>
#include <conio.h>

#define MAZE_MAX_HEIGHT 16
#define MAZE_MAX_WIDTH 16
#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3
#define INCREASE 1
#define DECREASE (-1)
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

struct Coordinate {
    int x;
    int y;
} coord;

std::stack<Coordinate> visitedCoord;

int mazeWeight[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH];
int maze[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH];

int mazeTemp[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] =
        // {
        //     {13, 5, 3, 3, 3, 3, 3, 1, 3, 3, 9, 5, 1, 3, 1, 9},
        //     {12, 12, 5, 9, 5, 1, 9, 6, 1, 9, 12, 12, 6, 9, 12, 12},
        //     {12, 12, 12, 6, 10, 12, 4, 9, 12, 12, 12, 6, 9, 12, 12, 14},
        //     {12, 12, 4, 9, 5, 10, 12, 6, 10, 12, 4, 3, 2, 10, 6, 9},
        //     {4, 2, 10, 12, 6, 9, 6, 3, 9, 6, 2, 3, 3, 3, 3, 8},
        //     {6, 3, 9, 6, 3, 2, 1, 9, 6, 3, 3, 9, 7, 3, 9, 12},
        //     {5, 3, 10, 5, 3, 1, 10, 6, 3, 1, 9, 12, 5, 3, 10, 12},
        //     {12, 5, 9, 4, 9, 6, 9, 5, 1, 10, 12, 12, 4, 3, 3, 10},
        //     {12, 12, 6, 10, 12, 5, 10, 6, 10, 5, 10, 12, 6, 3, 1, 9},
        //     {12, 12, 5, 9, 12, 12, 5, 3, 9, 6, 9, 12, 5, 3, 10, 12},
        //     {12, 4, 10, 4, 10, 6, 10, 5, 2, 9, 4, 8, 4, 3, 3, 10},
        //     {4, 10, 5, 10, 5, 3, 3, 2, 9, 6, 10, 12, 6, 3, 1, 9},
        //     {4, 3, 10, 5, 10, 5, 3, 9, 6, 3, 9, 6, 3, 3, 10, 12},
        //     {12, 5, 9, 12, 5, 10, 13, 12, 5, 9, 12, 5, 3, 3, 9, 12},
        //     {6, 8, 6, 10, 12, 5, 2, 2, 10, 14, 12, 12, 5, 11, 12, 12},
        //     {7, 2, 3, 3, 2, 2, 3, 3, 3, 3, 2, 10, 6, 3, 2, 10}
        // };
        {
            {5, 3, 1, 11, 5, 3, 3, 9, 5, 1, 3, 3, 1, 3, 3, 9},
            {12, 7, 0, 3, 2, 3, 11, 14, 12, 4, 11, 7, 0, 11, 5, 10},
            {12, 7, 0, 11, 5, 9, 5, 9, 12, 14, 13, 7, 0, 11, 6, 9},
            {12, 13, 6, 3, 10, 6, 10, 12, 12, 5, 8, 7, 0, 11, 5, 8},
            {6, 2, 3, 3, 3, 9, 5, 10, 12, 12, 4, 3, 10, 5, 10, 12},
            {5, 9, 5, 1, 3, 10, 12, 5, 10, 12, 4, 3, 3, 10, 13, 12},
            {12, 6, 10, 14, 7, 3, 2, 10, 5, 10, 14, 13, 7, 9, 4, 8},
            {6, 3, 3, 1, 3, 3, 11, 5, 8, 5, 3, 2, 3, 2, 10, 14},
            {5, 1, 9, 4, 9, 5, 9, 6, 10, 6, 1, 9, 5, 9, 5, 9},
            {12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 14, 6, 10, 6, 10, 12},
            {12, 12, 12, 14, 6, 10, 6, 8, 12, 6, 3, 1, 3, 9, 7, 8},
            {12, 12, 12, 7, 1, 3, 9, 12, 12, 5, 9, 6, 9, 6, 9, 12},
            {12, 12, 12, 5, 10, 7, 2, 10, 12, 12, 6, 9, 6, 9, 6, 8},
            {12, 12, 4, 10, 5, 3, 9, 5, 0, 8, 13, 6, 9, 6, 9, 14},
            {12, 12, 14, 5, 10, 5, 10, 12, 12, 14, 6, 3, 8, 13, 6, 9},
            {14, 6, 3, 10, 7, 2, 3, 10, 6, 3, 3, 11, 6, 2, 3, 10}
        };

// Algorithm


bool mazeVisited[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] = {false};
int mouseDirection = 0;
int startX = 0, startY = 15;
int mouseX = startX, mouseY = startY;
int goalX = 0, goalY = 0;
int pauseTime = 500;
bool isCheckpoint = false;
std::stack<Coordinate> startToGoal;
std::stack<Coordinate> goalToStart;
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

void chooseMaze(int id);

void SetBackground(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

std::string getCoordString(Coordinate coord);

void printMaze();

void printMazeWithWall();

void printMazeCodeWithWall();

void printVisitedMaze();

void printShortestPathMaze();

void printStack(std::stack<Coordinate> stack, std::string name);

bool isInStack(std::stack<Coordinate> stack, Coordinate coord);

std::stack<Coordinate> reverseStack(std::stack<Coordinate> stack);

bool compareCoordinate(Coordinate coord1, Coordinate coord2);

void updatePosition(int direction, int x, int y);

void updatePosAndPrintMaze(int direction, int x, int y);

void goStraight();

bool hasWallFront();

bool hasWallLeft();

bool hasWallRight();

int getMazeCode();

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

std::stack<Coordinate> findShortestPath(Coordinate start, Coordinate goal, int mode);

void followPath(std::stack<Coordinate> path);

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
    getchar();
    clear_screen();
    startFloodFill();
    getchar();
    findGoal();
    initMazeWeight();
    floodFill();
    printMazeWithWall();
    std::cout << "GOAL: " << getCoordString({goalX, goalY}) << std::endl;
    getchar();
    goalToStart = findShortestPath({startX, startY}, {goalX, goalY}, DECREASE);
    startToGoal = reverseStack(goalToStart);
    printStack(goalToStart, "Shortest Path");
    pauseTime = 500;
    followPath(reverseStack(findShortestPath({mouseX, mouseY}, {goalX, goalY},DECREASE)));
    getchar();
    while (true) {
        followPath(goalToStart);
        followPath(startToGoal);
    }
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
                        if (maze[(y - 1) / 2][x] & 4) {
                            setColor(14);
                            std::cout << "|";
                            resetColor();
                            continue;
                        }
                        std::cout << " ";
                        continue;
                    }
                    if (x == 16 * 2) {
                        if (maze[(y - 1) / 2][x / 2 - 1] & 8) {
                            setColor(14);
                            std::cout << "|";
                            resetColor();
                            continue;
                        }
                        std::cout << " ";
                        continue;
                    }
                }

                if (x % 2 == 1) {
                    if (y == 0) {
                        if (maze[y][(x - 1) / 2] & 1) {
                            setColor(14);
                            std::cout << "---";
                            resetColor();
                            continue;
                        }
                        std::cout << "   ";
                        continue;
                    }
                    if (y == 16 * 2) {
                        if (maze[y / 2 - 1][(x - 1) / 2] & 2) {
                            setColor(14);
                            std::cout << "---";
                            resetColor();
                            continue;
                        }
                        std::cout << "   ";
                        continue;
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
                    // setColor(10);
                    SetBackground(10, 1);
                    switch (mouseDirection) {
                        case TOP:
                            std::cout << " ^ ";
                            break;
                        case BOTTOM:
                            std::cout << " v ";
                            break;
                        case LEFT:
                            std::cout << " < ";
                            break;
                        case RIGHT:
                            std::cout << " > ";
                            break;
                        default:
                            std::cout << mazeWeight[y][x] << "  ";
                            break;
                    }
                    // resetColor();
                    SetBackground(7, 0);
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
                    if (maze[(y - 1) / 2][(x - 1) / 2] & 8) {
                        if (maze[(y - 1) / 2][(x + 1) / 2] & 4) {
                            setColor(14);
                        } else {
                            setColor(4);
                        }
                        std::cout << "|";
                        resetColor();
                    } else {
                        std::cout << " ";
                    }
                    continue;
                }
                if (x % 2 == 1 && y % 2 == 0) {
                    if (maze[(y - 2) / 2][(x - 1) / 2] & 2) {
                        if (maze[y / 2][(x - 1) / 2] & 1) {
                            setColor(14);
                        } else {
                            setColor(4);
                        }
                        std::cout << "---";
                        resetColor();
                    } else {
                        std::cout << "   ";
                    }
                }
            }
        }
        std::cout << std::endl;
    }
}

void printMazeCodeWithWall() {
    std::cout << std::endl << "---MAZE---------------------------------------------" << std::endl;
    for (int y = 0; y < 16 * 2 + 1; y++) {
        for (int x = 0; x < 16 * 2 + 1; x++) {
            if (x == 0 || x == 16 * 2 || y == 0 || y == 16 * 2) {
                if (y % 2 == 1) {
                    if (x == 0) {
                        if (maze[(y - 1) / 2][x] & 4) {
                            setColor(14);
                            std::cout << "|";
                            resetColor();
                            continue;
                        }
                        std::cout << " ";
                        continue;
                    }
                    if (x == 16 * 2) {
                        if (maze[(y - 1) / 2][x / 2 - 1] & 8) {
                            setColor(14);
                            std::cout << "|";
                            resetColor();
                            continue;
                        }
                        std::cout << " ";
                        continue;
                    }
                }

                if (x % 2 == 1) {
                    if (y == 0) {
                        if (maze[y][(x - 1) / 2] & 1) {
                            setColor(14);
                            std::cout << "---";
                            resetColor();
                            continue;
                        }
                        std::cout << "   ";
                        continue;
                    }
                    if (y == 16 * 2) {
                        if (maze[y / 2 - 1][(x - 1) / 2] & 2) {
                            setColor(14);
                            std::cout << "---";
                            resetColor();
                            continue;
                        }
                        std::cout << "   ";
                        continue;
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
                    // setColor(10);
                    SetBackground(10, 1);
                    switch (mouseDirection) {
                        case TOP:
                            std::cout << " ^ ";
                            break;
                        case BOTTOM:
                            std::cout << " v ";
                            break;
                        case LEFT:
                            std::cout << " < ";
                            break;
                        case RIGHT:
                            std::cout << " > ";
                            break;
                        default:
                            std::cout << maze[y][x] << "  ";
                            break;
                    }
                    // resetColor();
                    SetBackground(7, 0);
                } else {
                    if (mazeVisited[(y - 1) / 2][(x - 1) / 2]) {
                        setColor(3);
                    } else {
                        setColor(12);
                    }
                    if (maze[(y - 1) / 2][(x - 1) / 2] >= 10) {
                        std::cout << std::left << std::setw(3) << maze[(y - 1) / 2][(x - 1) / 2];
                    } else {
                        std::cout << " " << std::left << std::setw(2) << maze[(y - 1) / 2][(x - 1) / 2];
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
                    if (maze[(y - 1) / 2][(x - 1) / 2] & 8) {
                        if (maze[(y - 1) / 2][(x + 1) / 2] & 4) {
                            setColor(14);
                        } else {
                            setColor(4);
                        }
                        std::cout << "|";
                        resetColor();
                    } else {
                        std::cout << " ";
                    }
                    continue;
                }
                if (x % 2 == 1 && y % 2 == 0) {
                    if (maze[(y - 2) / 2][(x - 1) / 2] & 2) {
                        if (maze[y / 2][(x - 1) / 2] & 1) {
                            setColor(14);
                        } else {
                            setColor(4);
                        }
                        std::cout << "---";
                        resetColor();
                    } else {
                        std::cout << "   ";
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

void printShortestPathMaze() {
    std::cout << std::endl << "---MAZE---------------------------------------------" << std::endl;
    for (int y = 0; y < 16 * 2 + 1; y++) {
        for (int x = 0; x < 16 * 2 + 1; x++) {
            if (x == 0 || x == 16 * 2 || y == 0 || y == 16 * 2) {
                if (y % 2 == 1) {
                    if (x == 0) {
                        if (maze[(y - 1) / 2][x] & 4) {
                            setColor(14);
                            std::cout << "|";
                            resetColor();
                            continue;
                        }
                        std::cout << " ";
                        continue;
                    }
                    if (x == 16 * 2) {
                        if (maze[(y - 1) / 2][x / 2 - 1] & 8) {
                            setColor(14);
                            std::cout << "|";
                            resetColor();
                            continue;
                        }
                        std::cout << " ";
                        continue;
                    }
                }

                if (x % 2 == 1) {
                    if (y == 0) {
                        if (maze[y][(x - 1) / 2] & 1) {
                            setColor(14);
                            std::cout << "---";
                            resetColor();
                            continue;
                        }
                        std::cout << "   ";
                        continue;
                    }
                    if (y == 16 * 2) {
                        if (maze[y / 2 - 1][(x - 1) / 2] & 2) {
                            setColor(14);
                            std::cout << "---";
                            resetColor();
                            continue;
                        }
                        std::cout << "   ";
                        continue;
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
                        case TOP:
                            std::cout << " ^ ";
                            break;
                        case BOTTOM:
                            std::cout << " v ";
                            break;
                        case LEFT:
                            std::cout << " < ";
                            break;
                        case RIGHT:
                            std::cout << " > ";
                            break;
                        default:
                            // std::cout << mazeWeight[y][x] << "  ";
                            break;
                    }
                    resetColor();
                } else {
                    if (isInStack(goalToStart, {(x - 1) / 2, (y - 1) / 2})) {
                        setColor(5);
                    } else {
                        setColor(3);
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
                    if (maze[(y - 1) / 2][(x - 1) / 2] & 8) {
                        setColor(4);
                        std::cout << "|";
                        resetColor();
                    } else {
                        std::cout << " ";
                    }
                    continue;
                }
                if (x % 2 == 1 && y % 2 == 0) {
                    if (maze[(y - 2) / 2][(x - 1) / 2] & 2) {
                        setColor(4);
                        std::cout << "---";
                        resetColor();
                    } else {
                        std::cout << "   ";
                    }
                }
            }
        }
        std::cout << std::endl;
    }
}

void printStack(std::stack<Coordinate> stack, std::string name) {
    std::cout << "Stack[" << name << "]: ";
    while (!stack.empty()) {
        std::cout << getCoordString(stack.top()) << " ";
        stack.pop();
    }
    std::cout << std::endl;
}

bool isInStack(std::stack<Coordinate> stack, Coordinate coord) {
    while (!stack.empty()) {
        if (compareCoordinate(stack.top(), coord)) {
            return true;
        }
        stack.pop();
    }
    return false;
}

std::stack<Coordinate> reverseStack(std::stack<Coordinate> stack) {
    std::stack<Coordinate> reverseStack;
    while (!stack.empty()) {
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

int getMazeCode() {
    return mazeTemp[mouseY][mouseX];
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

void goToCoord(int x, int y) {
}

void startFloodFill() {
    float turn = 0;
    int step = 0;
    std::stack<Coordinate> stackStep;
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

    visitedCoord.push({mouseX, mouseY});
    // Mark the starting point
    mazeWeight[mouseY][mouseX] = 0;
    mazeVisited[mouseY][mouseX] = true;

    while (!stackStep.empty()) {
        std::cout << "Notification: Loading Maze" << std::endl;
        maze[mouseY][mouseX] = getMazeCode();
        Coordinate next = stackStep.top(); // Peek the top coordinate
        std::cout << "Next Coord: " << getCoordString(next) << std::endl;
        std::cout << "Mouse Coord: " << getCoordString({mouseX, mouseY}) << std::endl;
        if (isCoordVisited(next)) {
            stackStep.pop();
            continue;
        }
        // Adjust direction to face the current target
        if (onFront(next) && !hasWallFront()) {
            goStraight();
            step += 1;
            // Mark the position as visited after moving
            maze[mouseY][mouseX] = getMazeCode();
            mazeVisited[mouseY][mouseX] = true;
            stackStep.pop();
        } else if (onTheLeft(next) && !hasWallLeft()) {
            turnLeft90();
            turn += 1;
            // goStraight();
            // stackStep.pop();
            continue;
        } else if (onTheRight(next) && !hasWallRight()) {
            turnRight90();
            turn += 1;
            // goStraight();
            // stackStep.pop();
            continue;
        } else if (onBehind(next)) {
            turnRight180();
            turn += 1.5;
            // goStraight();
            // stackStep.pop();
        } else {
            turnRight180();
            turn += 1.5;
            // if (mouseX == visitedCoord.top().x && mouseY == visitedCoord.top().y)
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
                        turn += 1;
                        continue;
                    }
                    if (onTheRight(visitedCoord.top())) {
                        turnRight90();
                        turn += 1;
                        continue;
                    }
                    if (onBehind(visitedCoord.top())) {
                        turnRight180();
                        turn += 1.5;
                        continue;
                    }
                    if (onFront(visitedCoord.top())) {
                        goStraight();
                        step += 1;
                        visitedCoord.pop();
                        // visitedCoord.pop();
                        clear_screen();
                        printMazeCodeWithWall();
                        printStack(visitedCoord, "Visited");
                        printStack(stackStep, "Steps");
                        Sleep(pauseTime);
                        checkpoint();
                    }
                }
                if (onTheLeft(next) && !hasWallLeft()) {
                    turnLeft90();
                    turn += 1;
                    std::cout << "On Left" << std::endl;
                    break;
                }
                if (onTheRight(next) && !hasWallRight()) {
                    turnRight90();
                    turn += 1;
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


        if (!hasWallRight() && !isCoordVisited(getRightCord()) && getRightCord().x != -1 && getRightCord().y != -1) {
            stackStep.push(getRightCord());
        }

        if (!hasWallFront() && !isCoordVisited(getFrontCord()) && getFrontCord().x != -1 && getFrontCord().y != -1) {
            stackStep.push(getFrontCord());
        }
        if (!hasWallLeft() && !isCoordVisited(getLeftCord()) && getLeftCord().x != -1 && getLeftCord().y != -1) {
            stackStep.push(getLeftCord());
        }


        // Visualization
        printMazeCodeWithWall();
        printStack(visitedCoord, "Visited");
        printStack(stackStep, "Steps");
        // Simulate a pause
        checkpoint(); // Pause for input
        Sleep(pauseTime);
        clear_screen();
    }
    std::cout << "Step: " << step << ", Turn: " << turn << std::endl;
    getchar();

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

Coordinate getBackCord() {
    switch (mouseDirection) {
        case 0:
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1};
            }

            return {mouseX, -1};

        case 1:
            if (mouseY > 0) {
                return {mouseX, mouseY - 1};
            }
            return {mouseX, -1};

        case 2:
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY};
            }
            return {-1, mouseY};

        case 3:

            if (mouseX > 0) {
                return {mouseX - 1, mouseY};
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

bool isGoal(Coordinate coord) {
    /*
     +   +---+            +---+---+             TOP-LEFT:       1 || 4
     |       |                    |             TOP-RIGHT:      9
     +   +   +     OR     +   +   +             BOTTOM-LEFT:    6
     |       |            |       |             BOTTOM-RIGHT:   10
     +---+---+            +---+---+
    */
    if ((maze[coord.y][coord.x] == 1 || maze[coord.y][coord.x] == 4) //Top-Left
        && maze[coord.y][coord.x + 1] == 9 //Top-Right
        && maze[coord.y + 1][coord.x] == 6 //Bottom-Left
        && maze[coord.y + 1][coord.x + 1] == 10) //Bottom-Right
    {
        setGoal({coord.x, coord.y});
        return true;
    }
    /*
     +---+   +            +---+---+             TOP-LEFT:       5
     |       |            |                     TOP-RIGHT:      1 || 8
     +   +   +     OR     +   +   +             BOTTOM-LEFT:    6
     |       |            |       |             BOTTOM-RIGHT:   10
     +---+---+            +---+---+
    */
    if (maze[coord.y][coord.x] == 5 //Top-Left
        && (maze[coord.y][coord.x + 1] == 1 || maze[coord.y][coord.x + 1] == 8) //Top-Right
        && maze[coord.y + 1][coord.x] == 6 //Bottom-Left
        && maze[coord.y + 1][coord.x + 1] == 10) //Top-Right
    {
        setGoal({coord.x + 1, coord.y});
        return true;
    }
    /*
     +---+---+            +---+---+             TOP-LEFT:       5
     |       |            |       |             TOP-RIGHT:      9
     +   +   +     OR     +   +   +             BOTTOM-LEFT:    2 || 4
             |            |       |             BOTTOM-RIGHT:   10
     +---+---+            +   +---+
    */
    if (maze[coord.y][coord.x] == 5 //Top-Left
        && maze[coord.y][coord.x + 1] == 9 //Top-Right
        && (maze[coord.y + 1][coord.x] == 2 || maze[coord.y + 1][coord.x] == 4) //Bottom-Left
        && maze[coord.y + 1][coord.x + 1] == 10) //Bottom-Right
    {
        setGoal({coord.x, coord.y + 1});
        return true;
    }
    /*
     +---+---+            +---+---+             TOP-LEFT:       5
     |       |            |       |             TOP-RIGHT:      9
     +   +   +     OR     +   +   +             BOTTOM-LEFT:    6
     |                    |       |             BOTTOM-RIGHT:   2 || 8
     +---+---+            +---+   +
    */
    if (maze[coord.y][coord.x] == 5 //Top-Left
        && maze[coord.y + 1][coord.x] == 6 //Bottom-Left
        && maze[coord.y][coord.x + 1] == 9 //Top-Right
        && (maze[coord.y + 1][coord.x + 1] == 2 || maze[coord.y + 1][coord.x + 1] == 8)) //Bottom-Right
    {
        setGoal({coord.x + 1, coord.y + 1});
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
            if (isGoal({x, y}))
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
    std::queue<Coordinate> queueNext;
    queueNext.push({goalX, goalY});
    while (!queueNext.empty()) {
        Coordinate current = queueNext.front();
        if (!hasWallTopByMaze(current) && getTopWeight(current) == -1)
            queueNext.push({current.x, current.y - 1});
        if (!hasWallBottomByMaze(current) && getBottomWeight(current) == -1)
            queueNext.push({current.x, current.y + 1});
        if (!hasWallLeftByMaze(current) && getLeftWeight(current) == -1)
            queueNext.push({current.x - 1, current.y});
        if (!hasWallRightByMaze(current) && getRightWeight(current) == -1)
            queueNext.push({current.x + 1, current.y});
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
    return {coord.x, coord.y - 1};
}

Coordinate getBottomCoordByCoordinate(Coordinate coord) {
    return {coord.x, coord.y + 1};
}

Coordinate getLeftCoordByCoordinate(Coordinate coord) {
    return {coord.x - 1, coord.y};
}

Coordinate getRightCoordByCoordinate(Coordinate coord) {
    return {coord.x + 1, coord.y};
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

std::stack<Coordinate> findShortestPath(Coordinate start, Coordinate goal, int mode) {
    std::stack<Coordinate> path;
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
    return path; // If no path is found, return the empty stack
}

void followPath(std::stack<Coordinate> path) {
    while (!path.empty()) {
        clear_screen();
        printShortestPathMaze();
        Sleep(pauseTime);
        Coordinate next = path.top();
        if (compareCoordinate(next, {mouseX, mouseY})) {
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
