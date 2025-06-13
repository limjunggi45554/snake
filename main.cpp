#include <ncurses.h>
#include <fstream>
#include <deque>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <stdexcept>

using namespace std::chrono;

constexpr int TICK_MS = 2000;
constexpr int INIT_LEN = 3;

enum Cell { BLANK=0, WALL=1, IMWALL=2, HEAD=3, BODY=4 };

struct Pos { int y,x; };

class Game {
public:
    Game(): rng(std::random_device{}()) { loadMap(); init(); }
    void run();

private:
    std::vector<std::vector<int>> map;
    int rows{}, cols{};
    std::deque<Pos> snake;
    int dirY{0}, dirX{1};
    std::mt19937 rng;

    void loadMap();
    void init();
    void draw();
    void tick();
    void handleInput(int ch);
    bool moveSnake();
};

void Game::loadMap() {
    std::ifstream fin("map1.txt");
    if (!fin) throw std::runtime_error("map1.txt not found");
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<int> row;
        for (char c : line) row.push_back(c - '0');
        map.push_back(row);
    }
    rows = map.size();
    cols = map[0].size();
}

void Game::init() {
    snake.clear();
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            if (map[y][x] == HEAD) snake.push_front({y,x});
            else if (map[y][x] == BODY) snake.push_back({y,x});

    if (snake.empty()) {
        int y = rows / 2;
        snake.push_back({y, 10});
        snake.push_back({y, 9});
        snake.push_back({y, 8});
        map[y][10] = HEAD;
        map[y][9] = BODY;
        map[y][8] = BODY;
    }

    if (snake.size() > 1) {
        Pos h = snake[0], s = snake[1];
        dirY = h.y - s.y;
        dirX = h.x - s.x;
    } else {
        dirY = 0; dirX = 1;
    }

    Pos head = snake.front();
    const int dy[4] = {-1, 1, 0, 0};
    const int dx[4] = {0, 0, -1, 1};
    for (int i = 0; i < 4; ++i) {
        int ny = head.y + dy[i];
        int nx = head.x + dx[i];
        if (ny < 0 || ny >= rows || nx < 0 || nx >= cols) continue;
        int cell = map[ny][nx];
        if (cell == BLANK) {
            dirY = dy[i];
            dirX = dx[i];
            break;
        }
    }
}

void Game::draw() {
    clear();
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x) {
            int cell = map[y][x];
            if (cell == WALL || cell == IMWALL) mvaddch(y, x, '#');
            else if (cell == HEAD) { attron(COLOR_PAIR(1)); mvaddch(y,x,'O'); attroff(COLOR_PAIR(1)); }
            else if (cell == BODY) { attron(COLOR_PAIR(2)); mvaddch(y,x,'o'); attroff(COLOR_PAIR(2)); }
            else mvaddch(y, x, ' ');
        }
    refresh();
}

void Game::handleInput(int ch) {
    int ny=dirY, nx=dirX;
    switch(ch){
        case KEY_UP:    ny=-1; nx=0; break;
        case KEY_DOWN:  ny=1; nx=0; break;
        case KEY_LEFT:  ny=0; nx=-1; break;
        case KEY_RIGHT: ny=0; nx=1; break;
        default: return;
    }
    if (snake.size()>1){
        Pos h = snake.front(), s = snake[1];
        if (h.y+ny==s.y && h.x+nx==s.x) return;
    }
    dirY=ny; dirX=nx;
}

bool Game::moveSnake() {
    Pos head = snake.front();
    Pos next{head.y + dirY, head.x + dirX};
    if (next.y < 0 || next.y >= rows || next.x < 0 || next.x >= cols) return false;
    int cell = map[next.y][next.x];
    if (cell == WALL || cell == IMWALL || cell == BODY) return false;

    map[head.y][head.x] = BODY;
    snake.push_front(next);
    map[next.y][next.x] = HEAD;

    map[snake.back().y][snake.back().x] = BLANK;
    snake.pop_back();
    return true;
}

void Game::tick() {
    if (!moveSnake()) {
        mvprintw(rows/2, cols/2 - 5, "GAME OVER");
        refresh();
        napms(2000);
        endwin();
        exit(0);
    }
}

void Game::run() {
    initscr(); noecho(); cbreak(); curs_set(FALSE); keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); timeout(0);
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);

    auto lastMove = steady_clock::now();

    while (true) {
        int ch = getch();
        if (ch == 'q') break;
        if (ch != ERR) handleInput(ch);

        auto now = steady_clock::now();
        if (duration_cast<milliseconds>(now - lastMove).count() >= TICK_MS) {
            tick();
            draw();
            lastMove = now;
        }
    }
    endwin();
}

int main() {
    try {
        Game g; g.run();
    } catch (const std::exception& e) {
        endwin();
        fprintf(stderr, "Error: %s\n", e.what());
    }
    return 0;
}
