#pragma once
#include <deque>
#include <utility>
#include "Map.h"

enum Direction { UP = 0, DOWN, LEFT, RIGHT };

class Snake {
private:
    std::deque<std::pair<int, int>> body;
    Direction direction;

    std::pair<int, int> gate1, gate2;  // ✅ 게이트 좌표
    std::pair<int, int> getGateExitPosition(const Map& map, std::pair<int, int> gate);  // ✅ 출구 계산

public:
    void init(int y, int x);
    void render() const;
    bool move(Map& map);
    bool updateDirection(int key);
    Direction getDirection() const;
    void setGateInfo(std::pair<int, int> g1, std::pair<int, int> g2);  // ✅ 게이트 정보 설정
};

