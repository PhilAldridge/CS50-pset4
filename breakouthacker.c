//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval circle = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel score = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // initial velocity
    double velocityX = 2.0;
    int maxX = 4;
    double initvelocityY = 1.0 + drand48();
    double velocityY = initvelocityY;

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if one is heard
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // move paddle
                double x = getX(event) - getWidth(paddle) / 2;
                setLocation(paddle, x, getY(paddle));
            }
        }
        
        
        
        // circle movement
        // move circle along x-axis
        move(circle, velocityX, 0);

        // bounce off right edge of window
        if (getX(circle) + getWidth(circle) >= WIDTH)
        {
            velocityX = -velocityX;
        }

        // bounce off left edge of window
        else if (getX(circle) <= 0)
        {
            velocityX = -velocityX;
        }
        
        // move circle along y-axis
        move(circle, 0, velocityY);

        // bounce off top edge of window
        if (getY(circle) <= 0)
        {
            velocityY = -velocityY;
        } 
        
        // die if hit the bottom edge of window
        else if (getY(circle) + getHeight(circle) >= HEIGHT) { 
            velocityY = -velocityY;
            lives--;
            move(circle, 0, -WIDTH/2);
            waitForClick();
        }
        
        
        
        
        //collision detection and rules
        GObject object = detectCollision(window, circle);
        if (object == NULL) {
        
        } else if (object == paddle) {
            velocityX += 4*((getX(circle) + getWidth(circle)/2)-(getX(paddle) + getWidth(paddle)/2))/getWidth(paddle);
            if (velocityX > maxX)
            {
                velocityX = maxX;
            } else if (velocityX < -maxX) {
                velocityX = -maxX;
            }
            velocityY = -velocityY;
        } else if (strcmp(getType(object), "GRect") == 0) {
            removeGWindow(window,object);
            points++;
            bricks--;
            updateScoreboard(window, score, points);
            //velocityY = -velocityY;
            if(velocityY<0) {
                velocityY = initvelocityY * pow(points*1.0,1./3.);
            } else {
                velocityY = -initvelocityY * pow(points*1.0,1./3.);
            }
            setSize(paddle, getWidth(paddle)*0.95, getHeight(paddle));
        }
              

        // linger before moving again
        pause(10);
    }
    
   // if won
   if(lives>0) {
        setLabel(score,"You won!!");
        setColor(score, "GREEN");
   } 
   // if lost
   else {
        setLabel(score, "GAME OVER");
        setColor(score, "RED");
   }
    
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    string colours[5] = {"MAGENTA", "BLUE", "GREEN", "YELLOW", "RED"};
    GRect rectangle[ROWS][COLS];
    int gap = 5;
    int brkwdth = (WIDTH-2*gap)/COLS - gap;
    int brkhght = HEIGHT/(7*ROWS);
    
    for (int i = 0; i<ROWS; i++)
    {   
        for (int j = 0; j<COLS; j++)
        {
            //printf("%i, %i, %i, %i\n", brkwdth, brkhght, gap + j*(brkwdth + gap), gap + i*(brkhght+gap));
            rectangle[i][j] = newGRect (gap + j*(brkwdth + gap), gap + i*(brkhght+gap), brkwdth, brkhght);
            setColor(rectangle[i][j], colours[i%5]);
            setFilled(rectangle[i][j], true);
            add(window, rectangle[i][j]);
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    int r = 10;
    GOval circle = newGOval(WIDTH/2-r, HEIGHT/2-r, 2*r, 2*r);
    setColor(circle, "BLACK");
    setFilled(circle, true);
    add(window, circle);
    return circle;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    int paddlewdth = WIDTH/5;
    int paddlehght = HEIGHT/40;
    int gap  = 20;
    GRect paddle = newGRect((WIDTH-paddlewdth)/2, HEIGHT-gap-paddlehght, paddlewdth, paddlehght);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel score = newGLabel("0");
    setFont(score, "SansSerif-36");
    add(window, score);
    setColor(score, "LIGHT_GRAY");
    double x = (WIDTH - getWidth(score)) / 2;
    double y = (HEIGHT - getHeight(score)) / 2;
    setLocation(score, x, y);
    return score;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
