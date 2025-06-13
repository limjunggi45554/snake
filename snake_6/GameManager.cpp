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

    map.loadStage(1);

    Snake snake;
    snake.init(10, 10);

    startTime = time(nullptr);

    const int tick_ms = 150 * 1000;
    int frame = 0;
    map.addItem(GROWTH_ITEM);
    map.addItem(POISON_ITEM);
    generateGates();
    snake.setGateInfo(gate1, gate2);
    while (true) {
        clear();
        map.render();
        snake.render();
        renderScoreBoard(snake);

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

        Snake::MoveResult result = snake.move(map);

        switch (result) {
            case Snake::MOVE_DEAD:
                endwin();
                printf("Game Over\n");
                return;
            case Snake::MOVE_GROWTH:
                growthCount++;
                lastGrowthItemTime = time(nullptr);  // ✅ 먹은 시점 기록
                break;
            case Snake::MOVE_POISON:
                poisonCount++;
                break;
            case Snake::MOVE_GATE:
                gateUseCount++;
                lastPoisonItemTime = time(nullptr);  // ✅ 먹은 시점 기록
                break;
            default:
                break;
        }

        if (checkMissionClear(snake)) {
            mvprintw(15, 25, "Mission Completed!");
            refresh();
            sleep(2);
            break;
        }

        time_t now = time(nullptr);
        if (now - lastGrowthItemTime >= 10) {
	    map.clearItem(GROWTH_ITEM);
	    map.addItem(GROWTH_ITEM);
	    lastGrowthItemTime = now;
	}

	if (now - lastPoisonItemTime >= 10) {
	    map.clearItem(POISON_ITEM);
	    map.addItem(POISON_ITEM);
	    lastPoisonItemTime = now;
	}


        refresh();
        usleep(tick_ms);
        frame++;
        checkMissionClear(snake);
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

void GameManager::renderScoreBoard(const Snake& snake) const {
    int offsetX = 25;  // 맵 오른쪽 옆에 출력

    int currentLen = snake.getLength();

    mvprintw(1, offsetX, "Score Board");
    mvprintw(2, offsetX, "B: %d / %d", currentLen, maxLength);
    mvprintw(3, offsetX, "+: %d", growthCount);
    mvprintw(4, offsetX, "-: %d", poisonCount);
    mvprintw(5, offsetX, "G: %d", gateUseCount);

    mvprintw(7, offsetX, "Mission");
    mvprintw(9, offsetX, "+: 3 (%s)", (growthCount >= 3 ? "v" : " "));
    mvprintw(10, offsetX, "-: 2 (%s)", (poisonCount >= 2 ? "v" : " "));
    mvprintw(11, offsetX, "G: 1 (%s)", (gateUseCount >= 1 ? "v" : " "));
}
bool GameManager::checkMissionClear(const Snake& snake) const {
    return growthCount >= 3 &&
           poisonCount >= 2 &&
           gateUseCount >= 1;
}


