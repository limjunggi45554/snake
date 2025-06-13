#include "GameManager.h"
#include "Map.h"
#include "Snake.h"
#include <ncurses.h>
#include <unistd.h>

void GameManager::run() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);

    Map map;
    map.loadStage(1);

    Snake snake;
    snake.init(10, 10);

    const int tick_ms = 150 * 1000;

    while (true) {
        clear();
        map.render();
        snake.render();

        int ch = getch();
        if (ch != ERR) {
            Direction currentDir = snake.getDirection();
            Direction inputDir;
            switch (ch) {
                case KEY_UP:    inputDir = UP; break;
                case KEY_DOWN:  inputDir = DOWN; break;
                case KEY_LEFT:  inputDir = LEFT; break;
                case KEY_RIGHT: inputDir = RIGHT; break;
                default: inputDir = currentDir; break;
            }

            if (inputDir != currentDir) {
                if (!snake.updateDirection(ch)) break;  // 반대 방향 → 종료
            }
        }

        if (!snake.move(map)) break;  // 충돌 → 종료

        refresh();
        usleep(tick_ms);
    }

    endwin();
    printf("Game Over\n");
}

