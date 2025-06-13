#include "Snake.h"
#include "Map.h"
#include <ncurses.h>
#include <tuple>
#include <deque>

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

Snake::MoveResult Snake::move(Map& map) {
    auto [headY, headX] = body.front();
    int newY = headY + dy[direction];
    int newX = headX + dx[direction];

    int cell = map.getValue(newY, newX);
    bool usedGate = false;

    if (cell == GATE) {
        std::pair<int, int> outGate = (newY == gate1.first && newX == gate1.second) ? gate2 : gate1;
        std::tie(newY, newX) = getGateExitPosition(map, outGate);
        usedGate = true;
    }

    for (const auto& segment : body) {
        if (segment.first == newY && segment.second == newX)
            return MOVE_DEAD;
    }

    if (map.getValue(newY, newX) == WALL || map.getValue(newY, newX) == IMMUNE_WALL)
        return MOVE_DEAD;

    body.push_front({newY, newX});

    if (cell == GROWTH_ITEM) {
        map.setValue(newY, newX, EMPTY);
        return MOVE_GROWTH;
    } else if (cell == POISON_ITEM) {
        map.setValue(newY, newX, EMPTY);
        if (body.size() > 1) body.pop_back();
        if (body.size() > 1) body.pop_back();
        if (body.size() < 3)
            return MOVE_DEAD;
        return MOVE_POISON;
    } else {
        body.pop_back();
    }

    if (body.size() < 3)
        return MOVE_DEAD;

    if (usedGate)
        return MOVE_GATE;

    return MOVE_NORMAL;
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
void Snake::setGateInfo(std::pair<int, int> g1, std::pair<int, int> g2) {
    gate1 = g1;
    gate2 = g2;
}


std::pair<int, int> Snake::getGateExitPosition(const Map& map, std::pair<int, int> gate) {
    int y = gate.first;
    int x = gate.second;

    Direction priority[4];

    switch (direction) {
        case UP:
            priority[0] = UP;
            priority[1] = RIGHT;
            priority[2] = LEFT;
            priority[3] = DOWN;
            break;
        case DOWN:
            priority[0] = DOWN;
            priority[1] = LEFT;
            priority[2] = RIGHT;
            priority[3] = UP;
            break;
        case LEFT:
            priority[0] = LEFT;
            priority[1] = UP;
            priority[2] = DOWN;
            priority[3] = RIGHT;
            break;
        case RIGHT:
            priority[0] = RIGHT;
            priority[1] = DOWN;
            priority[2] = UP;
            priority[3] = LEFT;
            break;
    }

    for (int i = 0; i < 4; ++i) {
        int dir = priority[i];
        int ny = y + dy[dir];
        int nx = x + dx[dir];

        int val = map.getValue(ny, nx);
        if (val == EMPTY || val == GROWTH_ITEM || val == POISON_ITEM) {
            direction = static_cast<Direction>(dir);  // ✅ 새로운 이동 방향 설정
            return {ny, nx};  // ✅ 게이트 탈출 위치
        }
    }

    return gate;  // 이동 불가하면 제자리
}
int Snake::getLength() const {
    return body.size();
}
const std::deque<std::pair<int,int>>& Snake::getBody() const {
    return body;
}

