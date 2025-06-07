#include <iostream>
#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <cstdio>
#include <Windows.h>
#include <mmsystem.h>
#include <cmath> // For math functions like cosf and sinf
#include <fstream> // For file I/O

using namespace std;

// Bird position
GLdouble px = 100, py = 115;
// Number of obstacles
const int numObstacles = 8; // Adjusted to match the number of obstacles

// Define arrays to store the positions of multiple obstacles
GLdouble obstaclesX[numObstacles] = {440, 540, 640, 840, 940, 1040, 1140, 1240}; // Initial positions of the obstacles
GLdouble obstaclesY[numObstacles]; // Will be randomized for each obstacle
GLdouble d = 0;
GLdouble ps = 10;
GLdouble score = 0; // initialize score
GLdouble highScore = 0; // initialize high score

bool gameOver = false; // Flag to check game over state           // Initial speed (in milliseconds)
bool isFullScreen = false;    // Track whether the game is in full screen mode


//New state for the game
enum GameState { OVERVIEW, INGAME };
GameState currentState = OVERVIEW; // Start in the overview screen

//New state for the game
enum SpeedState { SLOW, MEDIUM, FAST, FASTFAST };
SpeedState currentSpeedState = SLOW; // Start in the SLOW SPEED
static GLdouble currentSpeed = 1.2; // Starting obstacles speed

static GLdouble gravitySpeed = 0.15; // Starting gravity speed

int obstacleGap = 300; // Adjust this value to change the gap size

//int minObstacleSpacing = 100; // Minimum horizontal distance between obstacles


void myInit() {
    glClearColor(0.5, 1.0, 1.0, 0);
    glPointSize(ps);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 640, 480, 0);
}

// Function to draw a circle
void drawCircle(int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int p = 1 - r;

    glBegin(GL_POINTS);  // Use GL_POINTS to draw individual points
    while (x <= y) {
        // Draw the 8 symmetric points of the circle
        glVertex2i(xc + x, yc + y);   // 1st octant
        glVertex2i(xc - x, yc + y);   // 2nd octant
        glVertex2i(xc + x, yc - y);   // 4th octant
        glVertex2i(xc - x, yc - y);   // 3rd octant
        glVertex2i(xc + y, yc + x);   // 5th octant
        glVertex2i(xc - y, yc + x);   // 6th octant
        glVertex2i(xc + y, yc - x);   // 7th octant
        glVertex2i(xc - y, yc - x);   // 8th octant
        x++;
        if (p < 0) {
            p += 2 * x + 1;  // Move x to the right
        } else {
            y--;             // Move y down
            p += 2 * (x - y) + 1;
        }
    }
    glEnd();
}


// Function to draw the bird
void drawBird() {
    // Draw body
    glColor3f(1.0, 0.5, 0.0); // Orange body
    drawCircle(px, py, 10);   // Call Midpoint Circle Algorithm for the body

    // Draw eye
    glColor3f(0.0, 0.0, 0.0); // Black eye
    drawCircle(px + 4, py - 4, 0);  // Call Midpoint Circle Algorithm for the eye

    // Draw wing
    glColor3f(1.0, 1.0, 1.0); // White wing
    glBegin(GL_TRIANGLES);
    glVertex2f(px, py);
    glVertex2f(px - 15, py - 10);
    glVertex2f(px - 15, py + 10);
    glEnd();
}




// Function to draw obstacles
void drawObstacles() {
    glColor3f(1.0, 0.5, 0.0); // Orange color for obstacles
    int obstacleWidth = 40; // Fixed width for obstacles

    for (int i = 0; i < numObstacles; i++) {
            if(i % 2 == 0) {
                continue;
            } else {
                // Ensure obstacle positions are valid
                if (obstaclesY[i] <= 0 || obstaclesY[i] >= 380) {
                    continue; // Skip drawing invalid obstacles
                }

                // Upper obstacle (spans downward from the top of the screen)
                glBegin(GL_QUADS);
                glVertex2i(obstaclesX[i], 0);                     // Top-left
                glVertex2i(obstaclesX[i] + obstacleWidth, 0);     // Top-right
                glVertex2i(obstaclesX[i] + obstacleWidth, obstaclesY[i]); // Bottom-right
                glVertex2i(obstaclesX[i], obstaclesY[i]);         // Bottom-left
                glEnd();

                // Lower obstacle (spans upward from the bottom of the gap)
                glBegin(GL_QUADS);
                glVertex2i(obstaclesX[i], obstaclesY[i] + obstacleGap); // Top-left of the lower obstacle
                glVertex2i(obstaclesX[i] + obstacleWidth, obstaclesY[i] + obstacleGap); // Top-right
                glVertex2i(obstaclesX[i] + obstacleWidth, 480);     // Bottom-right
                glVertex2i(obstaclesX[i], 480);                    // Bottom-left
                glEnd();
            }
    }
}



// Function to play sound
void playSound(const char* soundFile) {
    PlaySound(soundFile, NULL, SND_FILENAME | SND_ASYNC);
}

// Function to check if the bird collides with any obstacle
bool checkCollision() {
    // Bird's bounding box
    int birdLeft = px - 10, birdRight = px + 10;
    int birdTop = py - 10, birdBottom = py + 10;

    for (int i = 0; i < numObstacles; i++) {
            if(i % 2 == 0) {
                continue;
            } else {
                // Obstacle's bounding box
                int obstacleLeft = obstaclesX[i];
                int obstacleRight = obstaclesX[i] + 40; // Fixed obstacle width
                int obstacleTop = obstaclesY[i];
                int obstacleBottom = obstacleTop + obstacleGap;

                // Check horizontal overlap
                bool horizontalOverlap = (birdRight >= obstacleLeft) && (birdLeft <= obstacleRight);

                // Check vertical overlap with either upper or lower part of the obstacle
                if (horizontalOverlap) {
                    // Upper obstacle collision
                    if (birdTop <= obstacleTop) {
                        playSound("hit.wave");
                        return true; // Collision with upper part
                    }

                     // Lower obstacle collision
                    if (birdBottom >= obstacleBottom) {
                        playSound("hit.wave");
                        return true; // Collision with lower part
                    }
                }
            }


    }

    return false; // No collision detected
}

// debugging
void drawDebugging() {
    // Draw bird's bounding box
    glColor3f(gameOver ? 1.0 : 0.0, 0.0, 1.0); // Red if game over, blue otherwise
    glBegin(GL_LINE_LOOP);
    glVertex2i(px - 10, py - 10);
    glVertex2i(px + 10, py - 10);
    glVertex2i(px + 10, py + 10);
    glVertex2i(px - 10, py + 10);
    glEnd();

    // Draw obstacles' bounding boxes
    for (int i = 0; i < numObstacles; i++) {
        // Upper obstacle
        glColor3f(1.0, 0.0, 0.0); // Red for upper obstacle
        glBegin(GL_LINE_LOOP);
        glVertex2i(obstaclesX[i], 0);
        glVertex2i(obstaclesX[i] + 40, 0);
        glVertex2i(obstaclesX[i] + 40, obstaclesY[i]);
        glVertex2i(obstaclesX[i], obstaclesY[i]);
        glEnd();

        // Lower obstacle
        glColor3f(0.0, 1.0, 0.0); // Green for lower obstacle
        glBegin(GL_LINE_LOOP);
        glVertex2i(obstaclesX[i], obstaclesY[i] + 100);
        glVertex2i(obstaclesX[i] + 40, obstaclesY[i] + 100);
        glVertex2i(obstaclesX[i] + 40, 480);
        glVertex2i(obstaclesX[i], 480);
        glEnd();
    }
}



// Function to write high score to a binary file
void writeHighScore() {
    std::ofstream outfile("C:\\Users\\mf\\Desktop\\Graphics Project\\flappy_Bird_Gr4\\bin\\score.bin", std::ios::binary);
    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<const char*>(&highScore), sizeof(highScore));
        outfile.close();
    } else {
        cerr << "Error: Could not open file to save high score!" << endl;
    }
}

// Function to read high score from a binary file
void readHighScore() {
    std::ifstream infile("C:\\Users\\mf\\Desktop\\Graphics Project\\flappy_Bird_Gr4\\bin\\score.bin", std::ios::binary);
    if (infile.is_open()) {
        infile.read(reinterpret_cast<char*>(&highScore), sizeof(highScore));
        infile.close();
    } else {
        // If the file does not exist, initialize high score to 0
        highScore = 0;
    }
}


// Function to check if the current score is a new high score
void updateHighScore() {
    if (score > highScore) {
        highScore = score;  // Update high score
        writeHighScore();   // Save the new high score to the file
    }
}


// Idle function to move objects automatically
void myIdle() {
    // If not in INGAME state, do nothing
    if (currentState != INGAME) {
        glutPostRedisplay();
        return;
    }
    playSound("bgsound.wave");
    if (gameOver) return; // If the game is over, do nothing

    if(d == 0) {
        py += gravitySpeed; // Apply the vertical speed to the bird (gravity effect)
    }

    if(d == 1) {
        py -= currentSpeed;
    }

    if (py + 10 > 480 - 20) {  // Prevent the bird from going off-screen (bottom)
        playSound("hit");
        py  = 480 - 30;       // Limit the bird's position to the screen bottom
        gameOver = true;      // Game over if the bird hits the ground
        updateHighScore();
    }

    if (px <= 320) {
        px += gravitySpeed;   // Move to right until it reaches the middle
    }

    // Move obstacles and reset when they move off the screen
    for (int i = 0; i < numObstacles; i++) {
        obstaclesX[i] -= currentSpeed;    // Move each obstacle to the left

        // Check if the bird has successfully passed the obstacle
        if (obstaclesX[i] + 40 < px && obstaclesX[i] + 40 >= px - currentSpeed) {
            score++;      // Increment score
        }

        // manage obstacles gap depending on score to made game harder
        if(score > 50){
            obstacleGap = 200;
        }
        if(score > 80){
            obstacleGap = 120;
        }
        if(score > 100){
            obstacleGap = 80;
        }



        if (obstaclesX[i] + 40 <= 0) {
            obstaclesX[i] = 840;  // Reset to the right side of the screen
            obstaclesY[i] = rand() % 200;  // Randomize vertical position for this obstacle
        }
    }

    // Check for collisions with obstacles
    if (checkCollision()) {
        gameOver = true; // Set game over flag if collision occurs
        updateHighScore();
    }

    glutPostRedisplay();
}


// Restart
void restartGame() {
     playSound("bgsound.wav");
    // Reset bird position
    px = 100;
    py = 115;
    static GLdouble currentSpeed = 0.05;

    // Reset obstacles positions
    for (int i = 0; i < numObstacles; i++) {
        obstaclesX[i] = 640 + i * 100; // Spread obstacles horizontally
        obstaclesY[i] = rand() % 200; // Randomize vertical positions
    }

    // Reset score and game state
    score = 0;
    gameOver = false;
    currentState = INGAME; // Ensure the game is in the "in-game" state
    currentSpeed = 0.2; // slow
    obstacleGap = 300;
}



// screen toggle function
void toggleFullScreen() {
    if (isFullScreen) {
        glutReshapeWindow(640, 480); // Set windowed mode size
        glutPositionWindow(100, 100); // Optionally set the window's position
        isFullScreen = false;
    } else {
        glutFullScreen(); // Set full screen mode
        isFullScreen = true;
    }
}

// Handle keyboard input
void NormalKey(unsigned char key, int x, int y) {
    switch (key) {
         case '1':
            if(currentSpeedState == FAST || currentSpeedState == MEDIUM || currentSpeedState == FASTFAST) {
                currentSpeedState = SLOW;
                currentSpeed = 0.5;
                gravitySpeed = 0.15;
            }
             break;
        case '2':
            if(currentSpeedState == FAST || currentSpeedState == SLOW || currentSpeedState == FASTFAST) {
                currentSpeedState = MEDIUM;
                currentSpeed = 0.8;
                gravitySpeed = 0.2;
            }
             break;

        case '3':
            if(currentSpeedState == MEDIUM || currentSpeedState == SLOW || currentSpeedState == FASTFAST) {
                currentSpeedState = FAST;
                currentSpeed = 1.0;
                gravitySpeed = 0.2;
            }
             break;
         case '4':
            if(currentSpeedState == MEDIUM || currentSpeedState == SLOW || currentSpeedState == FAST) {
                currentSpeedState = FASTFAST;
                currentSpeed = 1.2;
                gravitySpeed = 0.2;
            }
             break;

        case 'a': case 'A': d == 1;  break;

        case 'p': case 'P': glutIdleFunc(NULL);  break;

        case 'c': case 'C': glutIdleFunc(myIdle); break;

        case 27: // ESC to exit
            if (currentState == OVERVIEW) {
                exit(0);
            } else {
                currentState = OVERVIEW;  // Go back to the overview screen
                gameOver = false;         // Reset game over state
            }
            break;
        case 13: // ENTER to start the game
            if (currentState == OVERVIEW) {
                currentState = INGAME; // Start the game
            }
            break;
        case 'r': case 'R':
            if (gameOver || currentState == INGAME) {
                restartGame(); // Restart the game
            }
            break;
        case 'f': case 'F': toggleFullScreen();  break; // Toggle between full screen and windowed
    }
    glutPostRedisplay();
}


void mySpecial(int key, int x, int y){
    switch(key){
        case GLUT_KEY_LEFT:
            px -= 5;
            //d = 1;
            break;
        case GLUT_KEY_RIGHT:
            px += 5;
            //d = 0;
            break;
        case GLUT_KEY_UP:
            py -= 20;
            //d = 1;
            break;
        case GLUT_KEY_DOWN:
            py += 15;
            d = 0;
            break;
    }
    glutPostRedisplay();
}

// Display characters
void displayChar(void* font, const char* str, int x, int y) {
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1.0, 0.0, 0.0); // Text color
    glRasterPos2i(x, y);
    for (size_t i = 0; i < strlen(str); i++) {
        glutBitmapCharacter(font, str[i]);
    }
    glPopAttrib();
}


// Draw the overview screen
void drawOverviewScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 0.5, 0.0); // Orange color for the ground
    glRecti(0, 460, 640, 480);
    glRecti(0, 0, 640, 20);
    glRecti(0, 0, 20, 480);
    glRecti(620, 0, 640, 480);
    // border for it
    glColor3f(0.0, 0.0, 0.0); // Border color (black)
    glLineWidth(10);
    glBegin(GL_LINE_LOOP);
    glVertex2i(50, 50);
    glVertex2i(600, 50);
    glVertex2i(600, 470);
    glVertex2i(50, 470);
    glEnd();

    glColor3f(1.0, 1.0, 0.0); // Yellow color for the text
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "FLAPPY BIRD GAME(BY Group-4)", 100, 100);
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------", 80, 120);
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "=> Use Arrow Keys to control the birds", 150, 150);
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "=> Press ENTER to Start", 150, 200);
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "=> Press F to make full screen", 150, 250);
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "=> Press P to pause (after start a game)", 150, 300);
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "=> Press C to resume (after start a game)", 150, 350);
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "=> Press R to restart (after start a game)", 150, 400);
    displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "=> Press ESC to Exit", 150, 450);

    glutSwapBuffers();
}



// Display function
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (currentState == OVERVIEW) {
        drawOverviewScreen();  // Draw the overview screen
    } else if (currentState == INGAME) {
        playSound("bird");
        glColor3f(1.0, 0.5, 0.0); // Orange color for the ground
        glRecti(0, 460, 640, 480);
        glRecti(0, 0, 640, 20);

        // debugger
        //drawDebugging();

        drawBird();       // Draw bird
        drawObstacles();  // Draw obstacle

        char highScoreStr[20]; // Display high score
        sprintf(highScoreStr, "High Score: %d", static_cast<int>(highScore));
        glColor3f(0.0, 0.0, 0.0);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_24, highScoreStr, 50, 18);

        char scoreStr[15]; // Display score
        sprintf(scoreStr, "Score: %d", static_cast<int>(score));
        glColor3f(0.0, 0.0, 0.0);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr, 500, 18);
        // page description
        displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "FLAPPY BIRD Game By Group-4", 200, 480);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, "press P to pause", 50, 450);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, "press C to continue", 100, 450);

        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, "THE GAP IS NARROW WHEN THE SCORE IS MORE", 450, 50);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, ". when score > 50 gap = 200", 510, 70);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, ". when score > 80 gap = 120", 510, 90);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, ". when score > 100 gap = 80", 510, 110);



        // SPEED SELECTOR
        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, "Press 1 to SLOW SPPED", 50, 50);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, "Press 2 to MEDIUM SPPED", 50, 80);
        displayChar(GLUT_BITMAP_TIMES_ROMAN_10, "Press 3 to FAST SPPED", 50, 110);
        // Display "Game Over" message if the game is over
        if (gameOver) {
            char gameOverStr[] = "GAME OVER!";
            glColor3f(1.0, 0.0, 0.0);
            displayChar(GLUT_BITMAP_TIMES_ROMAN_24, gameOverStr, 50, 240);
            displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "Press R to restart", 50, 270);
            displayChar(GLUT_BITMAP_TIMES_ROMAN_24, "OR ESC to exit", 50, 300);
        }
    }
    glutSwapBuffers();
    glFlush();
}


int main(int argc, char *argv[]) {
     playSound("bgsound.wav");
    // Randomize the vertical positions of obstacles initially
    for (int i = 0; i < numObstacles; i++) {
        obstaclesY[i] = rand() % 200;
    }

    readHighScore(); // Read the high score from file
    //writeHighScore();
    glutInit(&argc, argv);
    glutInitWindowSize(800, 640);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow("Moving Bird Game");
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(NormalKey);
    glutSpecialFunc(mySpecial);
    glutIdleFunc(myIdle);
    myInit();
    glutMainLoop();

    return 0;
}
