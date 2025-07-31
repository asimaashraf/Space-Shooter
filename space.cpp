#include <iostream>
#include <conio.h>
#include <vector>
#include <windows.h>
#include <fstream>
#include <ctime>
using namespace std;
void saveScore(int finalScore);  


const int width = 40, height = 20;
int spaceshipX, score;
bool gameOver;
int gameColor = 1;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
const int maxEnemies = 4;
int alienX[maxEnemies], alienY[maxEnemies];

vector<int> bulletYs;  // Store bullet Y positions


int customRandom(int min, int max) {
    return min + rand() % (max - min + 1);
}

void gotoxy(int x, int y) {
    COORD pos = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void clearScreen() {
    system("cls");
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void flushInputBuffer() {
    while (_kbhit()) _getch();
}


void drawField() {
    setColor(gameColor); // Set color for this frame

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == 0 || x == width - 1) {
                cout << '#'; // Borders
            } else if (y == height - 2 && x >= spaceshipX && x <= spaceshipX + 2) {
                cout << (x == spaceshipX ? '<' : (x == spaceshipX + 1 ? '^' : '>'));
            } else {
                bool printed = false;

                // Enemies
                for (int i = 0; i < maxEnemies; i++) {
                    if (y == alienY[i] && x == alienX[i]) {
                        cout << 'M';
                        printed = true;
                        break;
                    }
                }

                // Bullets
                if (!printed) {
                    for (int i = 0; i < bulletYs.size(); i++) {
                        if (y == bulletYs[i] && x == spaceshipX + 1) {
                            cout << '|';
                            printed = true;
                            break;
                        }
                    }
                }

                if (!printed) cout << ' ';
            }
        }
        cout << endl;
    }

        // Draw bottom boundary
    for (int i = 0; i < width; i++) cout << '#';
    cout << endl;

    setColor(7); // Reset color for text
    cout << "Score: " << score << "  Controls: A/D to move, Space to shoot, Esc to exit\n";

    // Cycle color
    gameColor++;
    if (gameColor > 15 || gameColor == 8) gameColor = 1;  // skip gray
}


void moveToTopLeft() {
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void handleInput() {
    if (GetAsyncKeyState(VK_ESCAPE)) gameOver = true;
    if (GetAsyncKeyState(0x41)) {
        if (spaceshipX > 1) spaceshipX--;
    }
    if (GetAsyncKeyState(0x44)) {
        if (spaceshipX < width - 4) spaceshipX++;
    }
        if (GetAsyncKeyState(VK_SPACE)) {
        bulletYs.push_back(height - 3);
    }

}

void updateLogic() {
        for (int i = 0; i < maxEnemies; i++) {
        alienX[i] = (alienX[i] >= width - 2) ? 1 : alienX[i] + 1;
        if (alienX[i] == 1) alienY[i]++;
        if (alienY[i] == height - 2) gameOver = true;
    }

    // Update bullets
    for (int i = 0; i < bulletYs.size(); ) {
        bulletYs[i]--;
        bool hit = false;

        for (int j = 0; j < maxEnemies; j++) {
            if (bulletYs[i] == alienY[j] && spaceshipX <= alienX[j] && spaceshipX + 2 >= alienX[j]) {
                score++;
                alienX[j] = customRandom(1, width - 2);
                alienY[j] = 1;
                hit = true;
                break;
            }
        }

        if (bulletYs[i] < 1 || hit) {
            bulletYs.erase(bulletYs.begin() + i);
        } else {
            i++;
        }
    }

}

 


int loadHighScore() {
    ifstream inFile("score.txt");
    int highScore = 0, scoreInFile;
    while (inFile >> scoreInFile) {
        if (scoreInFile > highScore) highScore = scoreInFile;
    }
    return highScore;
}

void showInstructions() {
    clearScreen();
    cout << "INSTRUCTIONS:\n";
    cout << "1. Use 'A' to move left\n";
    cout << "2. Use 'D' to move right\n";
    cout << "3. Press SPACE to shoot\n";
    cout << "4. Hit the enemy (M) to score\n";
    cout << "\nPress any key to return to menu...";
    _getch();
     
}

void showScores() {
    clearScreen();
    int high = loadHighScore();
    cout << "HIGH SCORE: " << high << endl;
    cout << "\nPress any key to return to menu...";
    _getch();
        flushInputBuffer();  // Clear buffer before returning to menu

}

void showMenu() {
    string title = " SPACE SHOOTER ";
    SetConsoleTextAttribute(h, 14);
    int color = 1;

    // Static part: print once
    clearScreen();
    gotoxy(10, 2); cout << "========================";
    gotoxy(10, 6); cout << "========================";
    gotoxy(12, 8); cout << "1. Play";
    gotoxy(12, 9); cout << "2. Instructions";
    gotoxy(12, 10); cout << "3. Score";
    gotoxy(12, 11); cout << "4. Quit";

    while (true) {
        // Blink only the title
        setColor(color);
        gotoxy(13, 4); cout << title;
        setColor(7); // Reset to white

        Sleep(200);
        color++;
        if (color > 15) color = 1;

        // Check for key press
        if (_kbhit()) {
            char op = _getch();
            if (op == '1') break;
            else if (op == '2') {
                showInstructions();
                clearScreen();  // Clear and redraw menu after returning
                showMenu();
                return;
            }
            else if (op == '3') {
                showScores();
                clearScreen();  // Clear and redraw menu after returning
                showMenu();
                return;
            }
            else if (op == '4') exit(0);
        }
    }
}



void startGame() {
    srand(time(0));
    spaceshipX = width / 2 - 1;
       for (int i = 0; i < maxEnemies; i++) {
        alienX[i] = customRandom(1, width - 2);
        alienY[i] = 1 + i * 2;
    }
    bulletYs.clear();


    while (!gameOver) {
        moveToTopLeft();
        drawField();
        handleInput();
        updateLogic();
        Sleep(100);
    }

       setColor(12);
    cout << "\nGame Over! Your Score: " << score << endl;
    saveScore(score);
    setColor(7);
    cout << "Press any key to return to menu...";
    _getch();

    flushInputBuffer(); // 🛠️ Clear stuck keys

}
void saveScore(int finalScore) {
    ofstream outFile("score.txt", ios::app);  // append mode
    if (outFile.is_open()) {
        outFile << finalScore << endl;
        outFile.close();
    }
}

int main() {
    system("cls");
    while (true) {
        showMenu();
        startGame();
    }
    return 0;
}
