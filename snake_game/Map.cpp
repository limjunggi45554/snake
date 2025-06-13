#include "Map.h"
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>


void Map::loadStage(int stage) {
    mapData.clear();

    std::string filename = "stage" + std::to_string(stage) + ".txt";
    std::ifstream fin(filename);

    if (!fin.is_open()) {
        std::cerr << "Failed to open map file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(fin, line)) {
        std::vector<int> row;
        for (char c : line) {
            if (c >= '0' && c <= '9') {
                row.push_back(c - '0');
            }
        }
        if (!row.empty())
            mapData.push_back(row);
    }

    fin.close();

    height = mapData.size();
    if (height > 0)
        width = mapData[0].size();

    srand(time(nullptr));
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
                case GATE:	     mvprintw(y, x, "G"); break;
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
std::vector<std::pair<int, int>> Map::getWallPositions() const {
    std::vector<std::pair<int, int>> walls;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (mapData[y][x] == 1) {
                walls.emplace_back(y, x);
            }
        }
    }
    return walls;
}
void Map::clearItem(int itemType) {
    for (auto& row : mapData) {
        for (auto& cell : row) {
            if (cell == itemType) {
                cell = EMPTY;
            }
        }
    }
}


