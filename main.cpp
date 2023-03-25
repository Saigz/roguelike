#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <string>
#include <cstring>


using namespace std;

// основнная тест комната
char map[238][74];



// Классы

class interface {
  public :

  
};

// класс для хранения координат обьектов
class coord {
  public :
  int x;
  int y;
};

class creature : public coord {
  public :
  int cur_hp;   // current health
  int max_hp;   // max health
  int cur_mana; // current mana
  int max_mana; // max mana
  int dmg;      // урон
  int armor;    // броня
  bool is_alive; // живой?

};


class room : public coord {
  public :
  int size_x;
  int size_y;

  void calc_coord(int rows, int cols);// рассчитывает координаты комнаты
  void create_room(int rows, int cols);// добавляет и нформацию о комнате в массив
  void draw_room(int rows, int cols);// рисует комнаты в консоле
};


class player : public creature {
  public :
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
};

//класс для мобов
class mob : public creature {
  public :
  int attack_radius; // Радиус атаки бота
  int evil_rate; // В скольких случаях из ста бот будет агриться на человека
  int damage_rate; // В скольких случаях из ста бот будет наносить урон игроку
  float spawn_rate;// В последующем будет влиять на шанс выпадения
  bool are_you_evil_now;
    
  mob spawn_mob (room start);
  void draw_mob(player pl, mob mob);
  void active_mode(player pl, mob mob);
  mob move_bot(player pl, mob mob, int action);
};


// Функции и методы 


// Поведение бота (передвижение бота по карте, нанесение урона игроку и т.д.)
mob mob::move_bot(player pl, mob mob, int action){
  if(mob.is_alive){ // Функция активна только при условии того, что бот живой 
    if((abs(pl.x - mob.x) <= 1) && (abs(pl.y - mob.y) <= 1) && mob.is_alive){ // Поведение бота, если рядом игрок 

      if(!mob.are_you_evil_now){ // Если сейчас не злой, то с определённым шансом будет злой 
        int rand_evil_number = (rand() % 101);

        if(mob.evil_rate >= rand_evil_number){
          mob.are_you_evil_now = true;
        }
      }
      if(mob.are_you_evil_now){ // Если бот злой, то он догоняет игрока и наносит ему урон с определённым шансом
        int rand_damage_number = (rand() % 100);

        if(mob.damage_rate >= rand_damage_number){ // Если бот решил атаковать
          pl.cur_hp = pl.cur_hp - mob.dmg;
        }

        //switch(action){ // Бот догоняет игрока

          //case KEY_LEFT :
            //mob.y--;
            //break;

          //case KEY_RIGHT :
            //mob.y++;
            //break;
            
          //case KEY_DOWN :
            //mob.x++;
            //break;

          //case KEY_UP :
            //mob.x--;
            //break;  

          //default:
            //break;
        //}        
      }

      return(mob);
    }

    else{ // Поведение бота, если игрока рядом нету
      int rand_move_number = (rand() % 4);

      switch(rand_move_number){

        case 0 :
          if (map[mob.x][mob.y - 1] == ' ') { // проверка чтобы не выйти за стену
            mob.y--;
          }
          break;

        case 1 :
          if (map[mob.x][mob.y + 1] == ' ') {
            mob.y++;
          }
          break;

        case 2 :
          if (map[mob.x + 1][mob.y] == ' ') {
            mob.x++;
          }
          break;

        case 3 :
          if (map[mob.x - 1][mob.y] == ' ') {
            mob.x--;
          }
          break;
      }
      return(mob);
    }
  }
  return(mob);
};

// Просчёт координат бота
mob mob::spawn_mob(room start) {
  mob mob;
  mob.x = (rand() % start.size_x) + start.x;
  mob.y = (rand() % start.size_y) + start.y;
  return mob;
};

// Нанесение бота на экран консоли
void mob::draw_mob(player pl, mob mob) {
  if((abs(pl.x - mob.x) <= 1) && (abs(pl.y - mob.y) <= 1) && mob.is_alive){
    start_color();		
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    mvaddch(mob.x, mob.y, 'a');
    attroff(COLOR_PAIR(1));
  }
  else if(mob.is_alive){
    mvaddch(mob.x, mob.y, 'a');
  }
  else{
    start_color();		
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    mvaddch(mob.x, mob.y, '%');
    attroff(COLOR_PAIR(1));
  }
};

void player::movement(int action) {
    switch (action) {

      case KEY_LEFT :
      if (map[x][y - 1] == ' ') { // проверка чтобы не выйти на стену
          y--;
        }
        break;

      case KEY_RIGHT :
        if (map[x][y + 1] == ' ') {
          y++;
        }
        break;
        
      case KEY_DOWN :
        if (map[x + 1][y] == ' ') {
          x++;
        }
        break;

      case KEY_UP :
        if (map[x - 1][y] == ' ') {
          x--;
        }
        break;  

      default:
        break;
    }
    mvaddch(x, y, '@'); // печать игрока по определенным координатам
};

void player::draw_stats(int rows, int cols) {
  mvwprintw(stdscr, rows - 1, 1, "HP : %d(%d)    Mana : %d(%d)   Armor : %d   Damage : %d", cur_hp, max_hp, cur_mana, max_mana, armor, dmg);
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
  for (int i = 0; i < rows - 1; i++) {
    for (int j = 0; j < cols; j++) {
      mvaddch(i, j, '#');
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

void room::calc_coord(int rows, int cols) {

  int collision = 1;


  while (collision == 1 ) {

    x = (rand() % (rows - 30)) + 6; // положение комнаты
    y = (rand() % (cols - 30)) + 6;
    size_x = (rand() % 10) + 5; // размер комнаты
    size_y = (rand() % 15) + 8;

    collision = 0;
    // антиналожение друг на друга комнат
    for (int i = x; i < x + size_x; i++) {
      for (int j = y; j < y + size_y; j++) {

        if (map[i][j] == ' ' || map[i - 1][j] == ' ' 
        || map[i][j - 1] == ' ' || map[i + 1][j] == ' ' 
        || map[i][j + 1] == ' ') {
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
  const char *mesg = "questt blablalalla ( press something to contunue)";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mvaddch(i, j, ' ');
        }
    }
  mvwprintw(stdscr, rows / 2, (cols - strlen(mesg)) / 2, "%s", mesg);
  getch();
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

void draw_all(int rows, int cols, room start, room lvl1, room lvl2, room lvl3, room lvl4, coord quest, player pl, mob test_mob) {
   //отрисовываем карту
    draw_walls(rows, cols);
    start.draw_room(rows, cols);
    calc_coridors(start, lvl1);
    lvl1.draw_room(rows, cols);
    calc_coridors(lvl1, lvl2);
    lvl2.draw_room(rows, cols);
    calc_coridors(lvl2, lvl3);
    lvl3.draw_room(rows, cols);
    calc_coridors(lvl3, lvl4);
    lvl4.draw_room(rows, cols);
    calc_coridors(lvl4, start);
    draw_quest(quest);
    pl.draw_stats(rows, cols);
    test_mob.draw_mob(pl, test_mob); // рисуем моба
};



int main() {
  srand(time(NULL));
  int action; // переменная для хранения нажатой клавиши
  int rows = 74, cols = 238; //  границы экрана
  player pl(100, 5, 5, 100); // игрок



  room start, lvl1, lvl2, lvl3, lvl4; // комнаты
  coord quest; // quest
  mob test_mob; //тестовый моб

  test_mob.cur_hp = 10;
  test_mob.dmg = 50;
  test_mob.are_you_evil_now = false;


  // добавляем стены в массив
  fill_map(rows, cols);


  // random spawn + добавляем комнаты в массив
  start.calc_coord(rows, cols);
  lvl1.calc_coord(rows, cols);
  lvl2.calc_coord(rows, cols);
  lvl3.calc_coord(rows, cols);
  lvl4.calc_coord(rows, cols);


  pl.x = start.x + 3; // начально положение  игрока
  pl.y = start.y + 3; 


  // curses settings
  initscr();                    // start curses
  keypad(stdscr, 1);            // allow arrows
  noecho();                     // dont dispay input
  curs_set(0);                  // hide cursor

  // getmaxyx(stdscr, rows, cols); // границы экрана(консоли)


  quest = create_quest(lvl1); // рандомим координаты квеста
  test_mob = test_mob.spawn_mob(start); // рандомим координаты моба 

  system("clear");


  //передвижение по карте
  do {
    draw_all(rows, cols, start, lvl1, lvl2, lvl3, lvl4, quest, pl, test_mob);



    test_mob = test_mob.move_bot(pl, test_mob, action); // поведение бота 
    pl.movement(action);
  


    if(pl.x == quest.x && pl.y == quest.y) { 
      quest = start_quest(rows, cols);
    }
    if(action == 32){ // если нажали клавишу атаки(пробел)
      test_mob.cur_hp = test_mob.cur_hp - pl.dmg;

      if(test_mob.cur_hp <= 0){ // если боту нанесли критический урон
        test_mob.is_alive = false;
      }
    }
   
  } while((action = getch()) != 27); // 27 - escape - leave from cycle
    






    endwin(); // end curses
    return 0;
}