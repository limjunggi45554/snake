#include "Map.h"
#include <ncurses.h>
#include <cstdlib>
#include <ctime>

void Map::loadStage(int stage) {
    mapData = {
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2}
    };

    srand(time(nullptr));  // 1회만 초기화
}

void Map::addItem(int type) {
    int y, x;
    for (int tries = 0; tries < 100; ++tries) {
        y = rand() % mapData.size();
        x = rand() % mapData[y].size();
        if (mapData[y][x] == EMPTY) {
            mapData[y][x] = type;
            break;
        }
    }
}

void Map::clearItems() {
    for (auto& row : mapData) {
        for (auto& cell : row) {
            if (cell == GROWTH_ITEM || cell == POISON_ITEM)
                cell = EMPTY;
        }
    }
}

void Map::render() {
    for (size_t y = 0; y < mapData.size(); ++y){
        for (size_t x = 0; x < mapData[y].size(); ++x){
            switch(mapData[y][x]) {
                case EMPTY:         mvprintw(y, x, " "); break;
                case WALL:          mvprintw(y, x, "#"); break;
                case IMMUNE_WALL:   mvprintw(y, x, "*"); break;
                case GROWTH_ITEM:   mvprintw(y, x, "+"); break;
                case POISON_ITEM:   mvprintw(y, x, "-"); break;
                default:            mvprintw(y, x, "?"); break;
            }
        }
    }
    refresh();
}

int Map::getValue(int y, int x) const {
    if (y < 0 || static_cast<size_t>(y) >= mapData.size()) return -1;
    if (x < 0 || static_cast<size_t>(x) >= mapData[y].size()) return -1;
    return mapData[y][x];
}

void Map::setValue(int y, int x, int value) {
    if (y < 0 || static_cast<size_t>(y) >= mapData.size()) return;
    if (x < 0 || static_cast<size_t>(x) >= mapData[y].size()) return;
    mapData[y][x] = value;
}

