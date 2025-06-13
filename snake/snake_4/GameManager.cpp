#include "GameManager.h"
#include "Map.h"
#include "Snake.h"
#include <ncurses.h>
#include <unistd.h>
#include <random>
#include <ctime>
#include <iostream>

void GameManager::run() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);

    map.loadStage(1); // ✅ 지역 변수 제거했으므로 멤버 map 사용

    Snake snake;
    snake.init(10, 10);

    const int tick_ms = 150 * 1000;
    int frame = 0;

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
                default:        inputDir = currentDir; break;
            }

            if (inputDir != currentDir) {
                if (!snake.updateDirection(ch)) break;
            }
        }

        if (!snake.move(map)) break;

        if (frame % 100 == 0) {
            map.clearItems();
            map.addItem(GROWTH_ITEM);
            map.addItem(POISON_ITEM);

            generateGates(); 
            snake.setGateInfo(gate1, gate2); 
        }

        refresh();
        usleep(tick_ms);
        frame++;
    }

    endwin();
    printf("Game Over\n");
}

void GameManager::generateGates() {
    auto walls = map.getWallPositions();
    if (walls.size() < 2) return;

    std::srand(std::time(nullptr));
    int i = rand() % walls.size();
    int j;
    do {
        j = rand() % walls.size();
    } while (j == i);

    gate1 = walls[i];
    gate2 = walls[j];

    map.setValue(gate1.first, gate1.second, GATE); // 예: 7은 게이트 표시용
    map.setValue(gate2.first, gate2.second, GATE);

}

