CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lncurses

OBJS = main.o GameManager.o Map.o Snake.o

snake: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o snake

