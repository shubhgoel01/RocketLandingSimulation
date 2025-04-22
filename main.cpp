#include <GL/glut.h>
#include<bits/stdc++.h>
using namespace std;

// Window dimensions 800 x 600
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Rocket properties
float rocketX = 400.0f; 
float rocketY = 500.0f;     //These defines the starting position of the rocket (rocketx and rocketY defines the centre of the rocket)
float rocketAngle = 0.0f;
float velocityX = 0.0f;
float velocityY = 0.0f;
float thrust = 0.0f;
float fuel = 100.0f;    //Initial values
const float GRAVITY = -0.05f;
const float ROTATION_SPEED = 3.0f;
const float rocketWidth = 20.0f;    //Dimensions of the rocket
const float rocketHeight = 40.0f;

// Platform properties
float platformX = 400.0f;   //Starts form mid - x
float platformY = 50.0f;    //platformX and platformY defines the centre of the platform
float platformWidth = 100.0f;
float platformHeight = 10.0f;
float platformSpeed = 2.0f;     //platform moving speed

// Game state
bool isGameOver = false;
bool landedSuccessfully = false;

//Support variables
bool hasPrintedSuccess = false;

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
    glColor3f(1.0f, 1.0f, 1.0f);    //Set drawing color to white
    glPointSize(1.5f);  
    glBegin(GL_POINTS);
    for (int i = 0; i < 200; i++) { //Generates random x,y positions and draws point on that position
        //This loops runs 200 times generating 200 stars
        float x = rand() % WINDOW_WIDTH;
        float y = rand() % WINDOW_HEIGHT;
        glVertex2f(x, y);
    }
    glEnd();
}

// Draw background why we used this, first we were thinking of dynamic backgrounds: day and night
void drawBackground() {
    glColor3f(0.0f, 0.0f, 0.15f);   //Draws a dark blue background.
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    drawStars();
}


//This function is responsible for:
//Positioning the rocket (glTranslatef)
//Rotating it (glRotatef)
//Drawing the rocket’s body, nose, and flame using OpenGL primitives (quads and triangles)
//We are using translatef and rotatef OpenGL 2D transformations primitives
void drawRocket() {
    glPushMatrix(); //Saves transformation matrix
    glTranslatef(rocketX, rocketY, 0.0f);
    glRotatef(rocketAngle, 0.0f, 0.0f, 1.0f);   //Rotate about z-axis

    //Draw the body of the rocket with center at origin 
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);  
    glVertex2f(-10.0f, -20.0f);
    glVertex2f(10.0f, -20.0f);
    glVertex2f(10.0f, 20.0f);
    glVertex2f(-10.0f, 20.0f);
    glEnd();

    //Draw the nose of the rocket, with center at origin.
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-10.0f, 20.0f);
    glVertex2f(10.0f, 20.0f);
    glVertex2f(0.0f, 40.0f);
    glEnd();

    //Drawing the thrust flame
    if (thrust > 0.0f && fuel > 0) {    
        glColor3f(1.0f, 0.5f, 0.0f);
        glBegin(GL_TRIANGLES);
        //Triangle 1
        glVertex2f(-8.0f, -20.0f);
        glVertex2f(-4.0f, -20.0f);
        glVertex2f(-6.0f, -30.0f);
        //Triangle 2
        glVertex2f(4.0f, -20.0f);
        glVertex2f(8.0f, -20.0f);
        glVertex2f(6.0f, -30.0f);
        glEnd();
    }

    glPopMatrix();
}

// Draw platform
void drawPlatform() {
    glPushMatrix();
    glTranslatef(platformX, platformY, 0.0f);

    // draw quad centered at (0, 0)
    glColor3f(0.0f, 1.0f, 0.0f);    //Sets color to green

    glBegin(GL_QUADS);
    glVertex2f(0-platformWidth / 2, 0-platformHeight/2);    //left_bottom
    glVertex2f(0 + platformWidth / 2, 0-platformHeight/2);     //right_bottom
    glVertex2f(platformWidth / 2, platformHeight/2);          //right_top
    glVertex2f(-platformWidth / 2, platformHeight/2);         //left top
    glEnd();

    glPopMatrix();

}

// Draw ground
void drawGround() {
    glColor3f(0.5f, 0.3f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(WINDOW_WIDTH, 0.0f);     //Taking full_width and height = 30 from bottom
    glVertex2f(WINDOW_WIDTH, 30.0f);
    glVertex2f(0.0f, 30.0f);
    glEnd();
}

// Draw text
void drawText(const string &text, float x, float y) {
    //text - Text to be displayed
    //x, y defines the position / Coordinates on the screen where the text should appear.
    glColor3f(1.0f, 1.0f, 1.0f);    //Set color to white

    //This sets the starting position where the text will begin drawing. My text starts form this point
    glRasterPos2f(x, y);

    //Iterates through each character in the string.
    //For each character, it draws it using the GLUT-provided font.

    //We are using iterator to iterate over the full string
    for (auto x : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, x);   //Note = Here we are passing the iterator.
        //GLUT_BITMAP_HELVETICA_18 is the font type and size (Helvetica, 18pt).
    }
}

void drawHUD(){
    string angle = "Angle: " + to_string(rocketAngle) + " deg";
    string y_velocity = "Velocity Y: " + to_string(velocityY);
    string x_velocity = "Velocity X: " + to_string(velocityX);
    string Fuel = "Fuel: " + to_string(fuel);

    drawText(angle, 10, WINDOW_HEIGHT - 30);
    drawText(y_velocity, 10, WINDOW_HEIGHT - 60);
    drawText(x_velocity, 10, WINDOW_HEIGHT - 90);
    drawText(Fuel, 10, WINDOW_HEIGHT - 120);
    
}

void checkCollision() {
    float rocketBottom = rocketY - rocketHeight/2;
    float rocketTop = rocketY + rocketHeight/2;
    float rocketLeft = rocketX - rocketWidth / 2;
    float rocketRight = rocketX + rocketWidth / 2;

    float platformTop = platformY + platformHeight/2;
    float platformLeft = platformX - platformWidth / 2;
    float platformRight = platformX + platformWidth / 2;

    bool outOfBounds = (rocketRight > WINDOW_WIDTH || rocketLeft < 0 || rocketTop > WINDOW_HEIGHT || rocketBottom < 30);
    bool landed = (rocketBottom <= platformTop && rocketLeft>=platformLeft && rocketRight<=platformRight);
    
    if (landed) {

        isGameOver = true;

        bool verticalSpeedSafe = std::abs(velocityY) <= 2.0f;
        landedSuccessfully = (velocityX <= 2 && velocityY <= 2 && rocketAngle <= 10 && verticalSpeedSafe);

    }

    if (outOfBounds) {
        isGameOver = true;
        landedSuccessfully = false;
    }
}

//this function is called after every 16 ms, (around 60 FPS), updating the game state.
//This function updates the game state, 
void update(int value) {    
    
    if (!isGameOver) {
        platformX += platformSpeed;

        //Checking if platform has reached extremeLeft or extremeRight
        if (platformX - platformWidth/2 <= 0 || platformX + platformWidth/2 >= WINDOW_WIDTH)
            platformSpeed = -platformSpeed; 

        //This calculates the x-direction velocity and y-direction velocity based on the thrust and rocketAngle
        if (thrust > 0.0f && fuel > 0) {
            velocityY += thrust * std::cos(toRadians(rocketAngle));
            velocityX -= thrust * std::sin(toRadians(rocketAngle));
            fuel -= 0.3f;
            if (fuel < 0) fuel = 0;
        }

        //Updating the velocity due to gravity along x-axis and y-axis
        velocityY += GRAVITY;
        velocityX *= 0.98f;

        //Update the position of the rocket
        rocketX += velocityX;
        rocketY += velocityY;

        checkCollision();
    }

    if (isGameOver && landedSuccessfully && !hasPrintedSuccess) {
        //Console output
        std::cout << "✅ Landing Successful!\n";    
        //update hasPrintedSuccess to avoid repeated console output due to re-rendering
        hasPrintedSuccess = true;
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

//These keys only affect gameplay if the game is not over:
//On pressing right (->) we are rotating clockwise
//On pressing left (->) we are rotating anticlockwise, limiting angle upto only 45 degrees in both cases
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

//This function is called when the special_key_up is released 
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
        hasPrintedSuccess = false;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("2D Rocket Landing Simulator");

    init();
    glutDisplayFunc(display);

    //This says whenever user presses the special key, call this function - Special keys includes : left key, right key, up key and down key
    glutSpecialFunc(specialKeys);
    //This says call this function  when a special key is released. Here Up-Key
    glutSpecialUpFunc(specialKeysUp);
    //Call this function whenever a standard key is pressed (like letters, numbers , etc.)
    glutKeyboardFunc(keyboard);
    //a timer function that calls update() after every 16 milliseconds
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
