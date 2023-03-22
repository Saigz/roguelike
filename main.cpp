#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <string>

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
  int cur_hp;  // current health
  int max_hp;  // max health
  int mana;    // mana
  int dmg;     // урон
  int armor;   // броня
  // bool is_alive = 1; // живой?

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

  int collision = 1;


  while (collision == 1 ) {

    start.x = (rand() % (rows - 30)) + 6; // положение комнаты
    start.y = (rand() % (cols - 30)) + 6;
    start.size_x = (rand() % 10) + 5; // размер комнаты
    start.size_y = (rand() % 15) + 8;

    collision = 0;
    // антиналожение друг на друга комнат
    for (int i = start.x; i < start.x + start.size_x; i++) {
      for (int j = start.y; j < start.y + start.size_y; j++) {

        if (map[i][j] == ' ' || map[i - 1][j] == ' ' 
        || map[i][j - 1] == ' ' || map[i + 1][j] == ' ' 
        || map[i][j + 1] == ' ') {
          collision = 1;
          i = start.x + start.size_x;
          j = start.y + start.size_y;
        }
      }
    }



  }
  
  

  
  return start;
};



player player::movement(player pl, int action) {
    switch (action) {

      case KEY_LEFT :
      if (map[pl.x][pl.y - 1] != '#') { // проверка чтобы не выйти на стену
          pl.y--;
        }
        break;

      case KEY_RIGHT :
        if (map[pl.x][pl.y + 1] != '#') {
          pl.y++;
        }
        break;
        
      case KEY_DOWN :
        if (map[pl.x + 1][pl.y] != '#') {
          pl.x++;
        }
        break;

      case KEY_UP :
        if (map[pl.x - 1][pl.y] != '#') {
          pl.x--;
        }
        break;  

      default:
        break;
    }
    mvaddch(pl.x, pl.y, '@'); // печать игрока по определенным координатам
    return pl;
};

void player::draw_stats(player pl, int rows, int cols) {
  const char *mesg = "";
  mvwprintw(stdscr, rows, (cols - strlen(mesg)) / 2, "%s", mesg);
}

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
  for (int i = 0; i < rows - 2; i++) {
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


coord create_quest(room start) {
  coord quest;
    quest.x = (rand() % start.size_x) + start.x;
    quest.y = (rand() % start.size_y) + start.y;
  return quest;
};

void draw_quest(coord quest) {
  mvaddch(quest.x, quest.y, '!');
};

coord start_quest(int rows, int cols) {
  coord quest;
  const char *mesg = "questt blablalalla ( press something to contunue )";

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      mvaddch(i, j, ' ');
    }
  }

  mvwprintw(stdscr, rows / 2, (cols - strlen(mesg)) / 2, "%s", mesg);
  // getch();
  quest.x = 0;
  quest.y = 0;
  return quest;
};

void calc_coridors(room old, room neww) {
  int neww_center_x, neww_center_y, old_center_x, old_center_y;
  neww_center_x = neww.x + (neww.size_x / 2);
  neww_center_y = neww.y + (neww.size_y / 2);
  old_center_x = old.x + (old.size_x / 2);
  old_center_y = old.y + (old.size_y / 2);

  int y;
  for (y = old_center_y; y != neww_center_y; ) {
    map[old_center_x][y] = ' ';
    mvaddch(old_center_x, y, ' ');

    if (old_center_y < neww_center_y) {
      y++;
    } else {
      y--;
    }
  }

  for (int x = old_center_x; x != neww_center_x; ) {
    map[x][y] = ' ';
    mvaddch(x, y, ' ');
    
    
    if (old_center_x < neww_center_x) {
      x++;
    } else {
      x--;
    }
  }
  
  
}

int main() {
  srand(time(NULL));
  int action; // переменная для хранения нажатой клавиши
  int rows = 74, cols = 238; //  границы экрана
  player pl; // игрок
  room start, lvl1, lvl2, lvl3, lvl4; // комнаты
  coord quest; // quest


  // добавляем стены в массив
  fill_map(rows, cols);


  // random spawn + добавляем комнаты в массив
  start = start.calc_coord(rows, cols, start);
  start.create_room(rows, cols, start);

  lvl1 = lvl1.calc_coord(rows, cols, lvl1);
  lvl1.create_room(rows, cols, lvl1);

  lvl2 = lvl2.calc_coord(rows, cols, lvl2);
  lvl2.create_room(rows, cols, lvl2);

  lvl3 = lvl3.calc_coord(rows, cols, lvl3);
  lvl3.create_room(rows, cols, lvl3);

  lvl4 = lvl4.calc_coord(rows, cols, lvl4);
  lvl4.create_room(rows, cols, lvl4);


  pl.x = start.x + 3; // начально положение  игрока
  pl.y = start.y + 3; 


  // curses settings
  initscr();                    // start curses
  keypad(stdscr, 1);            // allow arrows
  noecho();                     // dont dispay input
  curs_set(0);                  // hide cursor

  // getmaxyx(stdscr, rows, cols); // границы экрана(консоли)


  quest = create_quest(lvl1); // рандомим координаты квеста

  system("clear");


  //передвижение по карте
  do {
    //отрисовываем карту
    draw_walls(rows, cols);
    start.draw_room(rows, cols, start);
    calc_coridors(start, lvl1);
    lvl1.draw_room(rows, cols, lvl1);
    calc_coridors(lvl1, lvl2);
    lvl2.draw_room(rows, cols, lvl2);
    calc_coridors(lvl2, lvl3);
    lvl3.draw_room(rows, cols, lvl3);
    calc_coridors(lvl3, lvl4);
    lvl4.draw_room(rows, cols, lvl4);
    calc_coridors(lvl4, start);
    draw_quest(quest);




    pl = pl.movement(pl, action);


    if(pl.x == quest.x && pl.y == quest.y) {
      quest = start_quest(rows, cols);
    }
  } while((action = getch()) != 27); // 27 - escape - leave from cycle
    






    endwin(); // end curses
    return 0;
};