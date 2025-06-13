#include "Snake.h"
#include "Map.h"
#include <ncurses.h>

void Snake::init(int y, int x) {
    body.clear();
    body.push_back({y, x});     // head
    body.push_back({y, x - 1}); // body
    body.push_back({y, x - 2}); // tail
    dir = RIGHT;
}

bool Snake::updateDirection(int input) {
    Direction newDir = dir;
    switch (input) {
        case KEY_UP:    newDir = UP; break;
        case KEY_DOWN:  newDir = DOWN; break;
        case KEY_LEFT:  newDir = LEFT; break;
        case KEY_RIGHT: newDir = RIGHT; break;
        default: return true;
    }

    // 반대 방향 → 게임 오버
    if ((dir == UP && newDir == DOWN) ||
        (dir == DOWN && newDir == UP) ||
        (dir == LEFT && newDir == RIGHT) ||
        (dir == RIGHT && newDir == LEFT)) {
        return false;
    }

    // 같은 방향이면 무시
    if (newDir == dir) return true;

    dir = newDir;
    return true;
}

bool Snake::move(Map& map) {
    auto [y, x] = body.front();
    switch (dir) {
        case UP:    y--; break;
        case DOWN:  y++; break;
        case LEFT:  x--; break;
        case RIGHT: x++; break;
    }

    if (map.getValue(y, x) == 1 || map.getValue(y, x) == 2)
        return false;

    for (const auto& part : body) {
        if (part.first == y && part.second == x)
            return false;
    }

    body.push_front({y, x});
    body.pop_back();
    return true;
}

void Snake::render() const {
    for (size_t i = 0; i < body.size(); ++i) {
        int y = body[i].first;
        int x = body[i].second;
        mvprintw(y, x, i == 0 ? "O" : "o");  // Head / Body
    }
}

Direction Snake::getDirection() const {
    return dir;
}

