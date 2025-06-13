#pragma once
#include <deque>
#include <utility>
#include "Map.h"

enum Direction { UP = 0, DOWN, LEFT, RIGHT };

class Snake {
private:
    std::deque<std::pair<int, int>> body;
    Direction direction;

public:
    void init(int y, int x);
    void render() const;
    bool move(Map& map);
    bool updateDirection(int key);
    Direction getDirection() const;
};

