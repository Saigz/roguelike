#include <iostream>
#include <ncurses.h>

char room[80][24];




void create_room(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        room[i][j]  = '#';
        mvaddch(i, j, '#');
      }
    }
    for (int i = 5; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        room[i][j]  = '#';
        mvaddch(i, j, ' ');
      }
  }
}

using namespace std;

int main() {
    int x = 11, y = 21, c, rows, cols;
	getch();


    initscr();
    keypad(stdscr, 1); // allow arrows
    noecho(); // dont dispay input
    curs_set(0); // hide cursor
    getmaxyx(stdscr, rows, cols); // границы экрана






  do {
    // system("clear");
    // create_room(rows, cols);
      for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        room[i][j]  = '#';
        mvaddch(i, j, '#');
      }
    }
    for (int i = 5; i < rows / 2; i++) {
      for (int j = 11; j < cols / 2; j++) {
        room[i][j]  = '#';
        mvaddch(i, j, ' ');
      }
  }
    switch (c) {
      case KEY_UP :
        y--;
        break;

      case KEY_DOWN :
        y++;
        break;
        
      case KEY_RIGHT :
        x++;
        break;

      case KEY_LEFT :
        x--;
        break;  
      
      default:
        break;
    }
    mvaddch(y, x, '@');
      
  } while((c = getch()) != 27); // 27 - escape - leave from cycle
    
    getch();
    endwin();
    return 0;
};