#include <iostream>
#include <graphics.h>
#include <cmath>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>

// Screen dimensions
int screenWidth = 800;
int screenHeight = 600;

// Ship position and speed
float shipX = screenWidth / 2;
float shipY = screenHeight / 2;
float shipSpeed = 5.0f;

// Ship rotation angle
float shipAngle = 0.0f;

// Keyboard controls
bool keyLeft = false;
bool keyRight = false;
bool keyUp = false;
bool keyDown = false;

// Bullet structure
struct Bullet {
  float x;
  float y;
  float angle;
};

std::vector<Bullet> bullets;

// Bullet offset
const float bulletOffset = 15.0f;

// Asteroid structure
struct Asteroid {
  float x;
  float y;
  float speedX;
  float speedY;
  float rotationSpeed;
  std::vector<std::pair<float, float>> shape;
};

std::vector<Asteroid> asteroids;

int score = 0;
int health = 3;
bool gameOver = false;

void updateShipAngle() {
  if (keyLeft) {
    shipAngle -= 5.0f; // Adjust the rotation angle for turning left
  }
  if (keyRight) {
    shipAngle += 5.0f; // Adjust the rotation angle for turning right
  }
}

void updateShipPosition() {
  if (gameOver) return;

  float angle_radians = (shipAngle + 90.0f) * 3.14159 / 180.0;
  float cosAngle = cos(angle_radians);
  float sinAngle = sin(angle_radians);

  float movementX = 0.0f, movementY = 0.0f;
  if (keyUp) {
    movementX += shipSpeed * cosAngle;
    movementY += shipSpeed * sinAngle;
  } else if (keyDown) {
    movementX -= shipSpeed * cosAngle;
    movementY -= shipSpeed * sinAngle;
  }

  if (keyLeft && keyRight) {
    movementX *= 0.707f;
    movementY *= 0.707f;
  }

  shipX += movementX;
  shipY += movementY;

  if (shipX < 0) shipX += screenWidth;
  else if (shipX > screenWidth) shipX -= screenWidth;
  if (shipY < 0) shipY += screenHeight;
  else if (shipY > screenHeight) shipY -= screenHeight;
}

void updateBullets() {
  if (gameOver) return;

  for (size_t i = 0; i < bullets.size(); ++i) {
    Bullet& bullet = bullets[i];
    float angle = bullet.angle * 3.14159 / 180.0;
    bullet.x += 10.0f * cos(angle);
    bullet.y += 10.0f * sin(angle);

    if (bullet.x < 0 || bullet.x > screenWidth || bullet.y < 0 || bullet.y > screenHeight) {
      bullets.erase(bullets.begin() + i);
      --i;
    }
  }
}

void createAsteroidShapes() {
  srand(time(NULL));
  const int numVertices = 12;
  for (int i = 0; i < 5; ++i) {
    Asteroid asteroid;
    asteroid.speedX = static_cast<float>(rand() % 5 - 2);
    asteroid.speedY = static_cast<float>(rand() % 5 - 2);

    for (int j = 0; j < numVertices; ++j) {
      float angle = static_cast<float>(j) * 2.0f * 3.14159 / static_cast<float>(numVertices);
      float radius = static_cast<float>(rand() % 10 + 10);
      asteroid.shape.push_back(std::make_pair(radius * cos(angle), radius * sin(angle)));
    }

    asteroid.x = static_cast<float>(rand() % screenWidth);
    asteroid.y = static_cast<float>(rand() % screenHeight);

    asteroids.push_back(asteroid);
  }
}

void updateAsteroids() {
  if (gameOver) return;

  if (asteroids.size() < 10) {
    createAsteroidShapes();
  }

  for (size_t i = 0; i < asteroids.size(); ++i) {
    Asteroid& asteroid = asteroids[i];
    asteroid.x += asteroid.speedX;
    asteroid.y += asteroid.speedY;
    asteroid.rotationSpeed += 0.1f;

    if (asteroid.x < -50) asteroid.x = screenWidth + 50;
    if (asteroid.x > screenWidth + 50) asteroid.x = -50;
    if (asteroid.y < -50) asteroid.y = screenHeight + 50;
    if (asteroid.y > screenHeight + 50) asteroid.y = -50;

    if (sqrt(pow(asteroid.x - shipX, 2) + pow(asteroid.y - shipY, 2)) < 25) {
      health--;
      if (health <= 0) {
        gameOver = true;
      }
      asteroid.x = static_cast<float>(rand() % screenWidth);
      asteroid.y = static_cast<float>(rand() % screenHeight);
    }

    for (size_t j = 0; j < bullets.size(); ++j) {
      Bullet& bullet = bullets[j];
      if (bullet.x > asteroid.x - 25 && bullet.x < asteroid.x + 25 &&
        bullet.y > asteroid.y - 25 && bullet.y < asteroid.y + 25) {
        bullets.erase(bullets.begin() + j);
        asteroids.erase(asteroids.begin() + i);
        --i;
        score += 10;
        break;
      }
    }
  }
}

void createBullet() {
  Bullet bullet;
  float angle = (shipAngle + 90.0f) * 3.14159 / 180.0;
  bullet.x = shipX + bulletOffset * cos(angle);
  bullet.y = shipY + bulletOffset * sin(angle);
  bullet.angle = shipAngle;
  bullets.push_back(bullet);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'a':
      keyLeft = true;
      keyRight = false;
      keyUp = false;
      keyDown = false;
      break;
    case 'd':
      keyLeft = false;
      keyRight = true;
      keyUp = false;
      keyDown = false;
    break;
    case 'w':
      keyLeft = false;
      keyRight = false;
      keyUp = true;
      keyDown = false;
      break;
    case 's':
        keyLeft = false;
      keyRight = false;
      keyUp = false;
      keyDown = true;
      break;
    case ' ':
      createBullet();
      break;
    case 'r':
    case 'R':
      if (gameOver) {
        gameOver = false;
        health = 3;
        score = 0;
        bullets.clear();
        asteroids.clear();
        createAsteroidShapes();
      }
      break;
    default:
      break;
  }
}

void keyboardUp(unsigned char key, int x, int y) {
  switch (key) {
    case 'a':
      keyLeft = false;
      break;
    case 'd':
      keyRight = false;
      break;
    case 'w':
      keyUp = false;
      break;
    case 's':
      keyDown = false;
      break;
    default:
      break;
  }
}

void drawShip() {
  setcolor(WHITE);
  float angle_radians = (shipAngle + 90.0f) * 3.14159 / 180.0;
  float cosAngle = cos(angle_radians);
  float sinAngle = sin(angle_radians);

  float shipWidth = 20.0f;
  float shipHeight = 25.0f;
  float shipNoseX = 0.0f;
  float shipNoseY = -shipHeight / 2;
  float shipLeftX = -shipWidth / 2;
  float shipLeftY = shipHeight / 2;
  float shipRightX = shipWidth / 2;
  float shipRightY = shipHeight / 2;

  float rotatedNoseX = shipX + (shipNoseX * cosAngle - shipNoseY * sinAngle);
  float rotatedNoseY = shipY + (shipNoseX * sinAngle + shipNoseY * cosAngle);
  float rotatedLeftX = shipX + (shipLeftX * cosAngle - shipLeftY * sinAngle);
  float rotatedLeftY = shipY + (shipLeftX * sinAngle + shipLeftY * cosAngle);
  float rotatedRightX = shipX + (shipRightX * cosAngle - shipRightY * sinAngle);
  float rotatedRightY = shipY + (shipRightX * sinAngle + shipRightY * cosAngle);

  line(rotatedNoseX, rotatedNoseY, rotatedLeftX, rotatedLeftY);
  line(rotatedNoseX, rotatedNoseY, rotatedRightX, rotatedRightY);
  line(rotatedLeftX, rotatedLeftY, rotatedRightX, rotatedRightY);
}

void update() {
  updateShipAngle();
  updateShipPosition();
  updateBullets();
  updateAsteroids();
}

int main() {
  initwindow(screenWidth, screenHeight, "Space Shooter");
  createAsteroidShapes();

  while (true) {
    if (kbhit()) {
      keyboard(getch(), 0, 0);
    }
    update();
    cleardevice();

    drawShip();

    for (size_t i = 0; i < bullets.size(); ++i) {
      Bullet& bullet = bullets[i];
      circle(bullet.x, bullet.y, 2);
    }

    for (size_t i = 0; i < asteroids.size(); ++i) {
      Asteroid& asteroid = asteroids[i];
      for (size_t j = 0; j < asteroid.shape.size() - 1; ++j) {
        line(asteroid.x + asteroid.shape[j].first, asteroid.y + asteroid.shape[j].second,
           asteroid.x + asteroid.shape[j + 1].first, asteroid.y + asteroid.shape[j + 1].second);
      }
      line(asteroid.x + asteroid.shape.back().first, asteroid.y + asteroid.shape.back().second,
         asteroid.x + asteroid.shape.front().first, asteroid.y + asteroid.shape.front().second);
    }

    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    outtextxy(10, screenHeight - 20, scoreText);
    char healthText[50];
    sprintf(healthText, "Health: %d", health);
    outtextxy(screenWidth - 100, screenHeight - 20, healthText);

    if (gameOver) {
      setcolor(RED);
      outtextxy(screenWidth / 2 - 50, screenHeight / 2, "Game Over! Press R to Restart");
    }

    delay(16);
  }

  closegraph();
  return 0;
}
