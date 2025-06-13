#pragma once
#include <utility>  
#include "Map.h"

class GameManager {
public:
    void run();
    void generateGates();

private:
    Map map;
    std::pair<int, int> gate1, gate2;
};

