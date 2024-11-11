#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>

using namespace std;

// Structura pentru coordonatele unui obiect
struct Coordinate {
    int x;
    int y;
};

// Structura pentru scorul unui jucător
struct PlayerScore {
    string name;
    int score;
};

// Setarile si variabilele jocului
const int width = 31;   // Lațimea zonei de joc
const int height = 11;  // Inaltimea zonei de joc
Coordinate snakeHead;   // Pozitia capului sarpelui
Coordinate fruit;       // Pozitia fructului
Coordinate tail[100];   // Coordonatele cozii sarpelui
int tailLength;         // Lungimea cozii sarpelui
int score;              // Scorul actual
PlayerScore playerScore[100];  // Lista scorurilor jucatorilor
int playerScoreLength = 0;     // Lungimea listei de scoruri
int sleepTime = 100;           // Timpul de intarziere pentru viteza jocului
enum Dir { STOP, LEFT, RIGHT, UP, DOWN };  // Directiile de miscare
Dir dir;                       // Directia curenta

// Declararea funcțiilor
void Setup();
void AddScore(PlayerScore playerScore[], int& size, PlayerScore current);
void DisplayTopScores(PlayerScore playerScore[], int size);
void SetCursorPosition(int x, int y);
void DrawFrame();
void DrawGame();
void Input();
void Logic();
void ShowGameOverScreen();
void ResetGame();
void HideCursor();

// Initializarea jocului
void Setup() {
    srand(time(0));
    dir = STOP;
    snakeHead = { width / 2, height / 2 }; // Pozitionam sarpele in centru
    fruit = { rand() % width, rand() % height }; // Pozitie aleatorie pentru fruct
    score = 0;
    tailLength = 0;
}

// Functie pentru ascunderea cursorului
void HideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

// Adaugarea unui scor nou in lista de scoruri
void AddScore(PlayerScore playerScore[], int& size, PlayerScore current) {
    if (size < 100) {
        int i;
        for (i = size - 1; i >= 0 && playerScore[i].score < current.score; --i) {
            playerScore[i + 1] = playerScore[i];
        }
        playerScore[i + 1] = current;
        size++;
    }
    else if (current.score > playerScore[size - 1].score) {
        int i;
        for (i = size - 2; i >= 0 && playerScore[i].score < current.score; --i) {
            playerScore[i + 1] = playerScore[i];
        }
        playerScore[i + 1] = current;
    }
}

// Afisarea celor mai bune scoruri
void DisplayTopScores(PlayerScore playerScore[], int size) {
    system("cls");
    cout << "------ Top Scoruri ------\n";
    int limit = (size < 10) ? size : 10;
    for (int i = 0; i < limit; ++i) {
        cout << (i + 1) << ". " << playerScore[i].name << " - " << playerScore[i].score << "\n";
    }
    cout << "\nApasa 'R' pentru a juca din nou sau 'Q' pentru a iesi.\n";
}

// Setarea pozitiei cursorului pe ecran
void SetCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Desenarea cadrului de joc
void DrawFrame() {
    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;
    for (int i = 0; i < height; i++) {
        cout << "#";
        for (int j = 0; j < width; j++) {
            cout << " ";
        }
        cout << "#" << endl;
    }
    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;
}

// Desenarea jocului
void DrawGame() {
    for (int i = 0; i < height; i++) {
        SetCursorPosition(1, i+1);
        for (int j = 0; j < width; j++) {
            if (i == snakeHead.y && j == snakeHead.x) {
                if (dir == STOP) cout << "+";
                if (dir == UP) cout << "l";
                if (dir == DOWN) cout << "l";
                if (dir == LEFT) cout << "-";
                if (dir == RIGHT) cout << "-";
            }
            else if (i == fruit.y && j == fruit.x)
                cout << "$";
            else {
                bool printTail = false;
                for (int k = 0; k < tailLength; k++) {
                    if (tail[k].x == j && tail[k].y == i) {
                        cout << "+";
                        printTail = true;
                    }
                }
                if (!printTail) cout << " ";
            }
        }
    }

    // Afisam scorul dupa zona de joc
    SetCursorPosition(7, height + 3);
    cout << "Scor: " << score << endl;
}

// Functie pentru a primi input de la utilizator
void Input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'w': if (dir != DOWN) dir = UP; sleepTime = 200; break;
            case 's': if (dir != UP) dir = DOWN; sleepTime = 200; break;
            case 'a': if (dir != RIGHT) dir = LEFT; sleepTime = 100; break;
            case 'd': if (dir != LEFT) dir = RIGHT; sleepTime = 100; break;
            case 'p': exit(0);
        }
    }
}

// Logica jocului
void Logic() {
    Coordinate prev = tail[0];
    Coordinate prev2;
    tail[0] = snakeHead;

    for (int i = 1; i < tailLength; i++) {
        prev2 = tail[i];
        tail[i] = prev;
        prev = prev2;
    }

    switch (dir) {
        case UP:    snakeHead.y--; break;
        case DOWN:  snakeHead.y++; break;
        case LEFT:  snakeHead.x--; break;
        case RIGHT: snakeHead.x++; break;
    }

    // Trateaza iesirea din ecran
    if (snakeHead.x >= width) snakeHead.x = 0;
    else if (snakeHead.x < 0) snakeHead.x = width - 1;
    if (snakeHead.y >= height) snakeHead.y = 0;
    else if (snakeHead.y < 0) snakeHead.y = height - 1;

    // Verifica coliziunea cu coada
    for (int i = 0; i < tailLength; i++) {
        if (tail[i].x == snakeHead.x && tail[i].y == snakeHead.y) {
            ShowGameOverScreen();
            return;
        }
    }

    // Verifica coliziunea cu fructul
    if (snakeHead.x == fruit.x && snakeHead.y == fruit.y) {
        score += 1;
        fruit = { rand() % width, rand() % height };
        tailLength++;
    }
}

// Afiseaza ecranul de Game Over
void ShowGameOverScreen() {
    cout << "Game Over! Introdu numele tau: ";
    string name;
    cin >> name;

    AddScore(playerScore, playerScoreLength, {name, score});

    // Afiseaza cele mai bune scoruri si optiunea de a juca din nou
    DisplayTopScores(playerScore, playerScoreLength);

    // Asteapta alegerea jucatorului pentru a rejuca sau a iesi
    char choice;
    do {
        choice = _getch();
        if (choice == 'R' || choice == 'r') {
            ResetGame(); // Reporneste jocul daca este apasata tasta 'R'
            return;
        }
        else if (choice == 'Q' || choice == 'q') {
            exit(0); // Iesire din joc daca este apasata tasta 'Q'
        }
    } while (choice != 'R' && choice != 'r' && choice != 'Q' && choice != 'q');
}

// Reseteaza jocul
void ResetGame() {
    system("cls");
    Setup();
    DrawFrame();
    dir = STOP;
}

// Functia principala
int main() {
    Setup();
    DrawFrame();
    HideCursor();
    while (true) {
        Input();
        Logic();
        DrawGame();
        Sleep(sleepTime); // Controlul vitezei jocului
    }
}
