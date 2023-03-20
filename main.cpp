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

  room calc_coord(int rows, int cols, room start);
  void create_room(int rows, int cols, room start);
  void draw_room(int rows, int cols, room start);
};


class player : public creature {
  public :


    // функция передвежния по карте игрока
    player movement (player pl, int action); 
};


// Функции и методы 


room room::calc_coord(int rows, int cols, room start) {
  start.x = (rand() % (rows - 20)) + 6; // положение комнаты
  start.y = (rand() % (cols - 20)) + 6;
  start.size_x = (rand() % 14) + 6; // размер комнаты
  start.size_y = (rand() % 14) + 6;
  
  return start;
};



player player::movement(player pl, int action) {
    switch (action) {

      case KEY_LEFT :
      if (map[pl.x][pl.y - 1] == ' ') { // проверка чтобы не выйти на стену
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
    mvaddch(pl.x, pl.y, '@'); // печать игрока по определенным координатам
    return pl;
};

// заполнение  массива стенами
void fill_map(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      map[i][j] = '#';
    }
  }
};


// вывод стен в консоль
void draw_walls(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      mvaddch(i, j, '#');
    }
  }
};


// создание комнаты в массиве
void room::create_room(int rows, int cols, room start) {
  for (int i = start.x; i <= start.x + start.size_x; i++) {
    for (int j = start.y; j <= start.y + start.size_y; j++) {
      map[i][j]  = ' ';
    }
  }
};


// вывод комнат в консоль
void room::draw_room(int rows, int cols, room start) {
  for (int i = start.x; i <= start.x + start.size_x; i++) {
    for (int j = start.y; j <= start.y + start.size_y; j++) {
      mvaddch(i, j, ' ');
    }
  }
};




int main() {
  srand(time(NULL));
  int action; // переменная для хранения нажатой клавиши
  int rows = 74, cols = 238; //  границы экрана
  player pl; // игрок
  room start, lvl1, lvl2, lvl3, lvl4; // комнаты


  // random spawn
  start = start.calc_coord(rows, cols, start);
  lvl1 = lvl1.calc_coord(rows, cols, lvl1);
  lvl2 = lvl2.calc_coord(rows, cols, lvl2);
  lvl3 = lvl3.calc_coord(rows, cols, lvl3);
  lvl4 = lvl4.calc_coord(rows, cols, lvl4);

  pl.x = start.x + 3; // начально положение  игрока
  pl.y = start.y + 3; 


  // curses console settings
  initscr();                    // start curses
  keypad(stdscr, 1);            // allow arrows
  noecho();                     // dont dispay input
  curs_set(0);                  // hide cursor

  // getmaxyx(stdscr, rows, cols); // границы экрана(консоли)


  // добавляем комнаты в массив
  fill_map(rows, cols);
  start.create_room(rows, cols, start);
  lvl1.create_room(rows, cols, lvl1);
  lvl2.create_room(rows, cols, lvl2);
  lvl3.create_room(rows, cols, lvl3);
  lvl4.create_room(rows, cols, lvl4);


  system("clear");


  //передвижение по карте
  do {
    //отрисовываем карту
    draw_walls(rows, cols);
    start.draw_room(rows, cols, start);
    lvl1.draw_room(rows, cols, lvl1);
    lvl2.draw_room(rows, cols, lvl2);
    lvl3.draw_room(rows, cols, lvl3);
    lvl4.draw_room(rows, cols, lvl4);



    pl = pl.movement(pl, action);

  } while((action = getch()) != 27); // 27 - escape - leave from cycle
    






    endwin(); // end curses
    return 0;
};