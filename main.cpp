#include <iostream>
#include <ncurses.h>
#include <ctime>

using namespace std;

// основнная тест комната
char map[238][74];



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


class room : public coord {
  public :
  int size_x;
  int size_y;

};


class player : public creature {
  public :


    // функция передвежния по карте игрока
    player movement (player pl, int action); 
};


// Функции и методы 


player player::movement(player pl, int action) {
    switch (action) {

      case KEY_LEFT :
      if (map[pl.x][pl.y - 1] == ' ') {
          pl.y--;
        }
        break;

      case KEY_RIGHT :
        if (map[pl.x][pl.y + 1] == ' ') {
          pl.y++;
        }
        break;
        
      case KEY_DOWN :
        if (map[pl.x + 1][pl.y] == ' ') {
          pl.x++;
        }
        break;

      case KEY_UP :
        if (map[pl.x - 1][pl.y] == ' ') {
          pl.x--;
        }
        break;  

      default:
        break;
    }
    mvaddch(pl.x, pl.y, '@');
    return pl;
};


void fill_map(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      map[i][j] = '#';
      mvaddch(i, j, '#');
    }
  }
};

// создание комнаты
void create_room(int rows, int cols, room start) {
  for (int i = start.x; i < start.x + start.size_x; i++) {
    for (int j = start.y; j < start.y + start.size_y; j++) {
      map[i][j]  = ' ';
      mvaddch(i, j, ' ');
    }
  }
};





int main() {
  srand(time(NULL));
  int action, rows = 74, cols = 238; // переменная для хранения нажатой клавиши + границы экрана
  player pl; // игрок
  room start;

  start.x = (rand() % (rows - 20)) + 10; // положение комнаты
  start.y = (rand() % (cols - 20)) + 10;
  start.size_x = (rand() % 14) + 6; // размер комнаты
  start.size_y = (rand() % 14) + 6;
  pl.x = start.x + 3; // начально положение  игрока
  pl.y = start.y + 3; 



  initscr();                    // start curses
  keypad(stdscr, 1);            // allow arrows
  noecho();                     // dont dispay input
  curs_set(0);                  // hide cursor

    // getmaxyx(stdscr, rows, cols); // границы экрана(консоли)




  system("clear");
  //передвижение по карте
  do {
    fill_map(rows, cols);
    create_room(rows, cols, start);
    pl = pl.movement(pl, action);

  } while((action = getch()) != 27); // 27 - escape - leave from cycle
    






    endwin(); // end curses
    return 0;
};