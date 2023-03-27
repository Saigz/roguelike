#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <string>
#include <cstring>


using namespace std;

// карты
char map[238][74]; // карта обьектов
int map_vision[238][74]; // карта вижена


// Классы

// класс для хранения координат обьектов
class coord {
  public :
  int x;
  int y;
};

// класс живых созданий ( игрока, мобов)
class creature : public coord {
  public :
  int cur_hp;   // current health
  int max_hp;   // max health
  int cur_mana; // current mana
  int max_mana; // max mana
  int dmg;      // урон
  int armor;    // броня
  bool is_alive;// живой?

};

// класс комнаты
class room : public coord {
  public :
  int size_x; // ширина комнаты
  int size_y; // длина комнаты

  room() {
    size_x = 0;
    size_y = 0;
  }

  void calc_room_coord(int rows, int cols); // рассчитывает координаты комнаты
  void create_room(int rows, int cols);// добавляет и нформацию о комнате в массив
  void draw_room(int rows, int cols);// рисует комнаты в консоле
};


// класс игрока
class player : public creature {
  public :
	int floor_counter; // счетчик этажа ( 5 - этажей победа)

  // default stats 
    player(int health, int armour, int damage, int mana) {
      cur_hp = health;
      max_hp = health;
      armor = armour;
      dmg = damage;
      cur_mana = mana;
      max_mana = mana;
      is_alive = true;
    };

    // функция передвежния по карте игрока
    void movement (int action); 
    //функция вывода статов
    void draw_stats (int rows, int cols);
    // спавн игрока
    void spawn_player (room start);
};

//класс для мобов
class mob : public creature {
  public :
  int attack_radius; // Радиус атаки бота
  int evil_rate; // В скольких случаях из ста бот будет агриться на человека
  int damage_rate; // В скольких случаях из ста бот будет наносить урон игроку
  int spawn_rate;// В последующем будет влиять на шанс выпадения
  bool are_you_evil_now;
    
  void spawn_mob (room start);
  void draw_mob(player pl);
  void behavior_bot(player* pl, int action);

  mob(int u_cur_hp, int u_dmg, int u_attack_radius, int u_evil_rate, int u_damage_rate, int u_spawn_rate){
    cur_hp = u_cur_hp;
    dmg = u_dmg;

    attack_radius = u_attack_radius;
    evil_rate = u_evil_rate;
    damage_rate = u_damage_rate;
    spawn_rate = u_spawn_rate;
    are_you_evil_now = false;
    is_alive = true;
  }
};

class obj : public coord {
  public :
  void calc_obj_coord(room start);
};


// Функции и методы 


// Поведение бота (передвижение бота по карте, нанесение урона игроку и т.д.)
void mob::behavior_bot(player* pl, int action)  {
  if(is_alive){ // Функция активна только при условии того, что бот живой 
    if((abs(pl->x - x) <= 1) && (abs(pl->y - y) <= 1)) { // Поведение бота, если рядом игрок 

      if(!are_you_evil_now) { // Если сейчас не злой, то с определённым шансом будет злой 
        int rand_evil_number = (rand() % 101);

        if(evil_rate >= rand_evil_number) {
          are_you_evil_now = true;
        }
      }
      if(are_you_evil_now) { // Если бот злой, то он догоняет игрока и наносит ему урон с определённым шансом
        int rand_damage_number = (rand() % 100);

        if(damage_rate >= rand_damage_number) { // Если бот решил атаковать
          pl->cur_hp = pl->cur_hp - dmg;
          if(pl->cur_hp <= 0){
            pl->is_alive = false;
          }
        }

        // Тут может быть функция догонялок

      }

    } else { // Поведение бота, если игрока рядом нету
      int rand_move_number = (rand() % 4);

      switch(rand_move_number) {
        case 0 :
          if (map[x][y - 1] == ' ') { // проверка чтобы не выйти за стену
            y--;

            map[x][y] = 'a';
            map[x][y + 1] = ' ';
          }
          break;

        case 1 :
          if (map[x][y + 1] == ' ') {
            y++;

            map[x][y] = 'a';
            map[x][y - 1] = ' ';
          }
          break;

        case 2 :
          if (map[x + 1][y] == ' ') {
            x++;

            map[x][y] = 'a';
            map[x - 1][y] = ' ';
          }
          break;

        case 3 :
          if (map[x - 1][y] == ' ') {
            x--;

            map[x][y] = 'a';
            map[x + 1][y] = ' ';
          }
          break;
      }
    }
  }
};

// Просчёт координат бота
void mob::spawn_mob(room start) {
  x = (rand() % start.size_x) + start.x;
  y = (rand() % start.size_y) + start.y;
  this->is_alive = false; //////
  map[x][y] = 'a';
};

// Нанесение бота на экран консоли
void mob::draw_mob(player pl) {
  if((abs(pl.x - x) <= 1) && (abs(pl.y - y) <= 1) && is_alive) {
    start_color();		
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    mvaddch(y, x, 'a');
    attroff(COLOR_PAIR(1));
  }
  else if(is_alive) {
    mvaddch(y, x, 'a');
  }
  else{
    start_color();		
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    mvaddch(y, x, '%');
    attroff(COLOR_PAIR(1));
  }
};

// передвежение игрока
void player::movement(int action) {
    switch (action) {
      case KEY_UP :
      // проверка чтобы не выйти на стену
        if (map[x][y - 1] == ' ') { 
          y--;
          // вижн
          for (int i = x - 3; i < x + 3; i++) {
            for (int j = y - 3; j < y + 2; j++) {
              map_vision[i][j] = 1;
            }
          }
        }
        break;

      case KEY_DOWN :
        if (map[x][y + 1] == ' ') {
          y++;
          for (int i = x - 2; i < x + 3; i++) {
            for (int j = y - 2; j < y + 4; j++) {
              map_vision[i][j] = 1;
            }
          }
        }
        break;
        
      case KEY_RIGHT :
        if (map[x + 1][y] == ' ') {
          x++;
          for (int i = x; i < x + 4; i++) {
            for (int j = y - 2; j < y + 3; j++) {
              map_vision[i][j] = 1;
            }
          }
        }
        break;

      case KEY_LEFT :
        if (map[x - 1][y] == ' ') {
          for (int i = x - 4; i < x + 1; i++) {
            for (int j = y - 2; j < y + 3; j++) {
              map_vision[i][j] = 1;
            }
          }
          x--;
        }
        break;  

      default:
        break;
    }
    mvaddch(y, x, '@'); // печать игрока по определенным координатам
};

// состояние игрока и мира
void player::draw_stats(int rows, int cols) {
  mvwprintw(stdscr, cols - 1, 1, "HP : %d(%d)    Mana : %d(%d)   Armor : %d   Damage : %d", cur_hp, max_hp, cur_mana, max_mana, armor, dmg);
  mvwprintw(stdscr, 0, 1, "Floor : %d", floor_counter);
}

// обнуляем вижн
void init_map_vision(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      map_vision[i][j] = 0;
    }
  }
};

// спавн игрока
void player::spawn_player(room start) {
  x = start.x + 3;
  y = start.y + 3;

// spawn vision
  for (int i = x - 3; i < x + 5; i++) {
    for (int j = y - 3; j < y + 5; j++) {
      map_vision[i][j] = 1;
    }
  }
  
};

// заполнение  массива стенами
void fill_map(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      map[i][j] = '#'; // # - стена
    }
  }
};



// вывод стен в консоль
void draw_walls(int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 1; j < cols - 1; j++) {
      if (map_vision[i][j] == 1) {
        mvaddch(j, i, '#');
      } else {
        mvaddch(j, i, '.');
      }
    }
  }
};

// создание комнаты в массиве
void room::create_room(int rows, int cols) {
  for (int i = x; i <= x + size_x; i++) {
    for (int j = y; j <= y + size_y; j++) {
      map[i][j]  = ' ';
    }
  }
};

// рандоминг расположения комнаты на карте  + добавление ее в массив карту обьектов
void room::calc_room_coord(int rows, int cols) {
  int collision = 1;

  while (collision == 1) {

    x = (rand() % (rows - 30)) + 6; // положение комнаты
    y = (rand() % (cols - 30)) + 6;
    size_x = (rand() % 15) + 8; // размер комнаты
    size_y = (rand() % 10) + 5;
    collision = 0;

    // антиналожение друг на друга комнат
    for (int i = x; i < x + size_x; i++) {
      for (int j = y; j < y + size_y; j++) {

        if (map[i][j] == ' ' || map[i - 2][j] == ' ' 
        || map[i][j - 2] == ' ' || map[i + 2][j] == ' ' 
        || map[i][j + 2] == ' ') {
          collision = 1;
          i = x + size_x;
          j = y + size_y;
        }
      }
    }
  }
  create_room(rows, cols);
};


// вывод комнат в консоль
void room::draw_room(int rows, int cols) {
  for (int i = x; i <= x + size_x; i++) {
    for (int j = y; j <= y + size_y; j++) {
      if (map_vision[i][j] == 1) {
      mvaddch(j, i, ' ');
      } else {
      mvaddch(j, i, '.');
      }
    }
  }
};

// рандоминг координаты обьекта в выбранной комнате(квесты, переход на след этаж и тп)
void obj::calc_obj_coord(room start) {
  x = (rand() % start.size_x) + start.x;
  y = (rand() % start.size_y) + start.y;
};

// вывод значка квеста в консоль
void draw_quest(coord quest) {
  mvaddch(quest.y, quest.x, '!');
};

// вывод значка перехода на след этаж в консоль
void draw_restart(coord restart) {
  mvaddch(restart.y, restart.x, '0');
};

// старт текстового квеста
obj start_quest(int rows, int cols) {
  obj quest;
  const char *mesg = "questt blablalalla ( press something to contunue)";
  for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        mvaddch(j, i, ' ');
      }
  }
  mvwprintw(stdscr, rows / 2, (cols - strlen(mesg)) / 2, "%s", mesg);
  // getch();
  quest.x = 0; // убирать квест с карты
  quest.y = 0;
  return quest;
};

// рассчитывание коридоров между комнатами
void calc_coridors(room old, room neww) {
  int neww_center_x, neww_center_y, old_center_x, old_center_y;
  //  центры комнат по х и у
  neww_center_x = neww.x + (neww.size_x / 2); 
  neww_center_y = neww.y + (neww.size_y / 2);
  old_center_x = old.x + (old.size_x / 2);
  old_center_y = old.y + (old.size_y / 2);

  int y;
  for (y = old_center_y; y != neww_center_y; ) {

    map[old_center_x][y] = ' ';

    if (map_vision[old_center_x][y] == 1) {
    mvaddch(y, old_center_x, ' ');
    } else {
      mvaddch(y, old_center_x, '.');
    }

    if (old_center_y < neww_center_y) {
      y++;
    } else {
      y--;
    }
  }

  for (int x = old_center_x; x != neww_center_x; ) {
    map[x][y] = ' ';

    if (map_vision[x][y] == 1) {
      mvaddch(y, x, ' ');
    } else {
      mvaddch(y, x, '.');
    }
    
    
    if (old_center_x < neww_center_x) {
      x++;
    } else {
      x--;
    }
  }
  
  
}

void draw_all(int rows, int cols, room start, room lvl1, room lvl2, room lvl3, room lvl4, coord quest, coord restart, player pl, mob test_mob) {
   //отрисовываем карту
    draw_walls(rows, cols); // стены
    start.draw_room(rows, cols); // комната
    calc_coridors(start, lvl1); // коридор
    lvl1.draw_room(rows, cols); 
    calc_coridors(lvl1, lvl2);
    lvl2.draw_room(rows, cols);
    calc_coridors(lvl2, lvl3);
    lvl3.draw_room(rows, cols);
    calc_coridors(lvl3, lvl4);
    lvl4.draw_room(rows, cols);
    calc_coridors(lvl4, start);
    draw_quest(quest); // quest
    draw_restart(restart); // переход на след этаж
    pl.draw_stats(rows, cols); // состояние игрока и мира
    test_mob.draw_mob(pl); // Рисуем моба
    mvaddch(pl.y, pl.x, '@'); // Необходимо для корректной покраски мобов
};


void init_floor(int rows, int cols, room *start, room *lvl1, room *lvl2, room *lvl3, room *lvl4, obj *quest, obj *restart, mob *test_mob, player *pl) {
  // добавляем стены в массив
  fill_map(rows, cols);
  init_map_vision(rows, cols);

  // // random spawn + добавляем комнаты в массив
  start->calc_room_coord(rows, cols);
  lvl1->calc_room_coord(rows, cols);
  lvl2->calc_room_coord(rows, cols);
  lvl3->calc_room_coord(rows, cols);
  lvl4->calc_room_coord(rows, cols);

  quest->calc_obj_coord(*lvl1); // рандомим координаты квеста
  restart->calc_obj_coord(*start); // рандомим координаты перехода на след этаж
  test_mob->spawn_mob(*start);  // рандомим координаты моба 
  pl->spawn_player(*start);


};


int main() {
  srand(time(NULL));
  int action; // переменная для хранения нажатой клавиши
  int rows = 238, cols = 74; //  границы экрана
  player pl(100, 5, 100, 100); // игрок
  room start, lvl1, lvl2, lvl3, lvl4; // комнаты
  obj quest; // quest
  obj restart; // переход нна след этаж
  mob test_mob(10, 50, 1, 99, 99, 99); //тестовый моб



  init_floor(rows, cols, &start, &lvl1, &lvl2, &lvl3, &lvl4, &quest, &restart, &test_mob, &pl); // рандомим этаж
  // test_mob.spawn_mob(start); 
  

  // curses settings
  initscr();                    // start curses
  keypad(stdscr, 1);            // allow arrows
  noecho();                     // dont dispay input
  curs_set(0);                  // hide cursor

  // getmaxyx(stdscr, rows, cols); // границы экрана(консоли)



  system("clear");


  //передвижение по карте
  do {
    // отрисовываем все обьекты
    draw_all(rows, cols, start, lvl1, lvl2, lvl3, lvl4, quest, restart, pl, test_mob);
 
    pl.movement(action); // поведение игрока
    test_mob.behavior_bot(&pl, action); // поведение бота
    draw_all(rows, cols, start, lvl1, lvl2, lvl3, lvl4, quest, restart, pl, test_mob);
    

    if(pl.x == quest.x && pl.y == quest.y) { 
      quest = start_quest(rows, cols);
    }
    if(action == 32){ // если нажали клавишу атаки(пробел)
      test_mob.cur_hp = test_mob.cur_hp - pl.dmg;

      if(test_mob.cur_hp <= 0){ // если бот умер
        test_mob.is_alive = false;
        test_mob.draw_mob(pl);
      }
    }
    if(pl.x == restart.x && pl.y == restart.y) {
      init_floor(rows, cols, &start, &lvl1, &lvl2, &lvl3, &lvl4, &quest, &restart, &test_mob, &pl);
      pl.floor_counter++; // рандомим этаж и увеличениее счетчика этажа
    }
   
  } while((action = getch()) != 27 && pl.is_alive && pl.floor_counter != 5); // 27 - escape - leave from cycle and while player is alive


    endwin(); // end curses
    return 0;
}