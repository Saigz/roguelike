#include <iostream>
#include <ncurses.h>

using namespace std;

// основнная тест комната
char room[238][74];



// Классы


// класс для хранения координат обьектов
class coord {
  public :
  int x;
  int y;
};


class creature : public coord {
  public :
  int hp;      // health
  int mana;    // mana
  int dmg;     // урон
  int armor;   // броня


};




class player : public creature {
  public :


    // функция передвежния по карте игрока
    player movement (player pl, int action); 
};


// Функции и методы 


player player::movement(player pl, int action) {
    switch (action) {

      case KEY_UP :
      if (room[pl.x][pl.y - 1] == ' ') {
          pl.y--;
        }
        break;

      case KEY_DOWN :
        if (room[pl.x][pl.y + 1] == ' ') {
          pl.y++;
        }
        break;
        
      case KEY_RIGHT :
        if (room[pl.x + 1][pl.y] == ' ') {
          pl.x++;
        }
        break;

      case KEY_LEFT :
        if (room[pl.x - 1][pl.y] == ' ') {
          pl.x--;
        }
        break;  

      default:
        break;
    }
    mvaddch(pl.y, pl.x, '@');
    return pl;
};



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





int main() {
    int action, rows = 74, cols = 238; // переменная для хранения клавиши + границы экрана
    player pl; // игрок

    pl.x = 11; // начально положение  игрока
    pl.y = 11; 



    initscr();                    // start curses
    keypad(stdscr, 1);            // allow arrows
    noecho();                     // dont dispay input
    curs_set(0);                  // hide cursor

    // getmaxyx(stdscr, rows, cols); // границы экрана





  //передвижение по карте
  do {

    create_room(rows, cols);
    pl = pl.movement(pl, action);

  } while((action = getch()) != 27); // 27 - escape - leave from cycle
    






    endwin(); // end curses
    return 0;
};