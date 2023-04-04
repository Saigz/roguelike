#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <string>
#include <cstring>
#include <cstdlib>


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
  int cur_gold; // current mana
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
    player(int health, int armour, int damage) {
      cur_hp = health;
      max_hp = health;
      armor = armour;
      dmg = damage;
      cur_gold = 0;
      is_alive = true;
      floor_counter = 1;
    };

    // функция передвежния по карте игрока
    void map_movement (int action); 
    void game_movement (int action); 

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
  void taking_damage_and_death(player* pl, int action);

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

// Получение урона от игрока ботом
void mob::taking_damage_and_death(player* pl, int action) {
  if((abs(pl->x - x) <= 1) && (abs(pl->y - y) <= 1) && action == 32) {
    cur_hp = cur_hp - pl->dmg;
  }

  if(cur_hp <= 0 && is_alive) {
    is_alive = false;
    draw_mob(*pl);

    pl->cur_gold += 10; // Даём игроку 10 монет за убийство бота

    mvwprintw(stdscr, 0, 1, "               The monster is killed, 10 coins fell out of it!");
  }
}

// Поведение бота (передвижение бота по карте, нанесение урона игроку и т.д.)
void mob::behavior_bot(player* pl, int action)  {
  if(is_alive){ // Функция активна только при условии того, что бот живой 
    if((abs(pl->x - x) <= 1) && (abs(pl->y - y) <= 1)) { // Поведение бота, если рядом игрок 
      mvwprintw(stdscr, 0, 1, "               Monster noticed you !                      "); // Сообщение о том, что бот занет про игрока 
      if(!are_you_evil_now) { // Если сейчас не злой, то с определённым шансом будет злой 
        int rand_evil_number = (rand() % 101);

        if(evil_rate >= rand_evil_number) {
          are_you_evil_now = true;
        }
      }
      if(are_you_evil_now) { // Если бот злой, то он догоняет игрока и наносит ему урон с определённым шансом
        int rand_damage_number = (rand() % 100);

        if(damage_rate >= rand_damage_number) { // Если бот решил атаковать
          mvwprintw(stdscr, 0, 1, "               Monster dal pizdi you !                   ");
          pl->cur_hp = pl->cur_hp - dmg;
          if(pl->cur_hp <= 0){
            pl->is_alive = false;
          }
        }
      // Алгоритм преследования ботом игрока
        int rand_catch_number = (rand() % 4);

        switch(rand_catch_number) {
          case 0 :
            x = pl -> x + 1;
            y = pl -> y;
            break;
          
          case 1:
            x = pl -> x - 1;
            y = pl -> y;
            break;

          case 2:
            x = pl -> x;
            y = pl -> y + 1;
            break;

          case 3:
            x = pl -> x;
            y = pl -> y - 1;
            break;
        }
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
  this->is_alive = true; //////
  map[x][y] = 'a';
};

// Нанесение бота на экран консоли
void mob::draw_mob(player pl) {
  if((abs(pl.x - x) <= 5) && (abs(pl.y - y) <= 5)) { // Отрисовка только в поле зрения игрока, равному 5 клеткам
    if (is_alive && map_vision[x][y] == 1) {
      if((abs(pl.x - x) <= 1) && (abs(pl.y - y) <= 1)) {
        init_pair(1, COLOR_RED, COLOR_BLACK);
        attron(COLOR_PAIR(1));
        mvaddch(y, x, 'a');
        attroff(COLOR_PAIR(1));
        attrset(0);
      }
      else {
        mvaddch(y, x, 'a');
      }
    } 
    else if (map_vision[x][y] == 1) {
      init_pair(1, COLOR_YELLOW, COLOR_BLACK);
      attron(COLOR_PAIR(1));
      mvaddch(y, x, '%');
      attroff(COLOR_PAIR(1));
      attrset(0);
    } 
  }
};

// передвежение игрока
void player::game_movement(int action) {
    switch (action) {
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
    mvaddch(y, x, '@'); // печать игрока по определенным координатам
};


void player::map_movement(int action) {
    switch (action) {
      case KEY_UP :
      // проверка чтобы не выйти на стену
        if (map[x][y - 1] == ' ') { 
          y--;
          // вижн
          for (int i = x - 2; i < x + 3; i++) {
            for (int j = y - 3; j < y + 2; j++) {
              map_vision[i][j] = 1;
            }
          }
        }
        mvwprintw(stdscr, 0, 1, "               Nothing interesting is happening...                  ");
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
        mvwprintw(stdscr, 0, 1, "               Nothing interesting is happening...                  ");
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
        mvwprintw(stdscr, 0, 1, "               Nothing interesting is happening...                  ");
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
        mvwprintw(stdscr, 0, 1, "               Nothing interesting is happening...                  ");
        break;  

      default:
        break;
    }
    mvaddch(y, x, '@'); // печать игрока по определенным координатам
};


// состояние игрока и мира
void player::draw_stats(int rows, int cols) {
  mvwprintw(stdscr, cols - 1, 1, "HP : %d(%d)    Coins : %d   Armor : %d   Damage : %d", cur_hp, max_hp, cur_gold, armor, dmg);
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
  x = (rand() % start.size_x) + start.x - 1;
  y = (rand() % start.size_y) + start.y;
};

// вывод значка квеста в консоль
void draw_quest(coord quest) {
  if (map_vision[quest.x][quest.y] == 1) {
    mvaddch(quest.y, quest.x, '!');
  }
};

void draw_magazine(coord magazine) {
  if (map_vision[magazine.x][magazine.y] == 1) {
    mvaddch(magazine.y, magazine.x, '$');
  }
};

// вывод значка перехода на след этаж в консоль
void draw_restart(coord restart) {
  if (map_vision[restart.x][restart.y] == 1) {
    mvaddch(restart.y, restart.x, '0');
  }
};

// старт текстового квеста
obj start_quest(int rows, int cols, player *pl) {
  obj quest;
  coord accept;
  coord cancel;
  int action;
  bool is_complete = false;
  const char *mesg = "You see the bottle, do you want to sit on it?";
  coord checkpoint_pl;
  checkpoint_pl.x = pl->x;
  checkpoint_pl.y = pl->y;
  pl->x = rows / 2;
  pl->y = cols / 2;
  accept.x = (rows / 2) - 20;
  accept.y = cols / 2 + 2;
  cancel.x = (rows / 2) + 20;
  cancel.y = cols / 2 + 2;

  while (!is_complete && (action = getch()) != 27) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        mvaddch(j, i, ' ');
      }
    }
    mvwprintw(stdscr, cols / 4, (rows - strlen(mesg)) / 2, "%s", mesg);
    mvwprintw(stdscr, accept.y - 1, accept.x - 1, "YES");
    mvwprintw(stdscr, accept.y, accept.x, "x");
    mvwprintw(stdscr, cancel.y - 1, cancel.x - 1, "NO");
    mvwprintw(stdscr, cancel.y, cancel.x, "x");
    pl->game_movement(action);

    if (pl->x == accept.x && pl->y == accept.y ) {
      pl->cur_hp = pl->cur_hp - 10;
      is_complete = true;
    }
    if (pl->x == cancel.x && pl->y == cancel.y ) {
      pl->dmg = pl->dmg - 10;
      is_complete = true;
    }
  }
  
  pl->x = checkpoint_pl.x;
  pl->y = checkpoint_pl.y;
  quest.x = 0; // убирать квест с карты
  quest.y = 0;
  return quest;
};

void enter_magazine(int rows, int cols, player *pl) {
  coord dmg_up;
  coord restore_hp;
  coord exit_magazine;
  int action;
  bool is_complete = false;
  const char *mesg = "It`s KALINA MOLL";
  coord checkpoint_pl;
  checkpoint_pl.x = pl->x;
  checkpoint_pl.y = pl->y;
  pl->x = rows / 2;
  pl->y = cols / 2;
  dmg_up.x = (rows / 2) - 20;
  dmg_up.y = cols / 2 + 2;
  exit_magazine.x = (rows / 2);
  exit_magazine.y = cols / 2 + 18;
  restore_hp.x = (rows / 2) + 20;
  restore_hp.y = cols / 2 + 2;

  while (!is_complete && (action = getch()) != 27) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        mvaddch(j, i, ' ');
      }
    }
    pl->draw_stats(rows, cols);
    mvwprintw(stdscr, cols / 4, (rows - strlen(mesg)) / 2, "%s", mesg);

    mvwprintw(stdscr, dmg_up.y - 2, dmg_up.x - 2, "10 gold");
    mvwprintw(stdscr, dmg_up.y - 1, dmg_up.x - 3, "DAMAGE UP");
    mvwprintw(stdscr, dmg_up.y, dmg_up.x, "x");

    mvwprintw(stdscr, restore_hp.y - 2, restore_hp.x - 2, "10 gold");
    mvwprintw(stdscr, restore_hp.y - 1, restore_hp.x - 3, "RESTORE HP");
    mvwprintw(stdscr, restore_hp.y, restore_hp.x, "x");

    mvwprintw(stdscr, exit_magazine.y - 1, exit_magazine.x - 2, "EXIT");
    mvwprintw(stdscr, exit_magazine.y, exit_magazine.x, "x");

    pl->game_movement(action);

    if (pl->x == dmg_up.x && pl->y == dmg_up.y && pl->cur_gold >= 10) {
      pl->dmg = pl->dmg + 5;
      pl->cur_gold = pl->cur_gold - 10;
    }
    if (pl->x == restore_hp.x && pl->y == restore_hp.y && pl->cur_gold >= 10) {
      pl->cur_hp = pl->max_hp;
      pl->cur_gold = pl->cur_gold - 10;
    }
    if(pl->x == exit_magazine.x && pl->y == exit_magazine.y) {
      is_complete = true;
    }
  }


  pl->x = checkpoint_pl.x + 1;
  pl->y = checkpoint_pl.y;
}

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

void draw_all(int rows, int cols, room start, room lvl1, room lvl2, room lvl3, room lvl4, coord quest, coord restart, coord magazine, player pl, mob mob_1, mob mob_2, mob mob_3, mob mob_4, mob mob_5) {
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
    // calc_coridors(lvl4, start);
    draw_quest(quest); // quest
    draw_magazine(magazine);
    draw_restart(restart); // переход на след этаж
    pl.draw_stats(rows, cols); // состояние игрока и мира
    mob_1.draw_mob(pl); // Рисуем моба
    mob_2.draw_mob(pl);
    mob_3.draw_mob(pl);
    mob_4.draw_mob(pl);
    mob_5.draw_mob(pl);
    mvaddch(pl.y, pl.x, '@'); // Необходимо для корректной покраски мобов
};


void init_floor(int rows, int cols, room *start, room *lvl1, room *lvl2, room *lvl3, room *lvl4, obj *quest, obj *restart, obj *magazine, mob *mob_1, mob *mob_2, mob *mob_3, mob *mob_4, mob *mob_5, player *pl) {
  // добавляем стены в массив
  fill_map(rows, cols);
  init_map_vision(rows, cols);

  // // random spawn + добавляем комнаты в массив
  start->calc_room_coord(rows, cols);
  lvl1->calc_room_coord(rows, cols);
  lvl2->calc_room_coord(rows, cols);
  lvl3->calc_room_coord(rows, cols);
  lvl4->calc_room_coord(rows, cols);

  magazine->calc_obj_coord(*lvl2);
  quest->calc_obj_coord(*lvl1); // рандомим координаты квеста
  restart->calc_obj_coord(*lvl4); // рандомим координаты перехода на след этаж
  mob_1->spawn_mob(*start);  // рандомим координаты моба 
  mob_2->spawn_mob(*lvl1);
  mob_3->spawn_mob(*lvl2);
  mob_4->spawn_mob(*lvl3);
  mob_5->spawn_mob(*lvl4);
  pl->spawn_player(*start);


};


void menu_start(int rows, int cols, player *pl, bool *is_playing_game) {
  coord start_game , exit_game;
  bool is_complete = false;
  int action;
  pl->x = rows / 2;
  pl->y = cols / 2;
  start_game.x = (rows / 2) - 20;
  start_game.y = (cols / 2) + 2;
  exit_game.x = (rows / 2) + 20;
  exit_game.y = (cols / 2) + 2;

  while (!is_complete && (action = getch()) != 27) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        mvaddch(j, i, ' ');
      }
    }
    mvwprintw(stdscr, start_game.y, start_game.x, "x");
    mvwprintw(stdscr, start_game.y - 1, start_game.x - 5, "START GAME");
    mvwprintw(stdscr, start_game.y - 2, start_game.x - 5, "GIGACHAD :");

    mvwprintw(stdscr, exit_game.y, exit_game.x, "x");
    mvwprintw(stdscr, exit_game.y - 1, exit_game.x - 5, "EXIT GAME");
    mvwprintw(stdscr, exit_game.y - 2, exit_game.x - 5, "VIRGIN :");

    pl->game_movement(action);

    if(pl->x == start_game.x && pl->y == start_game.y) {
      *is_playing_game = true;
      is_complete = true;
    }

    if(pl->x == exit_game.x && pl->y == exit_game.y) {
      *is_playing_game = false;
      is_complete = true;
    }
  }
  


}

void end_game(int rows, int cols, player *pl) {
  coord restart_game;
  const char *mesg = "YOU DIED?? or just went to Chekanov for a colloquium";
  bool is_complete = false;
  int action;
  pl->x = rows / 2;
  pl->y = cols / 2;
  pl->floor_counter = 1;
  restart_game.x = (rows / 2);
  restart_game.y = (cols / 2) + 7;

  while (!is_complete && (action = getch()) != 27) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        mvaddch(j, i, ' ');
      }
    }
    mvwprintw(stdscr, restart_game.y, restart_game.x, "x");
    mvwprintw(stdscr, restart_game.y - 1, restart_game.x - 1, "OK");
    mvwprintw(stdscr, cols / 4, (rows - strlen(mesg)) / 2, "%s", mesg);

    pl->game_movement(action);

    if(pl->x == restart_game.x && pl->y == restart_game.y) {
      is_complete = true;
    }
  }
}

int main() {
  srand(time(NULL));
  int action; // переменная для хранения нажатой клавиши
  int rows = 238, cols = 74; //  границы экрана
  // int rows, cols;
  bool is_playing_game = true;
  player pl(100, 5, 100); // игрок in dung
  room start, lvl1, lvl2, lvl3, lvl4; // комнаты
  obj quest; // quest
  obj restart; // переход нна след этаж
  obj magazine; // magazine
  mob mob_1(10, 1, 1, 99, 99, 99); //тестовый моб
  mob mob_2(10, 1, 1, 99, 99, 99); //тестовый моб
  mob mob_3(10, 1, 1, 99, 99, 99); //тестовый моб
  mob mob_4(10, 1, 1, 99, 99, 99); //тестовый моб
  mob mob_5(10, 1, 1, 99, 99, 99); //тестовый моб




  // mob_1.spawn_mob(start); 
  

  // curses settings
  initscr();                    // start curses
  start_color();		            // colors
  keypad(stdscr, 1);            // allow arrows
  noecho();                     // dont dispay input
  curs_set(0);                  // hide cursor

  // getmaxyx(stdscr, cols, rows); // границы экрана(консоли)
  // getmaxyx(stdscr, rows, cols); // границы экрана(консоли)



  system("clear");

  menu_start(rows, cols, &pl, &is_playing_game);
  init_floor(rows, cols, &start, &lvl1, &lvl2, &lvl3, &lvl4, &quest, &restart, &magazine, &mob_1, &mob_2, &mob_3, &mob_4, &mob_5, &pl); // рандомим этаж


  while(is_playing_game) {

          //передвижение по карте
    do {
      // отрисовываем все обьекты
      draw_all(rows, cols, start, lvl1, lvl2, lvl3, lvl4, quest, restart, magazine, pl, mob_1, mob_2, mob_3, mob_4, mob_5);

      pl.map_movement(action); // поведение игрока

      mob_1.behavior_bot(&pl, action); // поведение бота
      mob_1.taking_damage_and_death(&pl, action); // Получение урона ботом от игрока

      mob_2.behavior_bot(&pl, action); // поведение бота
      mob_2.taking_damage_and_death(&pl, action); // Получение урона ботом от игрока

      mob_3.behavior_bot(&pl, action); // поведение бота
      mob_3.taking_damage_and_death(&pl, action); // Получение урона ботом от игрока

      mob_4.behavior_bot(&pl, action); // поведение бота
      mob_4.taking_damage_and_death(&pl, action); // Получение урона ботом от игрока

      mob_5.behavior_bot(&pl, action); // поведение бота
      mob_5.taking_damage_and_death(&pl, action); // Получение урона ботом от игрока

      draw_all(rows, cols, start, lvl1, lvl2, lvl3, lvl4, quest, restart, magazine, pl, mob_1, mob_2, mob_3, mob_4, mob_5);
      

      if(pl.x == quest.x && pl.y == quest.y) { 
        quest = start_quest(rows, cols, &pl);
      }
      if(pl.x == magazine.x && pl.y == magazine.y) { 
        enter_magazine(rows, cols, &pl);
      }
    
      if(pl.x == restart.x && pl.y == restart.y) {
        init_floor(rows, cols, &start, &lvl1, &lvl2, &lvl3, &lvl4, &quest, &restart, &magazine, &mob_1, &mob_2, &mob_3, &mob_4, &mob_5, &pl); // рандомим этаж
        pl.floor_counter++; // рандомим этаж и увеличениее счетчика этажа
      }
    
    } while((action = getch()) != 27 && pl.is_alive && pl.floor_counter != 5); // 27 - escape - leave from cycle and while player is alive

    end_game(rows, cols, &pl);
    menu_start(rows, cols, &pl, &is_playing_game);
    init_floor(rows, cols, &start, &lvl1, &lvl2, &lvl3, &lvl4, &quest, &restart, &magazine, &mob_1, &mob_2, &mob_3, &mob_4, &mob_5, &pl); // рандомим этаж

  }




    endwin(); // end curses
    return 0;
}