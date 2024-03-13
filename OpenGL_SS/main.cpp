#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

// Ekran boyutları
int screenWidth = 800;
int screenHeight = 600;

// Uzay gemisi pozisyonu ve hızı
float shipX = screenWidth / 2;
float shipY = screenHeight / 2;
float shipSpeed = 5.0f;

// Uzay gemisi dönme açısı
float shipAngle = 0.0f;

// Klavye kontrolleri
bool keyLeft = false;
bool keyRight = false;
bool keyUp = false;
bool keyDown = false;

// Mermi yapısı
struct Bullet {
    float x;
    float y;
    float angle;
};

std::vector<Bullet> bullets;

// Mermi ofseti
const float bulletOffset = 15.0f;

// Asteroid yapısı
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
bool gameStarted = false;


// Oyun durumu
bool gameOver = false;

// İlerleme vektörü hesaplama fonksiyonu
void updateShipPosition() {
    if (gameOver) return;

    if (keyLeft)
        shipAngle -= 5.0f;
    if (keyRight)
        shipAngle += 5.0f;

    // Radyan cinsinden dönüş açısını dereceye çevir
    float angle = (shipAngle + 90.0f) * 3.14159 / 180.0;

    if (keyUp) {
        shipX += shipSpeed * cos(angle);
        shipY += shipSpeed * sin(angle);
    }
    if (keyDown) {
        shipX -= shipSpeed * cos(angle);
        shipY -= shipSpeed * sin(angle);
    }

    // Ekran sınırlarını kontrol et
    if (shipX > screenWidth)
        shipX = 0;
    if (shipX < 0)
        shipX = screenWidth;
    if (shipY > screenHeight)
        shipY = 0;
    if (shipY < 0)
        shipY = screenHeight;
}

// Mermilerin hareketini güncelleme fonksiyonu
void updateBullets() {
    if (gameOver) return;

    for (size_t i = 0; i < bullets.size(); ++i) {
        Bullet& bullet = bullets[i];
        float angle = bullet.angle * 3.14159 / 180.0;
        bullet.x += 10.0f * cos(angle);
        bullet.y += 10.0f * sin(angle);

        // Eğer mermi ekran dışına çıktıysa, listeden çıkar
        if (bullet.x < 0 || bullet.x > screenWidth || bullet.y < 0 || bullet.y > screenHeight) {
            bullets.erase(bullets.begin() + i);
            --i;
        }
    }
}

// Asteroid şekillerini oluşturma fonksiyonu
void createAsteroidShapes() {
    srand(time(NULL));
    const int numVertices = 12; // Asteroid şekli için kenar sayısı
    for (int i = 0; i < 5; ++i) { // Başlangıçta daha fazla asteroid oluştur
        Asteroid asteroid;

        // Rastgele asteroid hızları
        asteroid.speedX = static_cast<float>(rand() % 5 - 2);
        asteroid.speedY = static_cast<float>(rand() % 5 - 2);

        // Asteroidin şeklini oluştur
        for (int j = 0; j < numVertices; ++j) {
            float angle = static_cast<float>(j) * 2.0f * 3.14159 / static_cast<float>(numVertices);
            float radius = static_cast<float>(rand() % 10 + 10); // Rastgele yarıçap
            asteroid.shape.push_back(std::make_pair(radius * cos(angle), radius * sin(angle)));
        }

        // Asteroidi ekranın rastgele bir yerine yerleştir
        asteroid.x = static_cast<float>(rand() % screenWidth);
        asteroid.y = static_cast<float>(rand() % screenHeight);

        asteroids.push_back(asteroid);
    }
}

// Asteroidlerin hareketini güncelleme fonksiyonu
void updateAsteroids() {
    if (gameOver) return;

    // Asteroid sayısını kontrol et
    if (asteroids.size() < 10) {
        createAsteroidShapes();
    }

    for (size_t i = 0; i < asteroids.size(); ++i) {
        Asteroid& asteroid = asteroids[i];
        asteroid.x += asteroid.speedX;
        asteroid.y += asteroid.speedY;
        asteroid.rotationSpeed += 0.1f; // Asteroidlerin dönme hızı sabit artırılıyor

        // Ekran dışına çıkan asteroidleri yeniden konumlandır
        if (asteroid.x < -50) asteroid.x = screenWidth + 50;
        if (asteroid.x > screenWidth + 50) asteroid.x = -50;
        if (asteroid.y < -50) asteroid.y = screenHeight + 50;
        if (asteroid.y > screenHeight + 50) asteroid.y = -50;

        // Gemi-asteroid çarpışmasını kontrol et
        if (sqrt(pow(asteroid.x - shipX, 2) + pow(asteroid.y - shipY, 2)) < 25) {
            // Gemiye çarpma durumu, canı azalt
            health--;
            if (health <= 0) {
                gameOver = true;
            }
            // Asteroidi yeniden konumlandır
            asteroid.x = static_cast<float>(rand() % screenWidth);
            asteroid.y = static_cast<float>(rand() % screenHeight);
        }

        // Mermi-asteroid çarpışmasını kontrol et
        for (size_t j = 0; j < bullets.size(); ++j) {
            Bullet& bullet = bullets[j];
            if (bullet.x > asteroid.x - 25 && bullet.x < asteroid.x + 25 &&
                bullet.y > asteroid.y - 25 && bullet.y < asteroid.y + 25) {
                // Mermi isabet etti, mermiyi ve asteroidi sil
                bullets.erase(bullets.begin() + j);
                asteroids.erase(asteroids.begin() + i);
                --i;
                // Skoru artır
                score += 10;
                break;
            }
        }
    }
}

// Mermi oluşturma fonksiyonu
void createBullet() {
    Bullet bullet;
    float angle = (shipAngle + 90.0f) * 3.14159 / 180.0; // Gemiden bakılan açıyı hesapla
    bullet.x = shipX + bulletOffset * cos(angle); // X koordinatını geminin ucuna göre ayarla
    bullet.y = shipY + bulletOffset * sin(angle); // Y koordinatını geminin ucuna göre ayarla
    bullet.angle = shipAngle; // Mermi açısını geminin dönüş açısına ayarla
    bullets.push_back(bullet);
}

void keyboard(unsigned char key, int x, int y) {
    if (!gameStarted) { // Oyun başlamadıysa
        gameStarted = true; // Oyunu başlat
        return;
    }

    if (key == 'r' || key == 'R') { // Oyuncu R tuşuna bastığında
        if (gameOver) { // Game over durumundaysa
            // Oyunu yeniden başlat
            gameOver = false;
            score = 0;
            health = 3;
            bullets.clear();
            asteroids.clear();
            createAsteroidShapes();
        }
    }

    switch (key) {
        case 'a':
            keyLeft = true;
            break;
        case 'd':
            keyRight = true;
            break;
        case 'w':
            keyUp = true;
            break;
        case 's':
            keyDown = true;
            break;
        case ' ':
            createBullet();
            break;
        default:
            break;
    }
}

// Klavye girişi bırakma işleme fonksiyonu
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

// Ekran yenileme fonksiyonu
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (!gameStarted) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(screenWidth / 2 - 100, screenHeight / 2);
        std::string startText = "Press any key to start";
        for (size_t i = 0; i < startText.length(); ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, startText[i]);
        }
    } else {
        // Oyun ekranı
        // Uzay gemisini çiz
        glPushMatrix();
        glTranslatef(shipX, shipY, 0); // Dönüş merkezi gemi ucuna alındı
        glRotatef(shipAngle, 0, 0, 1); // Geminin dönüş açısı
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex2f(0, 15);
        glVertex2f(-10, -10);
        glVertex2f(10, -10);
        glEnd();
        glPopMatrix();

        // Mermileri çiz
        glColor3f(1.0f, 1.0f, 1.0f);
        for (size_t i = 0; i < bullets.size(); ++i) {
            Bullet& bullet = bullets[i];
            glPushMatrix();
            glTranslatef(bullet.x, bullet.y, 0);
            glBegin(GL_QUADS);
            glVertex2f(-1, -1);
            glVertex2f(1, -1);
            glVertex2f(1, 1);
            glVertex2f(-1, 1);
            glEnd();
            glPopMatrix();
        }

        // Asteroidleri çiz
        glColor3f(1.0f, 1.0f, 1.0f);
        for (size_t i = 0; i < asteroids.size(); ++i) {
            Asteroid& asteroid = asteroids[i];
            glPushMatrix();
            glTranslatef(asteroid.x, asteroid.y, 0);
            glBegin(GL_LINE_LOOP);
            for (size_t j = 0; j < asteroid.shape.size(); ++j) {
                glVertex2f(asteroid.shape[j].first, asteroid.shape[j].second);
            }
            glEnd();
            glPopMatrix();
        }

        // Skoru yazdır
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(10, screenHeight - 20); // Yazının konumunu belirle
        std::string scoreText = "Score: " + std::to_string(score); // Skor metnini oluştur
        for (size_t i = 0; i < scoreText.length(); ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreText[i]); // Metni ekrana yazdır
        }

        // Canı yazdır
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(screenWidth - 100, screenHeight - 20); // Yazının konumunu belirle
        std::string healthText = "Health: " + std::to_string(health); // Can metnini oluştur
        for (size_t i = 0; i < healthText.length(); ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, healthText[i]); // Metni ekrana yazdır
        }

        // Game over mesajını yazdır
        if (gameOver) {
            glColor3f(1.0f, 0.0f, 0.0f);
            glRasterPos2f(screenWidth / 2 - 50, screenHeight / 2);
            std::string gameOverText = "Game Over! Press R to restart";
            for (size_t i = 0; i < gameOverText.length(); ++i) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, gameOverText[i]);
            }
        }
    }

    glFlush();
    glutSwapBuffers();
}


// Pencere boyutu değiştirme fonksiyonu
void reshape(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Oyun güncelleme fonksiyonu
void update(int value) {
    updateShipPosition();
    updateBullets();
    updateAsteroids();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    createAsteroidShapes();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Blasteroids");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
