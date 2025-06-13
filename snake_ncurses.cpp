#include <ncurses.h>
#include <vector>
#include <chrono>
#include <thread>

const int WIDTH = 21;
const int HEIGHT = 21;
const int INITIAL_LENGTH = 3; // head + tail segments
const int TICK_MS = 200;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point { int x, y; };

bool isOpposite(Direction d1, Direction d2) {
    return (d1 == UP && d2 == DOWN) || (d1 == DOWN && d2 == UP) ||
           (d1 == LEFT && d2 == RIGHT) || (d1 == RIGHT && d2 == LEFT);
}

int main() {
    initscr();            // ncurses start
    cbreak();             // disable line buffering
    noecho();             // do not echo input
    keypad(stdscr, TRUE); // enable arrow keys
    nodelay(stdscr, TRUE);// non-blocking getch
    curs_set(0);          // hide cursor

    // Initialize snake in center, horizontal to the left
    std::vector<Point> snake;
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    for (int i = 0; i < INITIAL_LENGTH; ++i) {
        snake.push_back({ startX - i, startY });
    }
    Direction dir = RIGHT;

    bool running = true;
    while (running) {
        int ch = getch();
        Direction newDir = dir;
        switch (ch) {
            case KEY_UP:    newDir = UP;    break;
            case KEY_DOWN:  newDir = DOWN;  break;
            case KEY_LEFT:  newDir = LEFT;  break;
            case KEY_RIGHT: newDir = RIGHT; break;
            case 'q': running = false;     break;
        }
        // ignore reverse direction and same direction
        if (!isOpposite(dir, newDir) && newDir != dir) {
            dir = newDir;
        }

        // compute new head
        Point head = snake.front();
        Point newHead = head;
        switch (dir) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }
        // collision with wall
        if (newHead.x <= 0 || newHead.x >= WIDTH-1 || newHead.y <= 0 || newHead.y >= HEIGHT-1) {
            running = false;
        }
        // collision with self
        for (auto &p : snake) {
            if (p.x == newHead.x && p.y == newHead.y) {
                running = false;
                break;
            }
        }
        if (!running) break;

        // move snake: add new head, remove tail
        snake.insert(snake.begin(), newHead);
        snake.pop_back();

        clear();
        // draw walls
        for (int x = 0; x < WIDTH; ++x) {
            mvaddch(0, x, '#');
            mvaddch(HEIGHT-1, x, '#');
        }
        for (int y = 0; y < HEIGHT; ++y) {
            mvaddch(y, 0, '#');
            mvaddch(y, WIDTH-1, '#');
        }
        // draw snake
        for (size_t i = 0; i < snake.size(); ++i) {
            char c = (i == 0 ? 'O' : 'o');
            mvaddch(snake[i].y, snake[i].x, c);
        }
        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
    }

    // game over screen
    nodelay(stdscr, FALSE);
    mvprintw(HEIGHT/2, (WIDTH-9)/2, "Game Over!");
    mvprintw(HEIGHT/2+1, (WIDTH-19)/2, "Press any key to exit.");
    refresh();
    getch();

    endwin();
    return 0;
}
