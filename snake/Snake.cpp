#include "Snake.h"
#include "Map.h"
#include <ncurses.h>

void Snake::init(int y, int x) {
    body.clear();
    body.push_back({y, x});
    dir = RIGHT;
}

void Snake::updateDirection(int input) {
    Direction newDir = dir;
    switch (input) {
        case KEY_UP:    newDir = UP; break;
        case KEY_DOWN:  newDir = DOWN; break;
        case KEY_LEFT:  newDir = LEFT; break;
        case KEY_RIGHT: newDir = RIGHT; break;
    }

    // 반대 방향이면 무시
    if ((dir == UP && newDir == DOWN) ||
        (dir == DOWN && newDir == UP) ||
        (dir == LEFT && newDir == RIGHT) ||
        (dir == RIGHT && newDir == LEFT)) return;

    dir = newDir;
}

bool Snake::move(Map& map) {
    auto [y, x] = body.front();
    switch (dir) {
        case UP:    y--; break;
        case DOWN:  y++; break;
        case LEFT:  x--; break;
        case RIGHT: x++; break;
    }

    // 충돌 체크
    if (map.getValue(y, x) == 1 || map.getValue(y, x) == 2)
        return false;

    body.push_front({y, x});
    body.pop_back();  // 기본: 길이 유지

    return true;
}

void Snake::render() const {
    for (size_t i = 0; i < body.size(); ++i) {
        int y = body[i].first, x = body[i].second;
        mvprintw(y, x, i == 0 ? "O" : "o");  // Head, Body
    }
}

