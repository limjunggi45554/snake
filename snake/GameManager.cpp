#include "GameManager.h"
#include "Map.h"
#include "Snake.h"
#include <ncurses.h>
#include <unistd.h>  // usleep
#include <iostream>
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

    const int tick_ms = 300000;  // 300ms

    while (true) {
        clear();
        map.render();
        snake.render();

        int ch = getch();
        if (ch != ERR) snake.updateDirection(ch);

        if (!snake.move(map)) break;

        refresh();
        usleep(tick_ms);
    }

    // 종료 메시지 표시 및 대기
    clear();
    mvprintw(10, 10, "Game Over! Press any key to exit.");
    refresh();
    nodelay(stdscr, FALSE);  // 입력 대기 모드
    getch();
    endwin();
}


