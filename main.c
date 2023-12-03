#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// [N.T.] Function for making the border of the snake pit
void MakeBorder(int width, int height, char *borderSymbol);

// Function to generate a random number between min and max (inclusive)
int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// [MQ] Function to handle trophies
void HandleTrophy(int* head, int (*bod)[2], int* sLength) {
    static int trophyPosition[2] = {-1, -1};
    static int trophyValue = 0;
    static time_t trophyExpirationTime = 0;

    // Check if the trophy is eaten or expired
    if (head[0] == trophyPosition[0] && head[1] == trophyPosition[1]) {
        *sLength += trophyValue;
        trophyPosition[0] = -1;
        trophyPosition[1] = -1;
        trophyExpirationTime = 0;
    } else if (time(NULL) >= trophyExpirationTime) {
        // Generate a new trophy if the previous one expired
        trophyPosition[0] = getRandomNumber(1, LINES - 2);
        trophyPosition[1] = getRandomNumber(1, COLS - 2);
        trophyValue = getRandomNumber(1, 9);
        trophyExpirationTime = time(NULL) + getRandomNumber(1, 9);
    }

    // Draw the trophy on the screen
    mvprintw(trophyPosition[0], trophyPosition[1], "%d", trophyValue);
}

int main(int arc, char **argv) {
  // [N.T.] Set charset to allow for unicode characters
  // Used for block character in screen border
  setlocale(LC_ALL, "");

  // Init curses screen
  initscr();

  // Set dimensions for game
  int width = LINES - 1;
  int height = COLS - 1;

  // Determines if game is currently running
  // Switches off if player lost
  bool alive = true;

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
  int head[2] = {LINES / 2, COLS / 2};
  int bod[500][2] = {0};
  int tl[2];

  int trophy[2];
  int trophyValue;
  int trophyExpiration;

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

  // [S.C.] For the initial direction of the snake
  int initDir = rand() % 4 + 1;
  switch (initDir) {
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

  // [S.C.] Build the snake based on the random starting direction
  for (int i = 0; i < 5; i++) {
    bod[i][0] = (LINES / 2) - i * yDir;
    bod[i][1] = (COLS / 2) - i * xDir;
  }
  tl[0] = (LINES / 2) - 5 * yDir;
  tl[1] = (COLS / 2) - 5 * xDir;

  // Main loop
  while (alive) {
    // [A.C.] Waits for user input, helps move snake accordingly
    int input = getch();

    // Clear screen
    clear();

    // [N.T.] Draw the border from the width, height, and symbol passed to the
    // function
    MakeBorder(width, height, "\u2588");

    // [S.C.] Print snake parts
    mvprintw(tl[0], tl[1], "X");
    mvprintw(bod[1][0], bod[1][1], "$");
    mvprintw(bod[2][0], bod[2][1], "$");
    mvprintw(bod[3][0], bod[3][1], "$");
    mvprintw(bod[4][0], bod[4][1], "$");
    mvprintw(head[0], head[1], "S");

    // [A.C.] checks keypress to change direction, will add reverse check when
    // ready
    switch (input) {
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

    // [A.C.] Update head location
    head[0] += xDir;
    head[1] += yDir;
    tl[0] = bod[sLength - 1][0];
    tl[1] = bod[sLength - 1][0];

    // [A.C.] Add more later for other conditions
    for (int i = sLength - 1; i > 0; i--) {
      bod[i][0] = bod[i - 1][0];
      bod[i][1] = bod[i - 1][1];
    }

    // [A.C.] Update body position
    bod[0][0] = head[0];
    bod[0][1] = head[1];

    HandleTrophy(head, bod, &sLength);

    // Draw contents to screen
    refresh();

    // Screen refresh rate
    sleep(1);
  }

  // Cleanup
  endwin();

  return 0;
}

void MakeBorder(int width, int height, char *borderSymbol) {
  // [N.T.] Draw loop for border
  for (int row = 0; row <= width; row++) {
    // Draw left side of the box
    move(row, 0);
    addstr(borderSymbol);

    // Draw the right side of the box
    move(row, height);
    addstr(borderSymbol);

    for (int column = 0; column <= height; column++) {
      // GROUP NOTES: DRAW STUFF HERE

      // Draw top and bottom of border box
      if (row == 0 || row == width) {
        // Set cursor to current row and column
        move(row, column);
        // Print border symbol
        addstr(borderSymbol);
      }
    }
  }
}

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
