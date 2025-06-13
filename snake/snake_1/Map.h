#pragma once
#include <vector>

class Map {
private:
    std::vector<std::vector<int>> mapData;

public:
    void loadStage(int stage);  // 맵 데이터 불러오기
    void render();              // ncurses로 출력
    int getValue(int y, int x);
};

