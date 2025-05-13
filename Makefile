build:
	gcc -Wall -I ./include ./src/*.c -lSDL2 -lm -lncursesw -o ./bin/visualizer

run:
	./bin/visualizer
clean:
	rm -rf ./bin/
