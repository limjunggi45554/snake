#pragma once
#include <deque>
#include <utility>
#include "Map.h"

enum Direction { UP = 0, DOWN, LEFT, RIGHT };

class Snake {
public:
    enum MoveResult {
        MOVE_NORMAL,
        MOVE_GROWTH,
        MOVE_POISON,
        MOVE_GATE,
        MOVE_DEAD
    };

private:
    std::deque<std::pair<int, int>> body;
    Direction direction;
    std::pair<int, int> gate1, gate2;

public:
    void init(int y, int x);
    void render() const;
    MoveResult move(Map& map);  // 🔁 바뀐 시그니처
    bool updateDirection(int key);
    Direction getDirection() const;
    void setGateInfo(std::pair<int, int> g1, std::pair<int, int> g2);
    std::pair<int, int> getGateExitPosition(const Map& map, std::pair<int, int> gate);
    int getLength() const;
};

