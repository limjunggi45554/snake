#include "Snake.h"
#include "Map.h"
#include <ncurses.h>
#include <tuple>

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

    if (cell == GATE) {
        std::pair<int, int> outGate = (newY == gate1.first && newX == gate1.second) ? gate2 : gate1;
        std::tie(newY, newX) = getGateExitPosition(map, outGate);
    }

    for (const auto& segment : body) {
        if (segment.first == newY && segment.second == newX)
            return false;
    }

    if (map.getValue(newY, newX) == WALL || map.getValue(newY, newX) == IMMUNE_WALL)
        return false;

    body.push_front({newY, newX});

    if (cell == GROWTH_ITEM) {
        map.setValue(newY, newX, EMPTY);
        map.addItem(GROWTH_ITEM);
    } else if (cell == POISON_ITEM) {
        map.setValue(newY, newX, EMPTY);
        map.addItem(POISON_ITEM);
        if (body.size() > 1) body.pop_back();
        if (body.size() > 1) body.pop_back();
    } else {
        body.pop_back();
    }

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
void Snake::setGateInfo(std::pair<int, int> g1, std::pair<int, int> g2) {
    gate1 = g1;
    gate2 = g2;
}
std::pair<int, int> Snake::getGateExitPosition(const Map& map, std::pair<int, int> gate) {
    int y = gate.first;
    int x = gate.second;

    for (int dir = 0; dir < 4; ++dir) {
        int ny = y + dy[dir];
        int nx = x + dx[dir];

        int val = map.getValue(ny, nx);
        if (val == EMPTY || val == GROWTH_ITEM || val == POISON_ITEM) {
            direction = static_cast<Direction>(dir);  // 방향도 갱신
            return {ny, nx};
        }
    }

    // 만약 탈출 못하면 그대로
    return gate;
}

