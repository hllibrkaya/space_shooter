#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;

const int width = 40;
const int height = 20;
const int asteroidSpeed = 30;

int playerX, playerY;
int score = 0;
int health = 3;

bool gameOver;

enum Direction { STOP = 0, LEFT, RIGHT };

Direction dir;

struct Bullet {
    int x, y;
    bool active;
};

struct Asteroid {
    int x, y;
};

vector<Bullet> bullets;
vector<Asteroid> asteroids;

void Initialize() {
    playerX = width / 2;
    playerY = height - 1;
    gameOver = false;
    dir = STOP;
    score = 0;
    health = 3;
    bullets.clear();
    asteroids.clear();
}

void Draw() {
    system("cls");
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#";
            bool printed = false;
            if (i == playerY && j == playerX) {
                cout << "A";
                printed = true;
            }
            for (const auto& bullet : bullets) {
                if (bullet.active && bullet.x == j && bullet.y == i) {
                    cout << "|";
                    printed = true;
                }
            }
            for (const auto& asteroid : asteroids) {
                if (asteroid.x == j && asteroid.y == i) {
                    cout << "*";
                    printed = true;
                }
            }
            if (!printed)
                cout << " ";
            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;
    cout << "Score: " << score << " Health: " << health << endl;
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a':
                dir = LEFT;
                break;
            case 'd':
                dir = RIGHT;
                break;
            case 'q':
                gameOver = true;
                break;
            case ' ':
                bullets.push_back({playerX, playerY - 1, true});
                break;
            case 's':
                dir = STOP;
                break;
        }
    }
}

void Update() {
    switch (dir) {
        case LEFT:
            if (playerX > 0)
                playerX--;
            break;
        case RIGHT:
            if (playerX < width - 1)
                playerX++;
            break;
        default:
            break;
    }

    // Move asteroids down and create new ones if needed
    for (size_t i = 0; i < asteroids.size(); i++) {
        asteroids[i].y++;
        if (asteroids[i].y >= height) {
            asteroids[i] = {rand() % width, 0};
        }
    }
    if (asteroids.size() < 7) {
        asteroids.push_back({rand() % width, 0});
    }

    // Move bullets up
    for (auto& bullet : bullets) {
        if (bullet.active)
            bullet.y--;
    }

    // Collision detection with bullets and asteroids
    for (auto& bullet : bullets) {
        if (bullet.active) {
            for (auto& asteroid : asteroids) {
                if (bullet.x == asteroid.x && bullet.y == asteroid.y) {
                    bullet.active = false;
                    score++;
                    asteroid = {rand() % width, 0};
                }
            }
        }
    }

    // Collision detection with player and asteroids
    for (auto& asteroid : asteroids) {
        if (asteroid.x == playerX && asteroid.y == playerY) {
            health--;
            asteroid = {rand() % width, 0};
            if (health == 0) {
                gameOver = true;
            }
        }
    }

    // Delay for slower asteroid movement
    this_thread::sleep_for(chrono::milliseconds(asteroidSpeed));
}

int main() {
    srand(time(NULL));

    Initialize();

    while (!gameOver) {
        Draw();
        Input();
        Update();
    }

    cout << "Game Over!" << endl;

    return 0;
}
