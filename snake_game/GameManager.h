#pragma once
#include <utility>
#include <ctime>
#include "Map.h"

class Snake;  // forward declaration

class GameManager {
public:
    void run();
    void generateGates();
    void renderScoreBoard(const Snake& snake) const;
    bool checkMissionClear(const Snake& snake) const;
    int currentStage = 1;

private:
    Map map;
    std::pair<int, int> gate1, gate2;
    int growthCount = 0;
    int poisonCount = 0;
    int gateUseCount = 0;
    int maxLength = 10;
    time_t startTime;
    time_t lastGrowthItemTime = 0;
    time_t lastPoisonItemTime = 0;
    void addItemAvoidSnake(int itemType, const Snake& snake);

};
