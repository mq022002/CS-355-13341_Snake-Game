# CS-355-13341_Snake-Game
Final Project for CS-355-13341, Systems Programming, at Central Connecticut State University. Team: Adrian Cruz, Steven Consolini, Nicholas Tranquilli, and Matthew Quijano.

To run this project:
1) Head over to: https://www.onlinegdb.com/
2) Select 'C' in the drop down menu next to 'Language'
3) Copy and paste our code from main.c
4) Click run
5) Enjoy

Appropriately authored requirements for this project, according to what our professor posted:
```The snake pit:```
- [N.T.] [001] The snake pit is the area where the snake can move.
- [N.T.] [002] The snake pit must utilize all available space of the current terminal window.
- [N.T.] [003] There must be a visible border delineating the snake pit.
- 
```The snake:```
- [S.C.] [004] The initial length of the snake is five characters.
- [S.C.] [005] Initial direction of the snake's movement is chosen randomly.
- [A.C] [006] The user can press one of the four arrow keys to change the direction of the snake's  movement.
- [M.Q.] [007] The snake's speed is proportional to its length.

```The trophies:```
- [M.Q.] [008] Trophies are represented by a digit randomly chosen from 1 to 9.
- [M.Q.] [009] There's always exactly one trophy in the snake pit at any given moment.
- [N.T.] [010] When the snake eats the trophy, its length is increased by the corresponding number of characters.
- [M.Q.] [011] A trophy expires after a random interval from 1 to 9 seconds.
- [M.Q.] [012] A new trophy is shown at a random location on the screen after the previous one has either expired or is eaten by the snake
- [M.Q.] [S001]  How did you manage displaying and changing the location of trophy?
- [M.Q.] [S002]  What about the random locations on the border of your screen?
-  [A.C.] [S004]  How do you add points if the snake eats the trophy?
-  
```The gameplay:```
- [S.C.] [013] The snake dies and the game ends if:
 - [S.C.] It runs into the border; or
 - [S.C.] It runs into itself; or
 - [S.C.] The user attempts to reverse the snake's direction.
- [A.C] [014]The user wins the game if the snake's length grows to the length equal to half the perimeter of the border.
- [A.C.] [S005]  Do you display total points for the player? 
- [M.Q] [S006]  Does your program handle interrupt signal to end the game?
- 
```Extra credit:```
- [N.T.] [E001] Multiplayer Functionality
- [N.T.] [E002] Main Menu
