#include <ncurses.h>
#include <stdlib.h>
#include <vector>
using namespace std;

struct Wall{
  int x;
  int y;
  Wall(int a, int b): x(a), y(b){}
};

vector<Wall> walls;

void make_wall(){

  int wall_x = 20;
  int wall_y = 20;
  for(; wall_x < 71; wall_x++){
    walls.push_back(Wall(wall_x, wall_y));
  }
}

int main()
{
  make_wall();
  WINDOW* GameWindow;

  initscr();
  noecho();
  curs_set(0);


  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK); //배경
  init_pair(5, COLOR_WHITE,COLOR_WHITE); // normal wall
  init_pair(8, COLOR_RED, COLOR_RED); // immune wall
  refresh(); 

  GameWindow = newwin(40, 90, 5, 5); // 맵 크기 40 x 90, (5,5)에서 시작 
  wbkgd(GameWindow, COLOR_PAIR(1)); // 배경 색 적용
  wattron(GameWindow, COLOR_PAIR(1)); // 배경 색 적용
  // 왼, 오, 상, 하, 모서리 4군데
  wborder(GameWindow, COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(5), COLOR_PAIR(8), COLOR_PAIR(8), COLOR_PAIR(8), COLOR_PAIR(8));
  
  
  for(int i=0;i<walls.size();i++){
      mvwaddch(GameWindow, walls[i].y, walls[i].x, COLOR_PAIR(5)); // 미리 제작한 구조물 출력
  }
  wrefresh(GameWindow);

  getch();
  delwin(GameWindow);
  endwin();
  return 0;
}

// commit test