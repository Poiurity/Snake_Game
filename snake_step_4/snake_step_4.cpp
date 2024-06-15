#include <ncurses.h>
#include <stdlib.h>
#include <vector>
#include<deque>
#include <ctime>
#include <chrono>
#include <unordered_set>
using namespace std;

// 자주 사용되는 설정값 상수로 define
#define width_game_Display 80
#define height_game_Display 40
#define width_board_Display 10
#define height_board_Display 20
#define X_game_Display 1
#define Y_game_Display 1

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define init_Snake 3

//구조체, 클래스 선언 공간
struct Wall{
  int x, y;
  Wall(int v1, int v2): x(v1), y(v2){}
};
struct Snake{
  int x, y;
  Snake(int v1, int v2): x(v1), y(v2){}
};
enum ItemType {
    GROWTH, POISON
};
struct Item {
  int x, y;  // 좌표
  ItemType type;  // 종류
  chrono::system_clock::time_point spawn_Time;// 아이템 생성 시간
  Item(int v1, int v2, ItemType t) : x(v1), y(v2), type(t), spawn_Time(chrono::system_clock::now()) {}
};
struct Gate {
    int x, y;
    Gate(int v1, int v2): x(v1), y(v2){}
};

//전역변수 선언 공간
vector<Wall> data_Wall;
vector<Item> data_Item;
vector<Gate> data_Gate;
int cur_Stage = 1;

//함수 선언 공간
void set_Snake(deque<Snake> &data){
  for(int i=1; i < init_Snake+1; i++){
    data.push_front(Snake(X_game_Display+i, Y_game_Display+1));
  }
}

inline void afterCrash(bool &setStop){
  setStop = true;
}

inline bool crash_Check(int x, int y){
  for(Wall val : data_Wall){
    bool is_gate = false;
    for (Gate gate : data_Gate) {
      if (x == gate.x && y == gate.y) {
        is_gate = true;
        break;
      }
    }
    if (!is_gate && x == val.x && y == val.y){
      return true;
    }
  }
  return false;
}

//아이템 생성
inline void generate_Item() {
  int itemType = rand() % 2;
  int x, y;

  do {
    x = rand() % (width_game_Display - 2) + 1;
    y = rand() % (height_game_Display - 2) + 1;
  } while (crash_Check(x, y));
  

  ItemType type = (rand() % 2 == 0) ? GROWTH : POISON;   // 아이템 타입 설정

  // 아이템 생성 후 현재 시간 저장
  Item item(x, y, type);
  item.spawn_Time = chrono::system_clock::now();

  
  data_Item.push_back(Item(x, y, (itemType == 0) ? GROWTH : POISON));

}

// 아이템 획득 -> type에 따라 tail 조정
inline void get_Item(deque<Snake> &data_Snake, Item &item, int &cnt_grwItems, int &cnt_psnItems) {
  if (item.type == GROWTH) {
    Snake tail = data_Snake.back();
    data_Snake.push_back(tail);
    cnt_grwItems++;
  } else if (item.type == POISON) {
    data_Snake.pop_back();
    cnt_psnItems++;
  }
}

// 게이트 생성
void draw_Gate() {
    data_Gate.clear();
    int gate_count = 0;
    std::unordered_set<int> selected_indices;  // 중복 인덱스 추적

    while (gate_count < 2) {
        int gate_index = rand() % data_Wall.size();
        
        // 중복 인덱스가 아닌 경우에만 실행
        if (selected_indices.find(gate_index) == selected_indices.end()) {
            Wall selected_wall = data_Wall[gate_index];

            // 벽이 가장자리 모서리에 위치하지 않는지 확인
            if (!((selected_wall.x == 0 || selected_wall.x == width_game_Display - 1) && 
                  (selected_wall.y == 0 || selected_wall.y == height_game_Display - 1))) {

                data_Gate.push_back(Gate(selected_wall.x, selected_wall.y));
                selected_indices.insert(gate_index);  // 선택된 인덱스 기록
                gate_count++;
            }
        }
    }
}


// 게이트 통과시 뱀 위치 이동
void teleport_Snake(deque<Snake> &data_Snake, int &head_x, int &head_y, int &cur_Dir, int &cnt_gatePass) {
    if (data_Gate.size() != 2) return; 
    Gate gate_in = data_Gate[0];
    Gate gate_out = data_Gate[1];

    if (head_x == gate_in.x && head_y == gate_in.y) {
        head_x = gate_out.x;
        head_y = gate_out.y;
        cnt_gatePass++; // gate 통과 횟수 증가
        draw_Gate(); // 새로운 gate 생성
    } else if (head_x == gate_out.x && head_y == gate_out.y) {
        head_x = gate_in.x;
        head_y = gate_in.y;
        cnt_gatePass++; // gate 통과 횟수 증가
        draw_Gate(); // 새로운 gate 생성
    }

    if (head_x == 0) cur_Dir = RIGHT;
    else if (head_x == width_game_Display - 1) cur_Dir = LEFT;
    else if (head_y == 0) cur_Dir = DOWN;
    else if (head_y == height_game_Display - 1) cur_Dir = UP;
}


void map_1();
void map_2();
void map_3();
void map_4();

void draw_Wall() {
    switch (cur_Stage) {
        case 1:
        case 2:
        case 3:
        case 4: {
            data_Wall.clear();
            // 상단과 하단 벽 생성
            for (int i = 0; i < width_game_Display; i++) {
                data_Wall.push_back(Wall(i, 0));
                data_Wall.push_back(Wall(i, height_game_Display - 1));
            }
            // 좌측과 우측 벽 생성
            for (int i = 0; i < height_game_Display; i++) {
                data_Wall.push_back(Wall(0, i));
                data_Wall.push_back(Wall(width_game_Display - 2, i));
            }

            // 맵 생성 함수 호출
            if (cur_Stage == 1) {
                map_1();
            } else if (cur_Stage == 2) {
                map_2();
            } else if (cur_Stage == 3) {
                map_3();
            } else if (cur_Stage == 4) {
                map_4();
            }

            cur_Stage++;
            break;
        }
        default: {
            cur_Stage = 1;
            draw_Wall();
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
  timeout(300);

  start_color();

  init_pair(1, COLOR_WHITE, COLOR_BLACK); //배경
  init_pair(5, COLOR_WHITE,COLOR_WHITE); // normal wall
  init_pair(8, COLOR_CYAN, COLOR_CYAN); // immune wall *poison 아이템과 구별을 위해 cyan으로 변경함. 
  init_pair(3, COLOR_YELLOW, COLOR_YELLOW); //snake head
  init_pair(4, COLOR_GREEN,COLOR_GREEN); //snake body
  init_pair(6, COLOR_BLUE, COLOR_BLUE); // growth item, blue
  init_pair(7, COLOR_RED, COLOR_RED); // poison item, red
  init_pair(9, COLOR_GREEN,COLOR_GREEN); // gate
  init_pair(2, COLOR_BLACK, COLOR_BLUE); // score board, mission board

  refresh(); 

  // 변수 선언 공간
  WINDOW* game_Display;
  WINDOW* score_Display;
  WINDOW* mission_Display;
  deque<Snake> data_Snake;
  
  time_t end_T;
  time_t cur_T;

  bool stop = false;
  bool err_Dir = false;
  bool isHead = true;

  int cur_Dir = RIGHT; 
  int in_Key; 

  int cnt_grwItems = 0;
  int cnt_psnItems = 0;
  int cnt_gatePass = 0;
  int cnt_Item = 0;
  int length_Goal = 5; // 목표 길이 초기값
  int grw_Goal = 3; // Growth Item 목표 초기값
  int psn_Goal = 1; // Poison Item 목표 초기값
  int gpss_Goal = 1; // Gate Pass 목표 초기값
  srand(time(NULL));  // 아이템 생성을 위한 난수 시드 설정


  // 함수 실행공간
  draw_Wall();
  set_Snake(data_Snake);
  draw_Gate();

  // while loop 진입
  while(!stop){
    //현재시간 측정, 미션 클리어한 시간(end_T)과 비교해 1초간 delay 발생
    time(&cur_T);
    if(cur_T - end_T < 2){
      in_Key = 0; // delay 되는동안 사용자 입력 비활성화
      erase();
      continue;
    }
    // 변수 선언 및 초기화
    Snake head = data_Snake.front();
    int head_x = head.x;
    int head_y = head.y;
    //키 입력받기
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
    isHead = true;
    for(Snake body : data_Snake){
        if(isHead) {
          isHead = false;
          continue;
        }
        else if (head_x == body.x && head_y == body.y){
          stop = true;
          break;
        }
    }
    // 구조물(테두리 벽, 추가된 벽)과 충돌하는지 체크
    if(crash_Check(head_x, head_y)) {break;/*afterCrash(stop);*/} 
    // 전체 길이가 3 미만이면 종료
    if(data_Snake.size() < 3) stop = true;
    //모든 스테이지 클리어 하거나 제한 길이 넘어서면 종료
    if(cur_Stage >= 5 || data_Snake.size() >= length_Goal+2){stop = true;}
    //모든 목표에 도달하면 다음 스테이지로 이동
    else if(data_Snake.size() >= length_Goal && cnt_grwItems >= grw_Goal && cnt_psnItems >= psn_Goal && cnt_gatePass >= gpss_Goal){
      time(&end_T);

      data_Snake.clear();
      data_Wall.clear();

      set_Snake(data_Snake);
      cur_Dir = RIGHT;
      draw_Wall();

      grw_Goal += 2;
      psn_Goal += 1;
      length_Goal += 2;
      gpss_Goal += 1;

      cnt_grwItems = 0;
      cnt_psnItems = 0;
      cnt_gatePass = 0;
      continue;
    }

    /* 아이템 생성 */
    if (data_Item.size() < 3 && rand() % 100 < 50) {  // 약 50% 확률로 아이템 생성
      generate_Item();
    }

    /* 스네이크와 아이템 상호작용 */
    // 아이템과 스네이크가 만났는지 체크
    for (auto it = data_Item.begin(); it != data_Item.end();) {
      if (head_x == it->x && head_y == it->y) {
        get_Item(data_Snake, *it, cnt_grwItems, cnt_psnItems);
        it = data_Item.erase(it);  // 아이템을 획득한 후 삭제
      } else {
        ++it;
      }
    }

    teleport_Snake(data_Snake, head_x, head_y, cur_Dir, cnt_gatePass);

    /* 스네이크 위치 최신화 */

    // 입력키에 따라 스네이크의 head 이동시키기
    if(cur_Dir == UP) {head_y--;}
    else if(cur_Dir == DOWN) {head_y++;}
    else if(cur_Dir == RIGHT) {head_x++;}
    else if(cur_Dir == LEFT) {head_x--;}
    // head의 새로운 위치를 삽입하고 꼬리부분 삭제, 스네이크를 실제로 이동시키는 코드
    data_Snake.push_front(Snake(head_x, head_y)); 
    data_Snake.pop_back();

    // 현재 시간 저장
    time_t now = time(NULL);

    /* 출력 코드 */

    //맵 배경, 경계선 출력
    game_Display = newwin(height_game_Display, width_game_Display,  
                          X_game_Display, Y_game_Display);
    wbkgd(game_Display, COLOR_PAIR(1));
    //wattron(game_Display, COLOR_PAIR(1)); 
    wborder(game_Display, 'o', 'o', 'o', 'o', // 왼,오,상,하 : 문자 'o'
                          'x', 'x', 'x', 'x'); // 모서리 : 문자 'X' 
    
    // 맵 구조물 출력
    for(int i=0;i < data_Wall.size();i++){
      bool is_gate = false;
      for (Gate gate : data_Gate) {
        if (data_Wall[i].x == gate.x && data_Wall[i].y == gate.y) {
            is_gate = true;
            break;
        }
      }
      if (is_gate) {
        mvwaddch(game_Display, data_Wall[i].y, data_Wall[i].x, COLOR_PAIR(9));
      } else {
        mvwaddch(game_Display, data_Wall[i].y, data_Wall[i].x, COLOR_PAIR(5));
      }
    }

    for (Gate gate : data_Gate) {
        mvwaddch(game_Display, gate.y, gate.x, COLOR_PAIR(9));
    }

    wrefresh(game_Display);

    // 아이템 출력 및 깜빡임 효과
    for (auto it = data_Item.begin(); it != data_Item.end();) {
        chrono::system_clock::time_point spawn_Time = it->spawn_Time;
        auto now = std::chrono::system_clock::now(); // 현재 시간 
        auto duration = chrono::duration_cast<chrono::seconds>(now - spawn_Time).count();


        if (duration >= 7.0) {
            it = data_Item.erase(it); // 7초가 지나면 아이템 삭제
        } 
        else {
            if (duration >= 5.0) { // 5초 경과 후 깜빡임 효과 시작
                auto blinking_Duration = duration - 5.0;
                // 5초 경과 후 0.5초 간격으로 깜빡임 효과
                if (((int)(blinking_Duration * 10) % 10) < 5) {
                    mvwaddch(game_Display, it->y, it->x, ' ' | (it->type == GROWTH ? COLOR_PAIR(6) : COLOR_PAIR(7)));
                }
            } 
            else {
                mvwaddch(game_Display, it->y, it->x, ' ' | (it->type == GROWTH ? COLOR_PAIR(6) : COLOR_PAIR(7)));
            }
        ++it;
        wrefresh(game_Display);
        }
    }


    // 스네이크 출력
    isHead = true;
    for(Snake i : data_Snake){
      if(isHead){
          mvwaddch(game_Display, i.y, i.x, COLOR_PAIR(3)); 
          isHead = false;
          continue;
      }
      mvwaddch(game_Display, i.y, i.x, COLOR_PAIR(4)); 
    }

    wrefresh(game_Display);

  //Score board 출력
  score_Display = newwin(width_board_Display, height_board_Display, Y_game_Display, width_game_Display + 5);
  wbkgd(score_Display, COLOR_PAIR(2));
  wattron(score_Display, COLOR_PAIR(2));
  //wborder(score_Display, '.', '.', '.', '.', '#', '#', '#', '#');
  mvwprintw(score_Display, 1, 1, "Score Board");
  mvwprintw(score_Display, 2, 1, "B : %d / %d", data_Snake.size(), length_Goal+2);
  mvwprintw(score_Display, 3, 1, "+ : (%d)", cnt_grwItems);
  mvwprintw(score_Display, 4, 1, "- : (%d)", cnt_psnItems);
  mvwprintw(score_Display, 5, 1, "G : (%d)", cnt_gatePass);
  wrefresh(score_Display);

  //Missin board 출력
  mission_Display = newwin(width_board_Display, height_board_Display, width_board_Display + 2, width_game_Display + 5);
  wbkgd(mission_Display, COLOR_PAIR(2));
  wattron(mission_Display, COLOR_PAIR(2));
  //wborder(mission_Display, '.', '.', '.', '.', '#', '#', '#', '#');
  mvwprintw(mission_Display, 1, 1, "Mission Board");
  mvwprintw(mission_Display, 2, 1, "B : %d (%c)", length_Goal,data_Snake.size() >= length_Goal?'v':' ');
  mvwprintw(mission_Display, 3, 1, "+ : %d (%c)", grw_Goal,cnt_grwItems >= grw_Goal?'v':' ');
  mvwprintw(mission_Display, 4, 1, "- : %d (%c)", psn_Goal,cnt_psnItems >= psn_Goal?'v':' ');
  mvwprintw(mission_Display, 5, 1, "G : %d (%c)", gpss_Goal,cnt_gatePass >= gpss_Goal?'v':' ');
  wrefresh(mission_Display);
  }

  getch();
  delwin(game_Display);
  endwin();
  return 0;
}

void map_1(){
  int tmp_x = width_game_Display / 5;
  int tmp_y = height_game_Display / 2;
  for (int i = tmp_x; i < tmp_x*4; i++)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }
}

void map_2(){
  int tmp_x = width_game_Display / 5;
  int tmp_y = height_game_Display / 4 * 3;
  for (int i = tmp_x; i < tmp_x*4; i++)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }

  tmp_x = width_game_Display / 5;
  tmp_y = height_game_Display / 4 * 1;
  for (int i = tmp_x; i < tmp_x*4; i++)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }
}

void map_3(){
  int tmp_x = width_game_Display / 5;
  int tmp_y = height_game_Display / 2;
  for (int i = tmp_x; i < tmp_x*4; i++)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }

  tmp_x = width_game_Display / 2;
  tmp_y = height_game_Display / 5;
  for (int i = tmp_y; i < tmp_y*4; i++)
  {
    data_Wall.push_back(Wall(tmp_x, i));
  }
}

void map_4(){
  // 1
  int tmp_x = width_game_Display / 7;
  int tmp_y = height_game_Display / 7;
  for (int i = tmp_x; i < (tmp_x*3 - 8); i++)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }
  tmp_x = width_game_Display / 7;
  tmp_y = height_game_Display / 7;
  for (int i = tmp_y; i < (tmp_y*3 - 1); i++)
  {
    data_Wall.push_back(Wall(tmp_x, i));
  }
  // 2
  tmp_x = width_game_Display / 7;
  tmp_y = height_game_Display / 7 * 6;
  for (int i = tmp_x; i < (tmp_x*3 - 8); i++)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }
  tmp_x = width_game_Display / 7;
  tmp_y = height_game_Display / 7 * 6;
  for (int i = tmp_y; i > (tmp_y/3 + 10); i--)
  {
    data_Wall.push_back(Wall(tmp_x, i));
  }
  //3
  tmp_x = width_game_Display / 7 * 6;
  tmp_y = height_game_Display / 7;
  for (int i = tmp_x; i > (tmp_x/3 + 16); i--)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }
  tmp_x = width_game_Display / 7 * 6;
  tmp_y = height_game_Display / 7;
  for (int i = tmp_y; i < (tmp_y*3 - 1); i++)
  {
    data_Wall.push_back(Wall(tmp_x, i));
  }
  // 4
  tmp_x = width_game_Display / 7 * 6;
  tmp_y = height_game_Display / 7 * 6;
  for (int i = tmp_x; i > (tmp_x/3 + 16); i--)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }
  tmp_x = width_game_Display / 7 * 6;
  tmp_y = height_game_Display / 7 * 6;
  for (int i = tmp_y; i > (tmp_y/3 + 10); i--)
  {
    data_Wall.push_back(Wall(tmp_x, i));
  }
  // 중앙 십자모양 벽
  tmp_x = width_game_Display / 3;
  tmp_y = height_game_Display / 2;
  for (int i = tmp_x; i < tmp_x*2; i++)
  {
    data_Wall.push_back(Wall(i, tmp_y));
  }

  tmp_x = width_game_Display / 2;
  tmp_y = height_game_Display / 3;
  for (int i = tmp_y; i < tmp_y*2; i++)
  {
    data_Wall.push_back(Wall(tmp_x, i));
  }
}
