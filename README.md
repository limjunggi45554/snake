이 프로젝트는 c++ 로 만든 스네이크 게임입니다.

각 단계별로 나누어져있으며 각 단계가 무엇인지 보고 싶다면 각 폴더에 들어가서 make후 ./snake를 하면 각 단계에서 수행한 것을 알 수 있습니다.

최종 파일은 snake_game 폴더입니다.

다음은 설치 방법입니다

git clone https://github.com/limjunggi45554/snake.git

ncurses 설치 방법 (Ubuntu)

sudo apt update

sudo apt install libncurses5-dev libncursesw5-dev

cd snake && cd snake_game&&make

./snake

움직이는 방법 카트라이더와 같이 방향키와 같음 단 머리와 꼬리가 부딪히면 안되고 움직이는 방향의 반대가 되면 안된다.

스테이지는 총 4개로 이루어져 있고, 각 스테이지마다 달성 해야하는 목표가 있고, 그것을 달성하면 다음맵으로 넘어간다. 
