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
// Multiplayer boolean
bool isMultiplayer = true;
// Score variable
int score = 0;
// Score for second snake
int score2 = 0;

// Function Prototypes
// ========================================

// [N.T.] Function for making the border of the snake pit
void makeBorder(int width, int height, char *borderSymbol);
// Create start menu
void initializeStartMenu();

// [S.C.] Function to initialize the snake's direction
void initializeSnakeDirection(int *xDir, int *yDir);
// [S.C.] Function to initialize the snake's length
void initializeSnakeLength(int (*bod)[2], int sLength, int xDir, int yDir);
// [A.C.] Function to change the direction of the snake
void changeDirection(int input, int *xDir, int *yDir, bool WASD);
// [M.Q.] Snake speed function
int makeSnakeZoomZoom(int snakeLength);

// [M.Q.] Function to generate a random number between min and max (inclusive)
int getRandomNumber(int min, int max);
// [M.Q.] Function to handle trophies
void handleTrophy(int *head, int (*bod)[2], int *sLength, int sLength1, int sLength2, int currentSnake);

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

    // Start menu to select isMultiplayer or not
    initializeStartMenu();

    // [S.C] Initial snake setup
    int head[2] = {gWidth / 2, gHeight / 2};
    if (isMultiplayer)
        head[1] = gHeight / 4;
    int bod[500][2] = {0};
    int tl[2];

    // Initial snake 2 values
    int head2[2] = {gWidth / 2, gHeight * 3 / 4};
    int bod2[500][2] = {0};
    int tl2[2];

    int trophy[2];
    int trophyValue;
    int trophyExpiration;

    bod[0][0] = head[0];
    bod[0][1] = head[1];

    // Determins up and down
    int xDir;
    // Determins left and right
    int yDir;
    // Sets initial length
    int sLength = 5;
    // Value to expand snakes length by
    int sExpansion = 0;

    // Determins up and down
    int xDir2;
    // Determins left and right
    int yDir2;
    // Sets initial length
    int sLength2 = 5;
    // Value to expand snakes length by
    int sExpansion2 = 0;

    initializeSnakeDirection(&xDir, &yDir);
    initializeSnakeLength(bod, sLength, xDir, yDir);

    tl[0] = (head[0]) - sLength * xDir;
    tl[1] = (head[1]) - sLength * yDir;

    if (isMultiplayer)
    {
        bod2[0][0] = head2[0];
        bod2[0][1] = head2[1];

        // Snake 2 initial direction is same initial direction as snake 1
        xDir2 = xDir; 
        yDir2 = yDir;
        initializeSnakeLength(bod2, sLength2, xDir2, yDir2);

        tl2[0] = (head2[0]) - sLength2 * xDir2;
        tl2[1] = (head2[1]) - sLength2 * yDir2;
    }

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

        changeDirection(input, &xDir, &yDir, true);

        // [A.C.] Update head location
        head[0] += xDir;
        head[1] += yDir;
        tl[0] = bod[sLength - 1][0];
        tl[1] = bod[sLength - 1][1];

        // [A.C.] Move snake and collision check with self
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

        if (isMultiplayer)
        {
            // End condition of snake hits other snake
            for (int i = sLength2 - 1; i > 0; i--)
                if (head[0] == bod2[i][0] && head[1] == bod2[i][1])
                    gameOver();
        }

        // [A.C.] Update body position
        bod[0][0] = head[0];
        bod[0][1] = head[1];

        // [N.T.] Make temp length = to sLength
        int tmpLength = sLength;

        // Trophy collision
        handleTrophy(head, bod, &tmpLength, sLength, sLength2, 1);

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
        if (score >= (gWidth * 2 + gHeight * 2) / 2)
            victory();

        /**** SNAKE 2 ****/

        if (isMultiplayer)
        {
            // Print snake 2
            mvprintw(head2[0], head2[1], "S");
            for (int i = 1; i < sLength2; i++)
                mvprintw(bod2[i][0], bod2[i][1], "$");

            changeDirection(input, &xDir2, &yDir2, false);

            // Update snake 2 head location
            head2[0] += xDir2;
            head2[1] += yDir2;
            tl2[0] = bod2[sLength2 - 1][0];
            tl2[1] = bod2[sLength2 - 1][1];

            // Move snake 2 and collision check with self
            for (int i = sLength2 - 1; i > 0; i--)
            {
                bod2[i][0] = bod2[i - 1][0];
                bod2[i][1] = bod2[i - 1][1];

                // Snake hits self
                if (head2[0] == bod2[i][0] && head2[1] == bod2[i][1])
                {
                    gameOver();
                }
            }

            // Snake hits other snake
            for (int i = sLength - 1; i > 0; i--)
                if (head2[0] == bod[i][0] && head2[1] == bod[i][1])
                    gameOver();

            // Update snake 2 body position
            bod2[0][0] = head2[0];
            bod2[0][1] = head2[1];

            int tmpLength2 = sLength2;

            // Trophy collision for snake 2
            handleTrophy(head2, bod2, &tmpLength2, sLength, sLength2, 2);

            // Set sExpansion2 length
            if (tmpLength2 > sLength2)
                sExpansion2 = tmpLength2 - sLength2;

            // Increase sLength2 by 1 every frame until sExpansion2 = 0
            if (sExpansion2 > 0)
            {
                sLength2++;
                sExpansion2--;
            }

            /**** END-GAME CONDITIONS ****/

            // [S.C] Condition to end game if hitting boarder
            if (head2[0] == 0 || head2[0] >= gWidth || head2[1] == 0 || head2[1] >= gHeight)
                gameOver();

            // [A.C.] Condition to end game if win condition is met
            if (score + score2 >= (gWidth * 2 + gHeight * 2) / 2)
                victory();

        }

        // [A.C.] Print Score
        mvprintw(0, 0, "Score: %d", score); 
        if (isMultiplayer)
            mvprintw(0, 12, "Score 2: %d", score2); 
            
        /**** DRAW / REFRESH SCREEN ****/

        // Draw contents to screen
        refresh();

        // Screen refresh rate
        // Use fastes snake speed
        int snakeSpeed = makeSnakeZoomZoom((sLength > sLength2) ? sLength : sLength2);
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

void initializeStartMenu()
{
    // Initialize colors
    start_color();
    
    bool startMenuActive = true;
    int currentSelection = 0;
    
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    while (startMenuActive)
    {
        // Clear screen
        clear();

        // Build the window
        mvprintw(gWidth / 2 - 3, gHeight / 2 - 1, "START MENU");
        mvprintw(gWidth / 2 - 2, gHeight / 2 - 10, "(USE ARROW KEYS TO NAVIGATE)");
        mvprintw(gWidth / 2 - 1, gHeight / 2 - 7, "(PRESS ENTER TO SELECT)");

        if (currentSelection == 1)
            attron(COLOR_PAIR(1));
        
        mvprintw(gWidth / 2 + 1, gHeight / 2 - 3, "* SINGLE-PLAYER");
        
        if (currentSelection == 1)
            attroff(COLOR_PAIR(1));

        if (currentSelection == 2)
            attron(COLOR_PAIR(1));
        mvprintw(gWidth / 2 + 2, gHeight / 2 - 3, "* MULTI-PLAYER");

        if (currentSelection == 2)
            attroff(COLOR_PAIR(1));

        int input = getch();

        switch (input)
        {
        case KEY_UP:
            currentSelection++;
            // [N.T.] Wrap currentSelection to upper bound of 2
            if (currentSelection > 2)
                currentSelection = 1;
            break;
        case KEY_DOWN:
            currentSelection--;
            // [N.T.] Wrap currentSelection to lower bound of 1
            if (currentSelection < 1)
                currentSelection = 2;
            break;
        case KEY_ENTER:
        case '\r':
        case '\n':
            startMenuActive = false;
            break;
        }

        // Select multiplayer status
        if (currentSelection == 2)
            isMultiplayer = true;
        else
            isMultiplayer = false;

        // Refresh screen
        refresh();
        usleep(10000);
    }
}

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
        bod[i][0] = bod[0][0] - i * xDir;
        bod[i][1] = bod[0][1] - i * yDir;
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
void changeDirection(int input, int *xDir, int *yDir, bool WASD)
{
    if (isMultiplayer)
    {
        if (WASD)
        {
            switch (input)
            {
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
        }
        else 
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
                default:
                    break;
            }
        }
    }
    else 
    {
        switch (input)
        {
            case KEY_UP:
            case 'w':
                *xDir = -1;
                *yDir = 0;
                break;
            case KEY_DOWN:
            case 's':
                *xDir = 1;
                *yDir = 0;
                break;
            case KEY_LEFT:
            case 'a':
                *xDir = 0;
                *yDir = -1;
                break;
            case KEY_RIGHT:
            case 'd':
                *xDir = 0;
                *yDir = 1;
                break;
            default:
                break;
        }
    }
} // changeDirection

// [007]
int makeSnakeZoomZoom(int snakeLength) {
    int baseSleepTime = 300000;
    int lengthMultiplier = 2000;
    return baseSleepTime - snakeLength * lengthMultiplier;
}


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

void handleTrophy(int *head, int (*bod)[2], int *sLength, int sLength1, int sLength2, int currentSnake)
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
        
        if (currentSnake == 2 && isMultiplayer == true)
            score2 += trophyValue;
        else 
            score += trophyValue;
    }
    // [009]
    else if (time(NULL) >= trophyExpirationTime)
    {
        // [012]
        // What if the trophy is too far from the snake regarding the snake’s speed?
        // What about the random locations on the border of your screen?
        while (true)
        {
            trophyPosition[0] = getRandomNumber(1, gWidth - 2);
            trophyPosition[1] = getRandomNumber(1, gHeight - 2);

            /*** Check if trophy is located in snake 1 ***/
            for (int i = 0; i < sLength1 - 1; i++)
                if (bod[i][0] == trophyPosition[0] && bod[i][1] == trophyPosition[1])
                    trophyPosition[0] = -1;

            /*** Check if trophy is located in snake 2 ***/ 
            for (int i = 0; i < sLength2 - 1; i++)
                if (bod[i][0] == trophyPosition[0] && bod[i][1] == trophyPosition[1])
                    trophyPosition[0] = -1;

            // Restart loop if trophy is inside the snake to reset the trophy position
            if (trophyPosition[0] == -1)
                continue;

            break;
        }
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