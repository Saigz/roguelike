#include <iostream>
#include <ncurses.h>
// основнная тест комната
char room[80][24];



//создание комнаты
void create_room(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        room[i][j]  = '#';
        mvaddch(i, j, '#');
      }
    }
    for (int i = 5; i < 21; i++) {
      for (int j = 11; j < 21; j++) {
        room[i][j]  = ' ';
        mvaddch(j, i, ' ');
      }
  }
}

using namespace std;

int main() {
    int x = 7, y = 18, action, rows, cols;
	getch();


    initscr();
    keypad(stdscr, 1); // allow arrows
    noecho(); // dont dispay input
    curs_set(0); // hide cursor
    getmaxyx(stdscr, rows, cols); // границы экрана






  do {
    // system("clear");
    create_room(rows, cols);
    switch (action) {
      case KEY_UP :
      if (room[x][y - 1] == ' ')
      {
        y--;
      }
        break;

      case KEY_DOWN :
      if (room[x][y + 1] == ' ')
      {
        y++;
      }
        break;
        
      case KEY_RIGHT :
      if (room[x + 1][y] == ' ')
      {
        x++;
      }
      
        break;

      case KEY_LEFT :
      if (room[x - 1][y] == ' ')
      {
        x--;
      }
      
        break;  
      
      default:
        break;
    }
    mvaddch(y, x, '@');
      
  } while((action = getch()) != 27); // 27 - escape - leave from cycle
    
    // getch();
    endwin();
    return 0;
};