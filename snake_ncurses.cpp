#include <ncurses.h>
#include <vector>
#include <chrono>
#include <thread>

const int WIDTH = 21;
const int HEIGHT = 21;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point { int x, y; };

int main() {
    // 초기화
    initscr();            // ncurses 시작
    cbreak();             // 라인 버퍼링 비활성
    noecho();             // 입력 키 보여주지 않음
    keypad(stdscr, TRUE); // 방향키 인식
    nodelay(stdscr, TRUE);// getch() 논블로킹
    curs_set(0);          // 커서 숨김

    // 초기 뱀 위치 및 방향
    Point head = { WIDTH/2, HEIGHT/2 };
    Direction dir = RIGHT;

    bool running = true;
    while (running) {
        // 입력 처리
        int ch = getch();
        switch (ch) {
            case KEY_UP:    if (dir != DOWN)  dir = UP;    break;
            case KEY_DOWN:  if (dir != UP)    dir = DOWN;  break;
            case KEY_LEFT:  if (dir != RIGHT) dir = LEFT;  break;
            case KEY_RIGHT: if (dir != LEFT)  dir = RIGHT; break;
            case 'q': running = false;                  break;
        }

        // 지도 그리기
        clear();
        for (int x = 0; x < WIDTH; ++x) {
            mvaddch(0, x, '#');
            mvaddch(HEIGHT-1, x, '#');
        }
        for (int y = 0; y < HEIGHT; ++y) {
            mvaddch(y, 0, '#');
            mvaddch(y, WIDTH-1, '#');
        }

        // 뱀 이동
        switch (dir) {
            case UP:    head.y = (head.y > 1) ? head.y - 1 : HEIGHT-2; break;
            case DOWN:  head.y = (head.y < HEIGHT-2) ? head.y + 1 : 1; break;
            case LEFT:  head.x = (head.x > 1) ? head.x - 1 : WIDTH-2; break;
            case RIGHT: head.x = (head.x < WIDTH-2) ? head.x + 1 : 1; break;
        }

        // 뱀 그리기 (머리만)
        mvaddch(head.y, head.x, 'O');

        // 화면 갱신
        refresh();

        // 속도 조절
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 종료 처리
    endwin();
    return 0;
}
