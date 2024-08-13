#include <iomanip>
#include <iostream>
#include <windows.h>
#include <string>
#include <queue>

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

bool mazeVisited[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH] = {false};

/*
 --- Table of Wall Code ---
     xx
    x  x     0 (None)
     xx

     xx
    |  x     1  (Left)
     xx

     __
    x  x     2  (Top)
     xx

     xx
    x  |     3  (Right)
     xx

     xx
    x  x     4  (Bottom)
     --

     xx
    |  x     5  (Bottom-Left)
     --

     xx
    x  |     6  (Bottom-Right)
     --

     __
    x  |     7  (Top-Right)
     xx

     __
    |  x     8  (Top-Left)
     xx

     xx
    |  |     9  (Left-Right)
     xx

     __
    x  x     10 (Top-Bottom)
     --

     xx
    |  |     11 (Left-Right)
     xx

     __
    x  |     12 (Top-Bottom-Right)
     --

     __
    |  |     13 (Top-Left-Right)
     xx

     --
    |  x     14 (Top-Bottom-Left)
     --

     xx
    |  |     15 (Bottom-Left-Right)
     --

     __
    |  |     16 (Top-Bottom-Left-Right)
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
int mouseDirection = 1;
int mouseX = 0, mouseY = 0;

//-----------------------------------------
void setColor(int color);

void clear_screen();

void resetColor();

void printMaze();

void printMazeWithWall();

void updatePosition(int direction, int x, int y);

void updatePosAndPrintMaze(int direction, int x, int y);

void goStraight();

bool hasWallBefore();

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

void startFloodFill();

Coordinate getFrontCord();

Coordinate getLeftCord();

Coordinate getRightCord();

bool isVisited();

bool isCoordVisited(Coordinate coord);

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
    //     _sleep(1000);
    //     clear_screen();
    // }
    startFloodFill();
    getchar();
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
                    setColor(14);
                    std::cout << "|";
                    resetColor();
                    continue;
                }
                if (x % 2 == 1) {
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
            // if (y%2==0 && x%2==0) {
            //     std::cout << "   +";
            //     continue;
            // }
            // if (y%2==1 && x%2==1) {
            //
            //     continue;
            // }
            // if (true) {
            //
            // } else {
            //
            // }
            if (x % 2 == 1 && y % 2 == 1) {
                if (mouseX == (x - 1) / 2 && mouseY == (y - 1) / 2) {
                    setColor(12);
                    switch (mouseDirection) {
                        case 0:
                            std::cout << " ^ ";
                            break;
                        case 1:
                            std::cout << " < ";
                            break;
                        case 2:
                            std::cout << " > ";
                            break;
                        case 3:
                            std::cout << " v ";
                            break;
                        default:
                            std::cout << mazeWeight[y][x] << "  ";
                            break;
                    }
                    resetColor();
                } else {
                    if (mazeWeight[(y - 1) / 2][(x - 1) / 2] >= 10) {
                        std::cout << std::left << std::setw(3) << mazeWeight[(y - 1) / 2][(x - 1) / 2];
                    } else {
                        std::cout << " " << std::left << std::setw(2) << mazeWeight[(y - 1) / 2][(x - 1) / 2];
                    }
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
                            setColor(14);
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
                            setColor(14);
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

void goStraight() {
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
                    std::cout << "Error: Wall before !" << std::endl;
                    return;
                default:
                    if (mouseX < MAZE_MAX_HEIGHT - 1) {
                        updatePosition(mouseDirection, mouseX, mouseY + 1);
                    } else {
                        std::cout << "Error: out of maze (Y=" << mouseY << ") !" << std::endl;
                    }
                    break;
            }
            break;

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
                    std::cout << "Error: Wall before !" << std::endl;
                    return;
                default:
                    if (mouseX < MAZE_MAX_WIDTH - 1) {
                        updatePosition(mouseDirection, mouseX - 1, mouseY);
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
    std::cout << "Mouse: Go Straight" << std::endl;
}

bool hasWallBefore() {
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
}

bool hasWallLeft() {
    switch (mouseDirection) {
        case 0:
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

        case 1:
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

        case 2:
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

        case 3:
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

        default:
            return false;
    }
}

bool hasWallRight() {
    switch (mouseDirection) {
        case 0:
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

        case 1:
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

        case 2:
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

        case 3:
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
        default:
            return false;
    }
}

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
}

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
}

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
}

void turnRight180() {
    switch (mouseDirection) {
        case 0:
            mouseDirection = 3;
            break;
        case 1:
            mouseDirection = 2;
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
    std::cout << "Mouse: Turn Right 190*" << std::endl;
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
    std::queue<Coordinate> qStep;
    qStep.push({mouseX, mouseY});
    // Đặt điểm bắt đầu
    mazeWeight[mouseY][mouseX] = 0;
    mazeVisited[mouseY][mouseX] = true;

    while (true) {
        int x = mouseX;
        int y = mouseY;
        int currentWeight = mazeWeight[y][x];

        // Cập nhật trọng số cho các ô lân cận
        if (y > 0 && !hasWallBefore() && !mazeVisited[y - 1][x]) {
            // Kiểm tra hướng lên
            mazeWeight[y - 1][x] = currentWeight + 1;
        }
        if (y < MAZE_MAX_HEIGHT - 1 && !hasWallBefore() && !mazeVisited[y + 1][x]) {
            // Kiểm tra hướng xuống
            mazeWeight[y + 1][x] = currentWeight + 1;
        }
        if (x > 0 && !hasWallBefore() && !mazeVisited[y][x - 1]) {
            // Kiểm tra hướng trái
            mazeWeight[y][x - 1] = currentWeight + 1;
        }
        if (x < MAZE_MAX_WIDTH - 1 && !hasWallBefore() && !mazeVisited[y][x + 1]) {
            // Kiểm tra hướng phải
            mazeWeight[y][x + 1] = currentWeight + 1;
        }

        // Đánh dấu ô hiện tại đã được khám phá
        mazeVisited[y][x] = true;

        // Tìm hướng có trọng số thấp nhất
        int minWeight = 999;
        int nextDirection = -1;

        // Kiểm tra các ô lân cận để tìm ô có trọng số nhỏ nhất và chưa được khám phá
        if (y > 0 && mazeVisited[y - 1][x] && mazeWeight[y - 1][x] < minWeight) {
            minWeight = mazeWeight[y - 1][x];
            nextDirection = 0;
        }
        if (y < MAZE_MAX_HEIGHT - 1 && mazeVisited[y + 1][x] && mazeWeight[y + 1][x] < minWeight) {
            minWeight = mazeWeight[y + 1][x];
            nextDirection = 1;
        }
        if (x > 0 && mazeVisited[y][x - 1] && mazeWeight[y][x - 1] < minWeight) {
            minWeight = mazeWeight[y][x - 1];
            nextDirection = 2;
        }
        if (x < MAZE_MAX_WIDTH - 1 && mazeVisited[y][x + 1] && mazeWeight[y][x + 1] < minWeight) {
            minWeight = mazeWeight[y][x + 1];
            nextDirection = 3;
        }

        // Nếu không tìm thấy hướng nào, kết thúc
        if (nextDirection == -1) {
            std::cout << "Mouse: Finished Exploring!" << std::endl;
            break;
        }

        // Xoay chuột về hướng cần di chuyển
        while (mouseDirection != nextDirection) {
            turnRight90();
        }

        // Di chuyển về phía trước
        goStraight();

        // In trạng thái mê cung sau khi di chuyển
        clear_screen();
        printMazeWithWall();
        _sleep(1000); // Đợi 0.5 giây để dễ theo dõi

        // Nếu phát hiện đã đến đích, có thể dừng lại hoặc tiếp tục tìm kiếm
        if (mouseX == MAZE_MAX_WIDTH / 2 && mouseY == MAZE_MAX_HEIGHT / 2) {
            std::cout << "Mouse: Reached Goal!" << std::endl;
            break;
        }
    }
}

void goToCoord(int x, int y) {
}

void startFloodFill() {
    std::queue<Coordinate> qStep;
    qStep.push({mouseX, mouseY});
    // Đặt điểm bắt đầu
    mazeWeight[mouseY][mouseX] = 0;
    mazeVisited[mouseY][mouseX] = true;

    while (!qStep.empty()) {
        if (!hasWallBefore() && isVisited()) {
            qStep.push(getFrontCord());
        }
        if (!hasWallLeft() && isCoordVisited(getLeftCord())) {
            qStep.push(getLeftCord());
        }
        if (!hasWallRight() && isCoordVisited(getRightCord())) {
            qStep.push(getRightCord());
        }
        goStraight();
        printMazeWithWall();
    }
}

Coordinate getFrontCord() {
    switch (mouseDirection) {
        case 0:
            if (mouseY > 0) {
                return {mouseX, mouseY - 1};
            }
            return {-1, -1};

        case 1:
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1};
            }
            return {-1, -1};
        case 2:
            if (mouseX > 0) {
                return {mouseX - 1, mouseY};
            }
            return {-1, -1};
        case 3:
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY};
            }
            return {-1, -1};
        default:
            return {-1, -1};
    }
}

Coordinate getLeftCord() {
    switch (mouseDirection) {
        case 0:
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY};
            }
            return {-1, -1};

        case 1:
            if (mouseX > 0) {
                return {mouseX - 1, mouseY};
            }
            return {-1, -1};

        case 2:
            if (mouseY > 0) {
                return {mouseX, mouseY - 1};
            }
            return {-1, -1};

        case 3:
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1};
            }
            return {-1, -1};

        default:
            return {-1, -1};
    }
}

Coordinate getRightCord() {
    switch (mouseDirection) {
        case 0:
            if (mouseX > 0) {
                return {mouseX - 1, mouseY};
            }
            return {-1, -1};

        case 1:
            if (mouseX < MAZE_MAX_WIDTH - 1) {
                return {mouseX + 1, mouseY};
            }
            return {-1, -1};

        case 2:
            if (mouseY < MAZE_MAX_HEIGHT - 1) {
                return {mouseX, mouseY + 1};
            }
            return {-1, -1};

        case 3:
            if (mouseY > 0) {
                return {mouseX, mouseY - 1};
            }
            return {-1, -1};

        default:
            return {-1, -1};
    }
}

bool isVisited() {
    return mazeVisited[mouseY][mouseX];
}

bool isCoordVisited(Coordinate coord) {
    return mazeVisited[coord.y][coord.x];
}
