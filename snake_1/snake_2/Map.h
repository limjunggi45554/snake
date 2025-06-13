#pragma once
#include <vector>
enum CellType {
    EMPTY = 0,
    WALL = 1,
    IMMUNE_WALL = 2,
    SNAKE_HEAD = 3,
    SNAKE_BODY = 4,
    GROWTH_ITEM = 5,
    POISON_ITEM = 6,
    GATE = 7
};

class Map {
private:
    std::vector<std::vector<int>> mapData;
    int height, width;

public:
    void setValue(int y, int x, int value);
    void loadStage(int stage);
    void render();
    int getValue(int y, int x) const;

    void addItem(int type); // type: 3(GROWTH), 4(POISON)
    void clearItems();      // 아이템 제거
};

