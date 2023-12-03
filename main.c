#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>
#include <curses.h>

// Struct containing all of the border information
struct border
{
    // Dimensions
    int rows;
    int columns;

    // Symbol for border
    char* borderSymbol;
};

int main(int arc, char** argv)
{
    // Set charset to allow for unicode characters
    setlocale(LC_ALL, "");

    // Init curses screen
    initscr();
    
    // Init curses text color
    start_color();

    // Instantiate border struct
    struct border b;


    // Set dimensions for 
    b.rows = LINES-1;
    b.columns = COLS-1;

    // Set the border symbol
    b.borderSymbol = "\u2588";

    // Set color pairs
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    init_pair(3, COLOR_WHITE, COLOR_WHITE);

    // Main loop
    while(1)
    {
        // Clear screen
        clear();

        // Draw loop
        for (int height = 0; height <= b.rows; height++)
        {
            // Draw left side of the box
            move(height, 0);

            attron(COLOR_PAIR((height % 2 == 0) ? 1 : 2));
            addstr(b.borderSymbol);
            attroff(COLOR_PAIR((height % 2 == 0) ? 1 : 2));
            
            // Draw the right side of the box
            move(height, b.columns);
            attron(COLOR_PAIR((height % 2 == 0) ? 2 : 1));
            addstr(b.borderSymbol);
            attroff(COLOR_PAIR((height % 2 == 0) ? 2 : 1));

            for (int width = 0; width <= b.columns; width++)
            {
                // GROUP NOTES: DRAW STUFF HERE

                // Draw top and bottom of border box
                if (height == 0 || height == b.rows)
                {
                    move(height, width);
                    attron(COLOR_PAIR((height == b.rows) ? ((width % 2 == 0) ? 2 : 1) : ((width % 2 == 0) ? 1 : 2)));
                    addstr(b.borderSymbol);
                    attroff(COLOR_PAIR((height == b.rows) ? ((width % 2 == 0) ? 2 : 1) : ((width % 2 == 0) ? 1 : 2)));
                }
                // Else draw background
                else if (width != 0 && width != b.columns)
                {
                    move(height, width);
                    attron(COLOR_PAIR(3));
                    addstr(b.borderSymbol);
                    attroff(COLOR_PAIR(3));
                }
            }
        }

        // Draw contents to screen
        refresh();

        // Screen refresh rate
        sleep(1);
    }

    // Cleanup
    endwin();

    return 0;
}