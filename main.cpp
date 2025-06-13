#include <ncurses.h>
#include <unistd.h>  // usleep

#define DELAY 100000  // microseconds (0.1초)

int main() {
    int x = 10, y = 10;       // Snake 시작 위치
    int max_x, max_y;         // 화면 크기
    int ch;                   // 입력값

    initscr();                // ncurses 초기화
    noecho();                 // 입력 문자 출력 안 함
    cbreak();                 // 버퍼 없이 입력
    keypad(stdscr, TRUE);     // 방향키 입력 활성화
    nodelay(stdscr, TRUE);    // getch() 블로킹 X
    curs_set(FALSE);          // 커서 숨기기

    getmaxyx(stdscr, max_y, max_x);  // 화면 크기 가져오기

    while (true) {
        clear();            // 화면 초기화
        mvprintw(y, x, "O");  // 뱀의 머리 출력
        refresh();

        ch = getch();       // 입력 받기

        switch (ch) {
            case KEY_UP:
                y--;
                break;
            case KEY_DOWN:
                y++;
                break;
            case KEY_LEFT:
                x--;
                break;
            case KEY_RIGHT:
                x++;
                break;
            case 'q':
                endwin();
                return 0;
        }

        // 화면 밖으로 나가는 거 방지
        if (x < 0) x = 0;
        if (x >= max_x) x = max_x - 1;
        if (y < 0) y = 0;
        if (y >= max_y) y = max_y - 1;

        usleep(DELAY);
    }

    endwin();  // ncurses 종료
    return 0;
}

