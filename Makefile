CXX = clang++
SDL = -framework SDL2
# If your compiler is a bit older you may need to change -std=c++11 to -std=c++0x
CXXFLAGS = -Wall -c -std=c++11
LDFLAGS = $(SDL)
EXE = chip8

all: $(EXE)


$(EXE): main.o

	$(CXX) $(LDFLAGS) $< -o $@

main.o: main.cpp screen.cpp chip8.cpp

	$(CXX) $(CXXFLAGS) $< -o $@

clean:

	rm *.o && rm $(EXE)

build:
	g++ main.cpp chip8.cpp screen.cpp

run:
	g++ main.cpp chip8.cpp screen.cpp -o chip8 -I include -l SDL2-2.0.0
	./chip8 clock.ch8
