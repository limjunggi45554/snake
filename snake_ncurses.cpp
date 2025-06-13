// snake_ncurses.cpp
// Snake Game using ncurses with Growth/Poison Items, Gates, Scoreboard, and 4 Stages

#include <ncurses.h>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>

// Map dimensions
const int WIDTH = 21;
const int HEIGHT = 21;

// Game settings
const int INITIAL_LENGTH = 3;
const int ITEM_LIFETIME_MS = 5000;  // 5 seconds
const int TICK_MS = 1000;           // 1 second per tick
const int TOTAL_STAGES = 4;

// Map tile codes
enum Tile { EMPTY, WALL, SNAKE_HEAD, SNAKE_BODY, GROWTH_ITEM, POISON_ITEM, GATE };
enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
    int x, y;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

// Random engine
static std::mt19937 rng{std::random_device{}()};

// Check opposite directions
bool isOpposite(Direction a, Direction b) {
    return (a==UP && b==DOWN) || (a==DOWN && b==UP) ||
           (a==LEFT && b==RIGHT) || (a==RIGHT && b==LEFT);
}

// Return a random empty cell, or {-1,-1} if none
Point randomEmpty(const std::vector<std::vector<int>>& map) {
    std::vector<Point> empties;
    for(int y=1; y<HEIGHT-1; ++y)
        for(int x=1; x<WIDTH-1; ++x)
            if(map[y][x] == EMPTY)
                empties.push_back({x,y});
    if(empties.empty()) return {-1,-1};
    std::uniform_int_distribution<int> dist(0, (int)empties.size()-1);
    return empties[dist(rng)];
}

int main() {
    initscr(); cbreak(); noecho();
    keypad(stdscr, TRUE); curs_set(0);
    nodelay(stdscr, TRUE);

    // Base map with walls
    std::vector<std::vector<int>> baseMap(HEIGHT, std::vector<int>(WIDTH, EMPTY));
    for(int x=0; x<WIDTH; ++x) {
        baseMap[0][x] = baseMap[HEIGHT-1][x] = WALL;
    }
    for(int y=0; y<HEIGHT; ++y) {
        baseMap[y][0] = baseMap[y][WIDTH-1] = WALL;
    }

    // Main stage loop
    for(int stage=1; stage<=TOTAL_STAGES; ++stage) {
        // Copy base map
        auto map = baseMap;

        // Initialize snake at center
        std::vector<Point> snake;
        int sx = WIDTH/2, sy = HEIGHT/2;
        for(int i=0; i<INITIAL_LENGTH; ++i)
            snake.push_back({sx-i, sy});
        Direction dir = RIGHT;

        // Item & gate state
        Point growthPos{-1,-1}, poisonPos{-1,-1};
        Point gateA{-1,-1}, gateB{-1,-1};

        // Spawn initial gate immediately
        {
            std::vector<Point> walls;
            // scan entire baseMap for WALL tiles
            for(int y=0; y<HEIGHT; ++y) {
                for(int x=0; x<WIDTH; ++x) {
                    if(baseMap[y][x] == WALL) {
                        walls.push_back({x,y});
                    }
                }
            }
            if(walls.size() >= 2) {
                std::shuffle(walls.begin(), walls.end(), rng);
                gateA = walls[0];
                gateB = walls[1];
                map[gateA.y][gateA.x] = GATE;
                map[gateB.y][gateB.x] = GATE;
            }
        }

        int growthCount = 0, poisonCount = 0, gateCount = 0;
        int maxLength = (int)snake.size();
        bool running = true, missionComplete = false;

        auto lastGrowth = std::chrono::steady_clock::now();
        auto lastPoison = lastGrowth;
        auto startTime = std::chrono::steady_clock::now();

        // Game loop for this stage
        while(running) {
            auto now = std::chrono::steady_clock::now();

            // Input handling
            int ch = getch();
            Direction newDir = dir;
            if(ch == KEY_UP)    newDir = UP;
            else if(ch == KEY_DOWN)  newDir = DOWN;
            else if(ch == KEY_LEFT)  newDir = LEFT;
            else if(ch == KEY_RIGHT) newDir = RIGHT;
            else if(ch == 'q') { running = false; break; }

            if(newDir != dir) {
                if(isOpposite(dir, newDir)) { running = false; break; }
                dir = newDir;
            }

            // Respawn Growth Item every 5 seconds
            if(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastGrowth).count() >= ITEM_LIFETIME_MS) {
                if(growthPos.x >= 0) {
                    map[growthPos.y][growthPos.x] = EMPTY;
                }
                growthPos = randomEmpty(map);
                if(growthPos.x >= 0) {
                    map[growthPos.y][growthPos.x] = GROWTH_ITEM;
                }
                lastGrowth = now;
            }

            // Respawn Poison Item every 5 seconds
            if(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPoison).count() >= ITEM_LIFETIME_MS) {
                if(poisonPos.x >= 0) {
                    map[poisonPos.y][poisonPos.x] = EMPTY;
                }
                poisonPos = randomEmpty(map);
                if(poisonPos.x >= 0) {
                    map[poisonPos.y][poisonPos.x] = POISON_ITEM;
                }
                lastPoison = now;
            }

            // Move snake head
            Point head = snake.front();
            Point nh = head;
            switch(dir) {
                case UP:    nh.y--; break;
                case DOWN:  nh.y++; break;
                case LEFT:  nh.x--; break;
                case RIGHT: nh.x++; break;
            }

            // Boundary check
            if(nh.x < 0 || nh.x >= WIDTH || nh.y < 0 || nh.y >= HEIGHT) {
                running = false; break;
            }

            int tile = map[nh.y][nh.x];

            // Wall collision
            if(tile == WALL) { running = false; break; }

            // Self collision
            if(std::find(snake.begin(), snake.end(), nh) != snake.end()) {
                running = false; break;
            }

            // Gate teleport
            if(tile == GATE) {
                gateCount++;
                nh = (nh == gateA ? gateB : gateA);
            }

            // Item pick-up
            if(tile == GROWTH_ITEM) {
                snake.insert(snake.begin(), nh);
                growthCount++;
            }
            else if(tile == POISON_ITEM) {
                snake.insert(snake.begin(), nh);
                if(snake.size() > 2) {
                    snake.pop_back();
                    snake.pop_back();
                }
                poisonCount++;
                if(snake.size() < INITIAL_LENGTH) {
                    running = false; break;
                }
            }
            else {
                // normal move
                snake.insert(snake.begin(), nh);
                snake.pop_back();
            }
            maxLength = std::max(maxLength, (int)snake.size());

            // Check mission success
            if((int)snake.size() >= 10 &&
               growthCount >= 5 &&
               poisonCount >= 2 &&
               gateCount >= 1) 
            {
                missionComplete = true;
                running = false;
            }

            // Draw everything
            clear();
            // Map tiles
            for(int y=0; y<HEIGHT; ++y) {
                for(int x=0; x<WIDTH; ++x) {
                    char c = ' ';
                    switch(map[y][x]) {
                        case WALL:        c = '#'; break;
                        case GROWTH_ITEM: c = '+'; break;
                        case POISON_ITEM: c = '-'; break;
                        case GATE:        c = 'G'; break;
                        default:          c = ' '; break;
                    }
                    mvaddch(y, x, c);
                }
            }
            // Snake
            mvaddch(snake.front().y, snake.front().x, 'O');
            for(size_t i=1; i<snake.size(); ++i) {
                mvaddch(snake[i].y, snake[i].x, 'o');
            }
            // Scoreboard & status
            int elapsed = (int)std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            int infoX = WIDTH + 2;
            mvprintw(1, infoX, "Stage %d/%d", stage, TOTAL_STAGES);
            mvprintw(2, infoX, "Length: %2d/%2d", (int)snake.size(), maxLength);
            mvprintw(3, infoX, "+: %2d", growthCount);
            mvprintw(4, infoX, "-: %2d", poisonCount);
            mvprintw(5, infoX, "G: %2d", gateCount);
            mvprintw(7, infoX, "Time: %2d", elapsed);
            mvprintw(9, infoX, "Mission");
            mvprintw(10,infoX, "Len>=10: %c", snake.size()>=10 ? 'v' : ' ');
            mvprintw(11,infoX, "+>=5: %c", growthCount>=5 ? 'v' : ' ');
            mvprintw(12,infoX, "->=2: %c", poisonCount>=2 ? 'v' : ' ');
            mvprintw(13,infoX, "G>=1: %c", gateCount>=1 ? 'v' : ' ');
            refresh();

            std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
        }

        // Stage end message
        clear();
        if(missionComplete) {
            mvprintw(HEIGHT/2, (WIDTH/2)-8, "Stage %d Complete!", stage);
        } else {
            mvprintw(HEIGHT/2, (WIDTH/2)-5, "Game Over");
        }
        mvprintw(HEIGHT/2+1, (WIDTH/2)-8, "Press any key");
        nodelay(stdscr, FALSE);
        getch();
        nodelay(stdscr, TRUE);
        if(!missionComplete) break;
    }

    endwin();
    return 0;
}

/*
Compile & Run:
    g++ -g -std=c++17 snake_ncurses.cpp -lncurses -o snake
    ./snake

Controls:
    Arrow keys - Move
    q          - Quit immediately
*/
