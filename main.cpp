#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <string>
#include <cstring>


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
  bool is_alive = true; // живой?



};


class room : public coord {
  public :
  int size_x;
  int size_y;

  room calc_coord(int rows, int cols, room start);// рассчитывает координаты комнаты
  void create_room(int rows, int cols, room start);// добавляет и нформацию о комнате в массив
  void draw_room(int rows, int cols, room start);// рисует комнаты в консоле
};


class player : public creature {
  public :


    // функция передвежния по карте игрока
    player movement (player pl, int action); 
};

//класс для мобов
class mob : public creature{
  public :
  int attack_radius; // Радиус атаки бота
  int evil_rate; // В скольких случаях из ста бот будет агриться на человека
  int damage_rate; // В скольких случаях из ста бот будет наносить урон игроку
  float spawn_rate;// В последующем будет влиять на шанс выпадения
  bool are_you_evil_now = false;
    
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

      if(not mob.are_you_evil_now){ // Если сейчас не злой, то с определённым шансом будет злой 
        int rand_evil_number = (rand() % 101);

        if(mob.evil_rate >= rand_evil_number){
          mob.are_you_evil_now = true;
        }
      }
      if(mob.are_you_evil_now){ // Если бот злой, то он догоняет игрока и наносит ему урон с определённым шансом
        int rand_damage_number = (rand() % 100);

        if(mob.damage_rate >= rand_damage_number){ // Если бот решил атаковать
          pl.hp - mob.dmg;
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



room room::calc_coord(int rows, int cols, room start) {

  int collision = 1;


  while (collision == 1 ) {

    start.x = (rand() % (rows - 20)) + 6; // положение комнаты
    start.y = (rand() % (cols - 20)) + 6;
    start.size_x = (rand() % 10) + 5; // размер комнаты
    start.size_y = (rand() % 15) + 8;


    // антиналожение друг на друга комнат
    for (int i = start.x; i <= start.x + start.size_x; i++) {
      for (int j = start.y; j <= start.y + start.size_y; j++) {

        if (map[i][j] == ' ' || map[i - 2][j] == ' ' 
        || map[i][j - 2] == ' ' || map[i + 2][j] == ' ' 
        || map[i][j + 2] == ' ') {
          collision = 1;
        } else {
          collision = 0;
        }
      }
    }



  }
  
  

  
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



int main() {
  srand(time(NULL));
  int action; // переменная для хранения нажатой клавиши
  int rows = 50, cols = 70; //  границы экрана
  player pl; // игрок
  room start, lvl1, lvl2, lvl3, lvl4; // комнаты
  coord quest; // quest
  mob test_mob; //тестовый моб

  pl.hp = 100;
  pl.dmg = 50;
  test_mob.hp = 10;
  test_mob.dmg = 50;


  // добавляем стены в массив
  fill_map(rows, cols);


  // random spawn
  start = start.calc_coord(rows, cols, start);
  lvl1 = lvl1.calc_coord(rows, cols, lvl1);
  lvl2 = lvl2.calc_coord(rows, cols, lvl2);
  lvl3 = lvl3.calc_coord(rows, cols, lvl3);
  lvl4 = lvl4.calc_coord(rows, cols, lvl4);

  pl.x = start.x + 3; // начально положение  игрока
  pl.y = start.y + 3; 


  // curses settings
  initscr();                    // start curses
  keypad(stdscr, 1);            // allow arrows
  noecho();                     // dont dispay input
  curs_set(0);                  // hide cursor

  // getmaxyx(stdscr, rows, cols); // границы экрана(консоли)


  // добавляем комнаты в массив
  start.create_room(rows, cols, start);
  lvl1.create_room(rows, cols, lvl1);
  lvl2.create_room(rows, cols, lvl2);
  lvl3.create_room(rows, cols, lvl3);
  lvl4.create_room(rows, cols, lvl4);

  quest = create_quest(start); // рандомим координаты квеста
  test_mob = test_mob.spawn_mob(start); // рандомим координаты моба 

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
    draw_quest(quest);
    test_mob.draw_mob(pl, test_mob); // рисуем моба
    test_mob = test_mob.move_bot(pl, test_mob, action); // поведение бота 



  
    pl = pl.movement(pl, action);
  


    if(pl.x == quest.x && pl.y == quest.y) { 
      quest = start_quest(rows, cols);
    }
    if(action == 32){ // если нажали клавишу атаки(пробел)
      test_mob.hp - pl.dmg;

      if(test_mob.hp <= 0){ // если боту нанесли критический урон
        test_mob.is_alive = false;
      }
    }
   
  } while((action = getch()) != 27); // 27 - escape - leave from cycle
    






    endwin(); // end curses
    return 0;
}