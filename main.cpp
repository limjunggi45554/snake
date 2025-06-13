#include <ncurses.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <deque>

#define DELAY 100000  // 0.1초 대기

// 맵 파일 불러오기
std::vector<std::string> loadMap(const std::string& filename) {
    std::ifstream fin(filename);
    std::vector<std::string> map;
    std::string line;
    while (std::getline(fin, line)) {
        map.push_back(line);
    }
    return map;
}

int main() {
    std::vector<std::string> map = loadMap("map1.txt");
    int map_rows = map.size();
    int map_cols = map[0].size();

    // ncurses 초기화
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(FALSE);
    start_color();

    // 주황색 사용자 정의 (10번)
    if (can_change_color()) {
        init_color(10, 1000, 500, 0);  // R=1000, G=500, B=0
    }

    // 색상쌍 정의
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);  // 머리
    init_pair(2, 10, COLOR_BLACK);            // 꼬리

    // 초기 뱀 위치 (머리 포함 3칸)
    std::deque<std::pair<int, int>> snake;
    snake.push_back({5, 5});  // 머리
    snake.push_back({5, 4});
    snake.push_back({5, 3});

    int dir_x = 1, dir_y = 0;  // 초기 방향: 오른쪽
    int ch;

    while (true) {
        clear();

        // 맵 출력
        for (int r = 0; r < map_rows; ++r)
            mvprintw(r, 0, "%s", map[r].c_str());

        // 뱀 출력 (머리/꼬리 색 다르게)
        for (size_t i = 0; i < snake.size(); ++i) {
            int y = snake[i].first;
            int x = snake[i].second;

            if (i == 0) { // 머리
                attron(COLOR_PAIR(1));
                mvprintw(y, x, "O");
                attroff(COLOR_PAIR(1));
            } else { // 꼬리
                attron(COLOR_PAIR(2));
                mvprintw(y, x, "o");
                attroff(COLOR_PAIR(2));
            }
        }

        refresh();

        // 키 입력 받기
        ch = getch();
        if (ch != ERR) {
            switch (ch) {
                case KEY_UP:    dir_y = -1; dir_x = 0; break;
                case KEY_DOWN:  dir_y = 1; dir_x = 0; break;
                case KEY_LEFT:  dir_y = 0; dir_x = -1; break;
                case KEY_RIGHT: dir_y = 0; dir_x = 1; break;
                case 'q':       endwin(); return 0;
            }

            // 다음 위치 계산
            int next_y = snake.front().first + dir_y;
            int next_x = snake.front().second + dir_x;

            // 충돌 검사 (벽이면 종료)
            if (next_y < 0 || next_y >= map_rows ||
                next_x < 0 || next_x >= map_cols ||
                map[next_y][next_x] == '#') {
                break;
            }

            // 이동: 머리에 새 위치 추가, 꼬리 제거
            snake.push_front({next_y, next_x});
            snake.pop_back();
        }

        usleep(DELAY);  // 너무 빠른 반복 방지 (원한다면 생략 가능)
    }

    // 종료 처리
    endwin();
    return 0;
}

