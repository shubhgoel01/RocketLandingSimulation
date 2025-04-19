#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <ctime>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Rocket properties
float rocketX = 400.0f;
float rocketY = 500.0f;
float rocketAngle = 0.0f;
float velocityX = 0.0f;
float velocityY = 0.0f;
float thrust = 0.0f;
float fuel = 100.0f;
const float GRAVITY = -0.05f;
const float ROTATION_SPEED = 3.0f;
const float rocketWidth = 20.0f;
const float rocketHeight = 40.0f;

// Platform properties
float platformX = 400.0f;
float platformY = 50.0f;
float platformWidth = 100.0f;
float platformHeight = 10.0f;
float platformSpeed = 2.0f;

// Game state
bool isGameOver = false;
bool landedSuccessfully = false;

// Convert degrees to radians
float toRadians(float degrees) {
    return degrees * 3.14159f / 180.0f;
}

// Initialize OpenGL
void init() {
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

// Draw stars in background
void drawStars() {
    srand(42);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 200; i++) {
        float x = rand() % WINDOW_WIDTH;
        float y = rand() % WINDOW_HEIGHT;
        glVertex2f(x, y);
    }
    glEnd();
}

// Draw background
void drawBackground() {
    glColor3f(0.0f, 0.0f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    drawStars();
}

// Draw rocket
void drawRocket() {
    glPushMatrix();
    glTranslatef(rocketX, rocketY, 0.0f);
    glRotatef(rocketAngle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0f, -20.0f);
    glVertex2f(10.0f, -20.0f);
    glVertex2f(10.0f, 20.0f);
    glVertex2f(-10.0f, 20.0f);
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-10.0f, 20.0f);
    glVertex2f(10.0f, 20.0f);
    glVertex2f(0.0f, 40.0f);
    glEnd();

    if (thrust > 0.0f && fuel > 0) {
        glColor3f(1.0f, 0.5f, 0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(-8.0f, -20.0f);
        glVertex2f(-4.0f, -20.0f);
        glVertex2f(-6.0f, -30.0f);
        glVertex2f(4.0f, -20.0f);
        glVertex2f(8.0f, -20.0f);
        glVertex2f(6.0f, -30.0f);
        glEnd();
    }

    glPopMatrix();
}

// Draw platform
void drawPlatform() {
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(platformX - platformWidth / 2, platformY - platformHeight);
    glVertex2f(platformX + platformWidth / 2, platformY - platformHeight);
    glVertex2f(platformX + platformWidth / 2, platformY + platformHeight);
    glVertex2f(platformX - platformWidth / 2, platformY + platformHeight);
    glEnd();
}

// Draw ground
void drawGround() {
    glColor3f(0.5f, 0.3f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(WINDOW_WIDTH, 0.0f);
    glVertex2f(WINDOW_WIDTH, 30.0f);
    glVertex2f(0.0f, 30.0f);
    glEnd();
}

// Draw text
void drawText(const char* text, float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Draw HUD
void drawHUD() {
    std::stringstream ss;
    ss.precision(3);

    ss << std::fixed << "Angle: " << rocketAngle << " deg";
    drawText(ss.str().c_str(), 10, WINDOW_HEIGHT - 30);

    ss.str(""); ss.clear();
    ss << std::fixed << "Velocity Y: " << velocityY;
    drawText(ss.str().c_str(), 10, WINDOW_HEIGHT - 60);

    ss.str(""); ss.clear();
    ss << "Fuel: " << (int)fuel;
    drawText(ss.str().c_str(), 10, WINDOW_HEIGHT - 90);
}

void checkCollision() {
    float rocketBottom = rocketY;
    float rocketTop = rocketY + rocketHeight;
    float rocketLeft = rocketX - rocketWidth / 2;
    float rocketRight = rocketX + rocketWidth / 2;

    float platformTop = platformY + platformHeight;
    float platformLeft = platformX - platformWidth / 2;
    float platformRight = platformX + platformWidth / 2;

    bool outOfBounds = (rocketY > WINDOW_HEIGHT || rocketX < 0 || rocketX > WINDOW_WIDTH);
    bool belowPlatform = (rocketBottom <= platformY - platformHeight && (rocketX < platformLeft || rocketX > platformRight));

    if (rocketBottom <= platformTop &&
        rocketRight >= platformLeft &&
        rocketLeft <= platformRight) {

        isGameOver = true;

        bool verticalSpeedSafe = std::abs(velocityY) <= 2.0f;
        bool horizontalSpeedSafe = std::abs(velocityX) <= 2.0f;
        bool angleSafe = std::abs(rocketAngle) <= 10.0f;
        bool fullyOnPlatform = (rocketX > platformLeft + rocketWidth / 2 &&
                                rocketX < platformRight - rocketWidth / 2);

        landedSuccessfully = verticalSpeedSafe && horizontalSpeedSafe && angleSafe && fullyOnPlatform;

        velocityX = 0;
        velocityY = 0;
        thrust = 0;
    }

    if (outOfBounds || belowPlatform) {
        isGameOver = true;
        landedSuccessfully = false;
        velocityX = 0;
        velocityY = 0;
        thrust = 0;
    }
}

void update(int value) {
    static bool hasPrintedSuccess = false;

    if (!isGameOver) {
        platformX += platformSpeed;
        if (platformX < platformWidth / 2 || platformX > WINDOW_WIDTH - platformWidth / 2)
            platformSpeed = -platformSpeed;

        if (thrust > 0.0f && fuel > 0) {
            velocityY += thrust * std::cos(toRadians(rocketAngle));
            velocityX += thrust * std::sin(toRadians(rocketAngle));
            fuel -= 0.3f;
            if (fuel < 0) fuel = 0;
        }

        velocityY += GRAVITY;
        velocityX *= 0.98f;

        rocketX += velocityX;
        rocketY += velocityY;

        if (rocketX < 0) rocketX = 0;
        if (rocketX > WINDOW_WIDTH) rocketX = WINDOW_WIDTH;
        if (rocketY < 0) rocketY = 0;

        checkCollision();
    }

    if (isGameOver && landedSuccessfully && !hasPrintedSuccess) {
        std::cout << "✅ Landing Successful!\n";
        hasPrintedSuccess = true;
    }

    if (!isGameOver) {
        hasPrintedSuccess = false;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawBackground();
    drawGround();
    drawPlatform();
    drawRocket();
    drawHUD();

    if (isGameOver) {
        if (landedSuccessfully)
            drawText("Success! You Landed!", 300, 300);
        else
            drawText("Crash! Game Over!", 300, 300);

        drawText("Press 'R' to Restart", 300, 270);
    }

    glutSwapBuffers();
}

void specialKeys(int key, int x, int y) {
    if (!isGameOver) {
        switch (key) {
        case GLUT_KEY_UP:
            if (fuel > 0)
                thrust = 0.1f;
            break;
        case GLUT_KEY_LEFT:
            rocketAngle += ROTATION_SPEED;
            if (rocketAngle > 45.0f) rocketAngle = 45.0f;
            break;
        case GLUT_KEY_RIGHT:
            rocketAngle -= ROTATION_SPEED;
            if (rocketAngle < -45.0f) rocketAngle = -45.0f;
            break;
        }
    }
}

void specialKeysUp(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        thrust = 0.0f;
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        rocketX = 400.0f;
        rocketY = 500.0f;
        rocketAngle = 0.0f;
        velocityX = 0.0f;
        velocityY = 0.0f;
        thrust = 0.0f;
        platformX = 400.0f;
        platformSpeed = 2.0f;
        isGameOver = false;
        landedSuccessfully = false;
        fuel = 100.0f;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("2D Rocket Landing Simulator");

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
