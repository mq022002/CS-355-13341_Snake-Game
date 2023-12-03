// Imports
// ========================================

#include <curses.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Global Variables
// ========================================

// Determines if game is currently running
// Switches off if player lost
bool alive = true;
// Set width for the game
int gWidth;
// Set height for the game
int gHeight;

// Function Prototypes
// ========================================

// [N.T.] Function for making the border of the snake pit
void makeBorder(int width, int height, char *borderSymbol);

// [S.C.] Function to initialize the snake's direction
void initializeSnakeDirection(int *xDir, int *yDir);
// [S.C.] Function to initialize the snake's length
void initializeSnakeLength(int (*bod)[2], int sLength, int xDir, int yDir);
// [A.C.] Function to change the direction of the snake
void changeDirection(int input, int *xDir, int *yDir);
// [M.Q.] Snake speed function
int makeSnakeZoomZoom(int snakeLength);

// [M.Q.] Function to generate a random number between min and max (inclusive)
int getRandomNumber(int min, int max);
// [M.Q.] Function to handle trophies
void handleTrophy(int *head, int (*bod)[2], int *sLength);

// [S.C.] Function to end the game when the player loses
void gameOver();

// [A.C.] Function to end the game when the player wins
void victory();

// Main Method
// ========================================

int main(int arc, char **argv)
{
    // [N.T.] Set charset to allow for unicode characters
    // Used for block character in screen border
    setlocale(LC_ALL, "");

    // Init curses screen
    initscr();

    // Initialize global variables
    gWidth = LINES - 1;
    gHeight = COLS - 1;

    // Seed for random num generator
    srand((long)time(NULL));

    // Turn off console Echo
    noecho();

    // Ignores cursor as input
    curs_set(0);

    // [S.C.] For using arrow keys
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    // [S.C] Initial snake setup
    int head[2] = {gWidth / 2, gHeight / 2};
    int bod[500][2] = {0};
    int tl[2];

    int trophy[2];
    int trophyValue;
    int trophyExpiration;

    bod[0][0] = gWidth / 2;
    bod[0][1] = gHeight / 2;
    int lines = gWidth;
    int cols = gHeight;
    // Determins up and down
    int xDir;
    // Determins left and right
    int yDir;
    // Sets initial length
    int sLength = 90;
    // Value to expand snakes length by
    int sExpansion = 0;

    initializeSnakeDirection(&xDir, &yDir);
    initializeSnakeLength(bod, sLength, xDir, yDir);

    tl[0] = (gWidth / 2) - sLength * xDir;
    tl[1] = (gHeight / 2) - sLength * yDir;

    // Main loop
    while (alive)
    {
        // [A.C.] Waits for user input, helps move snake accordingly
        int input = getch();

        // Clear screen
        clear();

        // [N.T.] Draw the border from the width, height, and symbol passed to the
        // function
        makeBorder(gWidth, gHeight, "\u2588");

        // [S.C.] Print snake parts
        mvprintw(head[0], head[1], "S");
        for (int i = 1; i < sLength; i++)
            mvprintw(bod[i][0], bod[i][1], "$");
        mvprintw(tl[0], tl[1], "X");

        changeDirection(input, &xDir, &yDir);

        // [A.C.] Update head location
        head[0] += xDir;
        head[1] += yDir;
        tl[0] = bod[sLength - 1][0];
        tl[1] = bod[sLength - 1][1];

        // [A.C.] Add more later for other conditions
        for (int i = sLength - 1; i > 0; i--)
        {
            bod[i][0] = bod[i - 1][0];
            bod[i][1] = bod[i - 1][1];

            // [S.C] End condition if head hits a body part
            if (head[0] == bod[i][0] && head[1] == bod[i][1])
            {
                gameOver();
            }
        }

        // [A.C.] Update body position
        bod[0][0] = head[0];
        bod[0][1] = head[1];

        // [N.T.] Make temp length = to sLength
        int tmpLength = sLength;

        //
        handleTrophy(head, bod, &tmpLength);

        // [N.T.] Set sExpansion length
        if (tmpLength > sLength)
            sExpansion = tmpLength - sLength;

        // [N.T.] Increase sLength by 1 every frame until sExpansion = 0
        if (sExpansion > 0)
        {
            sLength++;
            sExpansion--;
        }

        /**** END-GAME CONDITIONS ****/

        // [S.C] Condition to end game if hitting boarder
        if (head[0] == 0 || head[0] >= gWidth || head[1] == 0 || head[1] >= gHeight)
            gameOver();

        // [A.C.] Condition to end game if win condition is met
        if (sLength == (gWidth * 2 + gHeight * 2) / 2)
            victory();

        /**** DRAW / REFRESH SCREEN ****/

        // Draw contents to screen
        refresh();

        // Screen refresh rate
        int snakeSpeed = makeSnakeZoomZoom(sLength);
        usleep(snakeSpeed);
    }

    // Cleanup
    endwin();

    return 0;
}

// The Snake Pit
// [N.T.]   [001] The snake pit is the area where the snake can move.
// [N.T.]   [002] The snake pit must utilize all available space of the current
// terminal window. [N.T.]   [003] The snake pit must have a visible border
// delineating the snake pit.
// ========================================

// [001]
// [002]
// [003]
void makeBorder(int width, int height, char *borderSymbol)
{
    // [N.T.] Draw loop for border
    for (int row = 0; row <= width; row++)
    {
        // Draw left side of the box
        move(row, 0);
        addstr(borderSymbol);

        // Draw the right side of the box
        move(row, height);
        addstr(borderSymbol);

        for (int column = 0; column <= height; column++)
        {
            // GROUP NOTES: DRAW STUFF HERE

            // Draw top and bottom of border box
            if (row == 0 || row == width)
            {
                // Set cursor to current row and column
                move(row, column);
                // Print border symbol
                addstr(borderSymbol);
            }
        }
    }
} // makeBorder

// The Snake
// [S.C.]   [004] The inital length of the snake is 5 characters.
// [S.C.]   [005] Initial direction of the snake's movement is chosen randomly.
// [A.C.]   [006] The user can press either one of the four arrow keys or WASD
// to change the direction of the snake's movement. [M.Q.]   [007] The snake's
// speed is proportional to its length.
// ========================================

// [004]
void initializeSnakeLength(int (*bod)[2], int sLength, int xDir, int yDir)
{
    for (int i = 0; i < sLength; i++)
    {
        bod[i][0] = (gWidth / 2) - i * xDir;
        bod[i][1] = (gHeight / 2) - i * yDir;
    }
} // initializeSnakeLength

// [005]
void initializeSnakeDirection(int *xDir, int *yDir)
{
    int initDir = getRandomNumber(1, 4);
    switch (initDir)
    {
    // Face right
    case 1:
        *xDir = 0;
        *yDir = 1;
        break;
    // Face left
    case 2:
        *xDir = 0;
        *yDir = -1;
        break;
    // Face up
    case 3:
        *xDir = -1;
        *yDir = 0;
        break;
    // Face down
    case 4:
        *xDir = 1;
        *yDir = 0;
        break;
    default:
        break;
    }
} // initializeSnakeDirection

// [006]
void changeDirection(int input, int *xDir, int *yDir)
{
    switch (input)
    {
    case KEY_UP:
        *xDir = -1;
        *yDir = 0;
        break;
    case KEY_DOWN:
        *xDir = 1;
        *yDir = 0;
        break;
    case KEY_LEFT:
        *xDir = 0;
        *yDir = -1;
        break;
    case KEY_RIGHT:
        *xDir = 0;
        *yDir = 1;
        break;
    case 'w':
        *xDir = -1;
        *yDir = 0;
        break;
    case 's':
        *xDir = 1;
        *yDir = 0;
        break;
    case 'a':
        *xDir = 0;
        *yDir = -1;
        break;
    case 'd':
        *xDir = 0;
        *yDir = 1;
        break;
    default:
        break;
    }
} // changeDirection

// [007]
int makeSnakeZoomZoom(int snakeLength) { return 200000 - snakeLength * 5000; }

// The Trophies
// [M.Q.]   [008] Trophies are represented by a digit randomly chosen from 1
// to 9. [M.Q.]   [009] There's always exactly one trophy in the snakepit at any
// given moment. [N.T.]   [010] When the snake eats the trophy, its length is
// increased by the corresponding number of characters. [M.Q.]   [011] A trophy
// expires after a random interval from 1 to 9 seconds [M.Q.]   [012] A new
// trophy is shown at a random location on the screen after the previous one has
// either expired or is eated by the snake.
// ========================================

int getRandomNumber(int min, int max)
{
    return rand() % (max - min + 1) + min;
} // getRandomNumber

void handleTrophy(int *head, int (*bod)[2], int *sLength)
{
    static int trophyPosition[2] = {-1, -1};
    static int trophyValue = 0;
    static time_t trophyExpirationTime = 0;

    if (head[0] == trophyPosition[0] && head[1] == trophyPosition[1])
    {
        // [010]
        *sLength += trophyValue;
        trophyPosition[0] = -1;
        trophyPosition[1] = -1;
        trophyExpirationTime = 0;
    }
    // [009]
    else if (time(NULL) >= trophyExpirationTime)
    {
        // [012]
        // What if the trophy is too far from the snake regarding the snake’s speed?
        // What about the random locations on the border of your screen?
        do
        {
            trophyPosition[0] = getRandomNumber(1, gWidth - 2);
            trophyPosition[1] = getRandomNumber(1, gHeight - 2);
        } while (abs(head[0] - trophyPosition[0]) +
                     abs(head[1] - trophyPosition[1]) >
                 *sLength);

        // [008]
        trophyValue = getRandomNumber(1, 9);
        // [011]
        trophyExpirationTime = time(NULL) + getRandomNumber(1, 9);
    }

    // How did you manage displaying and changing the location of trophy?
    mvprintw(trophyPosition[0], trophyPosition[1], "%d", trophyValue);
} // handleTrophy

// The Gameplay
// [S.C.]   [013] The snake dies and the game ends if:
//              It runs into the border; or
//              It runs into itself; or
//              The user attempts to reverse the snake's direction.
// [A.C.]   [014] The user wins the game if the snake's length grows to the
// length equal to half the perimeter of the border.
// ========================================

// [013]
void gameOver()
{
    clear();
    refresh();
    char arr[18] = {"Game Over You Lose"};

    for (int i = 0; i <= 17; i++)
    {
        mvaddch(gWidth / 2, ((gHeight / 2) - 17) + i, arr[i]);
        refresh();
    }
    sleep(5);
    alive = false;
} // gameOver

// [014]
void victory()
{
    clear();
    refresh();
    char arr[8] = {"You win!"};

    for (int i = 0; i <= 7; i++)
    {
        mvaddch(gWidth / 2, ((gHeight / 2) - 7) + i, arr[i]);
        refresh();
    }
    sleep(5);
    alive = false;
} // victory

// Misc Stuff
// ========================================

// GROUP NOTES:
// COLORS EXAMPLE:

/*
    // Init curses text color
    // start_color();

    //Set color pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_WHITE);

    // Draw left side of the box
    move(row, 0);
    attron(COLOR_PAIR((row % 2 == 0) ? 1 : 2));
    addstr(b.borderSymbol);
    attroff(COLOR_PAIR((row % 2 == 0) ? 1 : 2));
*/