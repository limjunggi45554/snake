// snake_ncurses.cpp
// Snake Game using ncurses with Growth/Poison Items, Gates, Scoreboard, and 4 Stages
// – 벽 종류: WALL (게이트 eligible), IMMUNE_WALL (코너 등 게이트 금지)
// – 뱀이 차지한 위치에 아이템 스폰 금지
// – 아이템 즉시 리스폰 / 미사용 시 15초 후 재스폰
// – 미션 목표: 길이>=4, +아이템>=1, -아이템>=1, 게이트>=1
// – 게이트는 뱀보다 항상 위에 그리기
// – 아이템 타이머 동기화
// – GATE 타일을 추가해 텔레포트 시 즉사 방지
// – 스테이지2 중앙행 벽 스킵
// – 키 입력: 같은 방향 무시, 직각 방향 즉시 반영, 반대 방향은 즉시 종료
// – 게이트 텔레포트: 진행 방향 우선 → 시계방향으로 차례대로
// – 스테이지 종료 시 종합 점수 표시

#include <ncurses.h>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>

// 맵 크기
const int WIDTH  = 21;
const int HEIGHT = 21;

// 게임 설정
const int INITIAL_LENGTH   = 3;
const int ITEM_LIFETIME_MS = 15000;  // 아이템 재스폰: 15초
const int TICK_MS          = 1000;   // 1초당 한 틱
const int TOTAL_STAGES     = 4;

// 타일 종류
enum Tile {
    EMPTY        = 0,
    WALL         = 1,
    IMMUNE_WALL  = 2,
    GROWTH_ITEM  = 3,
    POISON_ITEM  = 4,
    GATE         = 5
};

// 이동 방향
enum Direction { UP, RIGHT, DOWN, LEFT };

struct Point {
    int x, y;
    bool operator==(const Point& o) const { return x==o.x && y==o.y; }
};

// 랜덤 엔진
static std::mt19937 rng{std::random_device{}()};

// 반대 방향 체크
bool isOpposite(Direction a, Direction b) {
    return (a==UP&&b==DOWN)||(a==DOWN&&b==UP)
        ||(a==LEFT&&b==RIGHT)||(a==RIGHT&&b==LEFT);
}

// 방향별 이동 벡터
const int dx[4] = {0,1,0,-1};
const int dy[4] = {-1,0,1,0};

// 뱀과 겹치지 않는 빈 칸을 랜덤 반환
Point randomEmpty(const std::vector<std::vector<int>>& map,
                  const std::vector<Point>& snake) {
    std::vector<Point> empties;
    for(int y=1; y<HEIGHT-1; ++y){
        for(int x=1; x<WIDTH-1; ++x){
            if(map[y][x]==EMPTY &&
               std::find(snake.begin(), snake.end(), Point{x,y})==snake.end())
                empties.push_back({x,y});
        }
    }
    if(empties.empty()) return {-1,-1};
    std::uniform_int_distribution<int> d(0, (int)empties.size()-1);
    return empties[d(rng)];
}

int main(){
    // ncurses 초기화
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    nodelay(stdscr, TRUE);

    // 기본 맵 (외곽 벽, 코너는 IMMUNE_WALL)
    std::vector<std::vector<int>> baseMap(HEIGHT, std::vector<int>(WIDTH, EMPTY));
    for(int x=0; x<WIDTH; ++x){
        bool corner = (x==0||x==WIDTH-1);
        baseMap[0][x]        = corner ? IMMUNE_WALL : WALL;
        baseMap[HEIGHT-1][x] = corner ? IMMUNE_WALL : WALL;
    }
    for(int y=1; y<HEIGHT-1; ++y){
        baseMap[y][0]       = WALL;
        baseMap[y][WIDTH-1] = WALL;
    }

    // 스테이지별 반복
    for(int stage=1; stage<=TOTAL_STAGES; ++stage){
        // 맵 복사 및 스테이지별 장애물 추가
        auto map = baseMap;
        int sx=WIDTH/2, sy=HEIGHT/2;
        if(stage==2){
            for(int y=2; y<HEIGHT-2; ++y)
                if(y!=sy) map[y][sx]=WALL;
        } else if(stage==3){
            for(int d=-3; d<=3; ++d){
                map[sy+d][sx]=WALL;
                map[sy][sx+d]=WALL;
            }
        } else if(stage==4){
            for(int dy=-1; dy<=1; ++dy)
            for(int dx=-1; dx<=1; ++dx)
                if(abs(dx)+abs(dy)!=0)
                    map[sy+dy][sx+dx]=WALL;
        }

        // 스테이지 시작 시각
        auto t0 = std::chrono::steady_clock::now();

        // — 뱀 초기화 — 横連続区間列挙方式
        std::vector<Point> snake;
        Direction dir = RIGHT;
        // 1) 横連続 빈칸 길이 INITIAL_LENGTH 후보 수집
        std::vector<Point> heads;
        for(int y=1; y<HEIGHT-1; ++y){
            for(int x=INITIAL_LENGTH-1; x<WIDTH-1; ++x){
                bool ok=true;
                for(int i=0;i<INITIAL_LENGTH;++i){
                    if(map[y][x-i]!=EMPTY){ ok=false; break; }
                }
                if(ok) heads.push_back({x,y});
            }
        }
        // 2) 후보 중 랜덤 선택, 없으면 중앙
        if(heads.empty()){
            for(int i=0;i<INITIAL_LENGTH;++i)
                snake.push_back({sx-i, sy});
        } else {
            std::uniform_int_distribution<int> d(0, (int)heads.size()-1);
            Point head = heads[d(rng)];
            for(int i=0;i<INITIAL_LENGTH;++i)
                snake.push_back({ head.x-i, head.y });
        }

        // 게이트 배치 (WALL 중 랜덤 2곳)
        Point gateA{-1,-1}, gateB{-1,-1};
        {
            std::vector<Point> walls;
            for(int y=0;y<HEIGHT;++y)
            for(int x=0;x<WIDTH;++x)
                if(map[y][x]==WALL) walls.push_back({x,y});
            std::shuffle(walls.begin(), walls.end(), rng);
            if(walls.size()>=2){
                gateA=walls[0]; gateB=walls[1];
                map[gateA.y][gateA.x]=GATE;
                map[gateB.y][gateB.x]=GATE;
            }
        }

        // 아이템 즉시 스폰
        Point growth = randomEmpty(map, snake);
        if(growth.x>=0) map[growth.y][growth.x]=GROWTH_ITEM;
        Point poison = randomEmpty(map, snake);
        if(poison.x>=0) map[poison.y][poison.x]=POISON_ITEM;
        auto lastG = t0, lastP = t0;

        int gc=0, pc=0, gtc=0;
        int maxLen = (int)snake.size();
        bool running=true, done=false;

        // — 메인 게임 루프 —
        while(running){
            auto now = std::chrono::steady_clock::now();

            // 1) 입력 처리: 이번 틱의 마지막 키만 사용
            int ch = getch();
            if(ch!=ERR){
                int tmp;
                while((tmp=getch())!=ERR) ch=tmp;
            }
            Direction nd = dir;
            if     (ch==KEY_UP)    nd=UP;
            else if(ch==KEY_RIGHT) nd=RIGHT;
            else if(ch==KEY_DOWN)  nd=DOWN;
            else if(ch==KEY_LEFT)  nd=LEFT;
            else if(ch=='q'){ running=false; break; }
            if(nd!=dir){
                if(isOpposite(dir, nd)){ running=false; break; }
                dir=nd;
            }

            // 2) 아이템 재스폰 (만료 시)
            if(std::chrono::duration_cast<std::chrono::milliseconds>(now-lastG).count() >= ITEM_LIFETIME_MS){
                if(growth.x>=0) map[growth.y][growth.x]=EMPTY;
                growth = randomEmpty(map, snake);
                if(growth.x>=0) map[growth.y][growth.x]=GROWTH_ITEM;
                lastG=now;
            }
            if(std::chrono::duration_cast<std::chrono::milliseconds>(now-lastP).count() >= ITEM_LIFETIME_MS){
                if(poison.x>=0) map[poison.y][poison.x]=EMPTY;
                poison = randomEmpty(map, snake);
                if(poison.x>=0) map[poison.y][poison.x]=POISON_ITEM;
                lastP=now;
            }

            // 3) 머리 이동
            Point head = snake.front(), nh=head;
            nh.x += dx[dir]; nh.y += dy[dir];
            if(nh.x<0||nh.x>=WIDTH||nh.y<0||nh.y>=HEIGHT) break;
            int tile = map[nh.y][nh.x];
            if(tile==WALL||tile==IMMUNE_WALL) break;
            if(std::find(snake.begin(),snake.end(),nh)!=snake.end()) break;

            // 4) 게이트 텔레포트
            if(tile==GATE){
                gtc++;
                Point exitGate = (nh==gateA?gateB:gateA);
                bool found=false;
                for(int k=0;k<4;++k){
                    Direction d2 = Direction((dir+k)%4);
                    int ex=exitGate.x+dx[d2], ey=exitGate.y+dy[d2];
                    if(ex>=0&&ex<WIDTH&&ey>=0&&ey<HEIGHT){
                        int t2=map[ey][ex];
                        if(t2!=WALL&&t2!=IMMUNE_WALL){
                            nh={ex,ey}; dir=d2; found=true; break;
                        }
                    }
                }
                if(!found) break;
            }

            // 5) 아이템 먹기 / 성장·감소
            if(tile==GROWTH_ITEM){
                map[nh.y][nh.x]=EMPTY;
                snake.insert(snake.begin(), nh); gc++;
                growth=randomEmpty(map, snake);
                if(growth.x>=0) map[growth.y][growth.x]=GROWTH_ITEM;
                lastG=now; lastP=now;
            }
            else if(tile==POISON_ITEM){
                map[nh.y][nh.x]=EMPTY;
                snake.insert(snake.begin(), nh);
                if(snake.size()>2){ snake.pop_back(); snake.pop_back(); }
                pc++;
                poison=randomEmpty(map, snake);
                if(poison.x>=0) map[poison.y][poison.x]=POISON_ITEM;
                lastP=now; lastG=now;
                if(snake.size()<INITIAL_LENGTH) break;
            }
            else{
                snake.insert(snake.begin(), nh);
                snake.pop_back();
            }
            maxLen = std::max(maxLen, (int)snake.size());

            // 6) 미션 달성 체크 (아이템 1개로 완화)
            if((int)snake.size()>=4 && gc>=1 && pc>=1 && gtc>=1){
                done=true; break;
            }

            // 7) 그리기
            clear();
            for(int y=0;y<HEIGHT;++y)for(int x=0;x<WIDTH;++x){
                char c=' ';
                switch(map[y][x]){
                case WALL:        c='#'; break;
                case IMMUNE_WALL: c='#'; break;
                case GROWTH_ITEM: c='+'; break;
                case POISON_ITEM: c='-'; break;
                case GATE:        c='G'; break;
                }
                mvaddch(y,x,c);
            }
            mvaddch(snake.front().y, snake.front().x, 'O');
            for(size_t i=1;i<snake.size();++i)
                mvaddch(snake[i].y, snake[i].x, 'o');
            int elapsed = (int)std::chrono::duration_cast<std::chrono::seconds>(now-t0).count();
            int ix=WIDTH+2;
            mvprintw(1,ix,"Stage %d/%d",stage,TOTAL_STAGES);
            mvprintw(2,ix,"Len:%2d/%2d",(int)snake.size(),maxLen);
            mvprintw(3,ix,"+: %2d",gc);
            mvprintw(4,ix,"-: %2d",pc);
            mvprintw(5,ix,"G: %2d",gtc);
            mvprintw(7,ix,"Time:%2d",elapsed);
            mvprintw(9,ix,"Mission");
            mvprintw(10,ix,"L>=4:%c",snake.size()>=4?'v':' ');
            mvprintw(11,ix,"+>=1:%c",gc>=1?'v':' ');
            mvprintw(12,ix,"->=1:%c",pc>=1?'v':' ');
            mvprintw(13,ix,"G>=1:%c",gtc>=1?'v':' ');
            refresh();

            // 8) 남은 틱 동안 직각 키 즉시 반영
            auto start = std::chrono::steady_clock::now();
            while(running){
                auto passed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now()-start).count();
                if(passed>=TICK_MS) break;
                int c2 = getch();
                if(c2!=ERR){
                    Direction nd2=dir;
                    if     (c2==KEY_UP)    nd2=UP;
                    else if(c2==KEY_RIGHT) nd2=RIGHT;
                    else if(c2==KEY_DOWN)  nd2=DOWN;
                    else if(c2==KEY_LEFT)  nd2=LEFT;
                    else if(c2=='q'){ running=false; break; }
                    if(nd2!=dir){
                        if(isOpposite(dir,nd2)){ running=false; break; }
                        dir=nd2; break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        // 스테이지 종료 화면 (총점 표시)
        clear();
        int cy=HEIGHT/2-5, cx=(WIDTH-22)/2;
        mvprintw(cy,  cx,(done?" Stage %d Complete! ":"      Game Over  "),stage);
        mvprintw(cy+1,cx,"----------------------");
        mvprintw(cy+2,cx," Length   : %2d / %2d", (int)snake.size(), maxLen);
        mvprintw(cy+3,cx," Growth   : %2d", gc);
        mvprintw(cy+4,cx," Poison   : %2d", pc);
        mvprintw(cy+5,cx," Gate use : %2d", gtc);
        int elapsed_total = (int)std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now()-t0).count();
        mvprintw(cy+6,cx," Time (s) : %2d", elapsed_total);
        mvprintw(cy+7,cx,"----------------------");
        int totalScore = gc + pc + gtc;
        mvprintw(cy+8,cx," Total    : %2d", totalScore);
        mvprintw(cy+10,cx,"Press any key to continue");
        nodelay(stdscr,FALSE);
        getch();
        nodelay(stdscr,TRUE);
        if(!done) break;
    }

    endwin();
    return 0;
}
