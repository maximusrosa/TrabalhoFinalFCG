SOURCES := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/*.c) 

./bin/Linux/CowQuest: $(SOURCES)
	mkdir -p bin/Linux
	g++ -std=c++17 -Wall -Wno-unused-function -g -I ./include/ -o ./bin/Linux/CowQuest $(SOURCES) ./lib-linux/libglfw3.a -lrt -lm -ldl -lX11 -lpthread -lXrandr -lXinerama -lXxf86vm -lXcursor

.PHONY: clean run
clean:
	rm -f bin/Linux/CowQuest

run: ./bin/Linux/CowQuest
	cd bin/Linux && ./CowQuest
