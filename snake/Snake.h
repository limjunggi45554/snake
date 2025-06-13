#pragma once
#include <deque>

class Map;

enum Direction { UP, DOWN, LEFT, RIGHT };

class Snake {
private:
    std::deque<std::pair<int, int>> body;
    Direction dir;

public:
    void init(int y, int x);
    bool updateDirection(int input);  // true if success, false if game over
    bool move(Map& map);
    void render() const;
    Direction getDirection() const;   // 현재 방향 getter
};

