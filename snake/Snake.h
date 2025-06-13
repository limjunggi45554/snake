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
    bool updateDirection(int input);  // return false if fail
    bool move(Map& map);  // 이동, 실패하면 false 반환
    void render() const;
};


