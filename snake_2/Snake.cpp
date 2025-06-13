#include "Snake.h"
#include "Map.h"
#include <ncurses.h>

const int dy[4] = {-1, 1, 0, 0}; // UP, DOWN
const int dx[4] = {0, 0, -1, 1}; // LEFT, RIGHT

void Snake::init(int y, int x) {
    body.clear();
    body.push_back({y, x});     // Head
    body.push_back({y, x - 1}); // Body1
    body.push_back({y, x - 2}); // Body2
    direction = RIGHT;
}


void Snake::render() const {
    for (size_t i = 0; i < body.size(); ++i) {
        int y = body[i].first;
        int x = body[i].second;
        if (i == 0)
            mvprintw(y, x, "O");  // Head
        else
            mvprintw(y, x, "o");  // Body
    }
}

bool Snake::move(Map& map) {
    auto [headY, headX] = body.front();
    int newY = headY + dy[direction];
    int newX = headX + dx[direction];

    int cell = map.getValue(newY, newX);

    if (cell == WALL || cell == IMMUNE_WALL || cell == SNAKE_BODY)
        return false;

    body.push_front({newY, newX});

    if (cell == GROWTH_ITEM) {
        map.setValue(newY, newX, EMPTY); // grow (no tail removal)
    } else if (cell == POISON_ITEM) {
        map.setValue(newY, newX, EMPTY);
        if (body.size() > 1) body.pop_back();
        if (body.size() > 1) body.pop_back();
    } else {
        body.pop_back(); // normal move
    }

    // 길이가 3보다 작아지면 게임 종료
    if (body.size() < 3)
        return false;

    return true;
}


bool Snake::updateDirection(int key) {
    Direction newDir = direction;
    switch (key) {
        case KEY_UP:    newDir = UP; break;
        case KEY_DOWN:  newDir = DOWN; break;
        case KEY_LEFT:  newDir = LEFT; break;
        case KEY_RIGHT: newDir = RIGHT; break;
        default: return true;
    }

    if ((direction == UP && newDir == DOWN) ||
        (direction == DOWN && newDir == UP) ||
        (direction == LEFT && newDir == RIGHT) ||
        (direction == RIGHT && newDir == LEFT)) {
        return false;
    }

    direction = newDir;
    return true;
}

Direction Snake::getDirection() const {
    return direction;
}

