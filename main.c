#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <curses.h>
#include <time.h>

void MakeBorder(int width, int height, char* borderSymbol);

int main(int arc, char** argv)
{
    // Set charset to allow for unicode characters
    // Used for block character in screen border
    setlocale(LC_ALL, "");

    // Init curses screen
    initscr();

    // Set dimensions for game
    int width = LINES-1;
    int height = COLS-1;

    // Determines if game is currently running
    // Switches off if player lost
    bool alive = true;
    
    // Seed for random num generator
    srand((long)time(NULL));

    // Turn off console Echo
    noecho();

    // Ignores cursor as input
    curs_set(0);

    // For using arrow keys
    keypad(stdscr, TRUE);                   
    nodelay(stdscr,TRUE);

    // Initial snake setup
    int head[2] = {LINES/2, COLS/2};
    int bod[500][2] = {0};
    int tl[2];

    bod[0][0] = LINES / 2;
    bod[0][1] = COLS / 2;
    int lines = LINES;
    int cols = COLS;
    // Determins up and down
    int xDir;
    // Determins left and right
    int yDir;
    // Sets initial length    
    int sLength = 5;

    // For the initial direction of the snake
    int initDir = rand() % 4 + 1;           
    switch(initDir) {
    // Face right
    case 1:
        xDir = 0;
        yDir = 1;
        break;
    // Face left
    case 2:
        xDir = 0;
        yDir = -1;
        break;
    // Face up
    case 3:
        xDir = -1;
        yDir = 0;
        break;
    // Face down
    case 4:
        xDir = 1;
        yDir = 0;
        break;
    default:
        break;
    }

    // Build the snake based on the random starting direction
    for (int i = 0; i < 5; i++)
    {
        bod[i][0] = (LINES / 2) - i * yDir;
        bod[i][1] = (COLS / 2) - i * xDir;
    }
    tl[0] = (LINES / 2) - 5 * yDir;
    tl[1] = (COLS / 2) - 5 * xDir;

    // Main loop
    while(alive)
    {
        // Clear screen
        clear();

        // Draw the border from the width, height, and symbol passed to the function
        MakeBorder(width, height, "\u2588");

        // Waits for user input, helps move snake accordingly
        int input = getch();
        mvprintw(tl[0], tl[1], " ");
        mvprintw(bod[1][0], bod[1][1], "$");
        mvprintw(bod[2][0], bod[2][1], "$");
        mvprintw(bod[3][0], bod[3][1], "$");
        mvprintw(bod[4][0], bod[4][1], "$");
        mvprintw(head[0], head[1], "S");

        //[A.C.] checks keypress to change direction, will add reverse check when ready
        switch(input){
            case KEY_UP:
                xDir = -1;
                yDir = 0;
                break;
            case KEY_DOWN: 
                xDir = 1;
                yDir = 0;
                break;
            case KEY_LEFT:
                xDir = 0;
                yDir = -1;
                break;
            case KEY_RIGHT:
                xDir = 0;
                yDir = 1;
            default:
                break;
        }

        head[0] += xDir;
        head[1] += yDir;
        tl[0] = bod[sLength-1][0];
        tl[1] = bod[sLength-1][0];     

        bod[0][0] = head[0];
        bod[0][1] = head[1];

        // Draw contents to screen
        refresh();

        // Screen refresh rate
        sleep(1);
    }

    // Cleanup
    endwin();

    return 0;
}

void MakeBorder(int width, int height, char* borderSymbol)
{
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
}

// NOTES:
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



