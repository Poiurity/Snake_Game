#include <ncurses.h>
#include <stdlib.h>
#include <vector>
#include<deque>
using namespace std;

// 자주 사용되는 설정값 상수로 define
#define width_game_Display 50
#define height_game_Display 80
#define X_game_Display 1
#define Y_game_Display 1

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//구조체, 클래스 선언 공간
struct Wall{
  int x, y;
  Wall(int v1, int v2): x(v1), y(v2){}
};
struct Snake{
  int x, y;
  Snake(int v1, int v2): x(v1), y(v2){}
};

//전역변수 선언 공간
vector<Wall> data_Wall;
int cur_Stage = 1;

//함수 선언 공간
void draw_Wall(){
  switch (cur_Stage) {
    case 1: {
      int tmp_x = 20;
      int tmp_y = 20;
      for(; tmp_x < 71; tmp_x++){
        data_Wall.push_back(Wall(tmp_x, tmp_y));
      }
      break;
    }
    case 2: {
      break;
    }
    case 3: {
      break;
    }
    case 4: {
      break;
    }
  }
}

int main()
{
  // ncurses 관련 공간
  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, true);
  timeout(200);

  // color pair 설정 공간
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK); //배경
  init_pair(5, COLOR_WHITE,COLOR_WHITE); // normal wall
  init_pair(8, COLOR_RED, COLOR_RED); // immune wall
  init_pair(3, COLOR_YELLOW, COLOR_YELLOW); //snake head
  init_pair(4, COLOR_GREEN,COLOR_GREEN); //snake body
  refresh(); 

  // 변수 선언 공간
  WINDOW* game_Display;
  deque<Snake> data_Snake;

  bool stop = false;
  bool err_Dir = false;

  int cur_Dir = RIGHT; 
  int in_Key; 
  int len_Snake = 3;

  // 함수 실행 공간
  draw_Wall();

  // 초기값 설정 공간
  for(int i=1; i < len_Snake+1; i++)
    data_Snake.push_front(Snake(X_game_Display+i, Y_game_Display+1));

  // while loop 진입
  while(!stop){
    // 변수 선언 및 초기화
    Snake head = data_Snake.front();
    int head_x = head.x;
    int head_y = head.y;

    in_Key = getch();
    switch (in_Key) {
      case KEY_LEFT:
        if(cur_Dir == RIGHT) {err_Dir = true;} 
        else {cur_Dir = LEFT;} 
        break;
      case KEY_RIGHT:
        if(cur_Dir == LEFT) {err_Dir = true;} 
        else {cur_Dir = RIGHT;} 
        break;
      case KEY_UP:
        if(cur_Dir == DOWN) {err_Dir = true;} 
        else {cur_Dir = UP;} 
        break;
      case KEY_DOWN:
        if(cur_Dir == UP) {err_Dir = true;} 
        else {cur_Dir = DOWN;} 
        break;
    }

    /* 게임이 종료될 조건 체크 */

    // 진행 방향과 반대 방향으로 입력이 들어왔는지 체크
    if(err_Dir) stop = true; 
    // 몸통과 접촉하는지 체크
    for(Snake body : data_Snake){
        if(head.x == body.x && head.y == body.y) {
          //stop = true;
          break;
      }
    } 

    /* 스네이크 위치 최신화 */

    // 입력키에 따라 스네이크의 head 이동시키기
    if(cur_Dir == UP) {head_y--;}
    else if(cur_Dir == DOWN) {head_y++;}
    else if(cur_Dir == RIGHT) {head_x++;}
    else if(cur_Dir == LEFT) {head_x--;}
    // head의 새로운 위치를 삽입하고 꼬리부분 삭제, 스네이크를 실제로 이동시키는 코드
    data_Snake.push_front(Snake(head_x, head_y)); 
    data_Snake.pop_back();

    /* 출력 코드 */

    //맵 배경, 경계선 출력
    game_Display = newwin(width_game_Display, height_game_Display,  // 맵 크기 50 x 80, (1,1)에서 시작 
                          X_game_Display, Y_game_Display);
    wbkgd(game_Display, COLOR_PAIR(1)); 
    //wattron(game_Display, COLOR_PAIR(1)); 
    wborder(game_Display, COLOR_PAIR(4), COLOR_PAIR(4), COLOR_PAIR(5), COLOR_PAIR(5), // 왼,오,상,하 : color pair 5번
                          'X', 'X', 'X', 'X'); // 모서리 : color pair 8번
    
    // 맵 구조물 출력
    for(int i=0;i < data_Wall.size();i++){
        mvwaddch(game_Display, data_Wall[i].y, data_Wall[i].x, COLOR_PAIR(5));
    }
    wrefresh(game_Display);

    // 스네이크 출력
    bool isHead = true;
    for(Snake i : data_Snake){
      if(isHead){
          mvwaddch(game_Display, i.y, i.x, COLOR_PAIR(3)); // snakes의 첫 원소 = head의 좌표, head_idx는 첫 번째 좌표인지 아닌지를 구분하기 위해서만 사용됨
          isHead = false;
          continue;
      }
      mvwaddch(game_Display, i.y, i.x, COLOR_PAIR(4)); // body 부분 출력
    }
    wrefresh(game_Display);
  }

  getch();
  delwin(game_Display);
  endwin();
  return 0;
}
// commit test