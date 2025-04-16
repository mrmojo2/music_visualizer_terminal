build:
	gcc -Wall -I ./include ./src/*.c -lSDL2 -lm -lpulse -lpulse-simple -o ./bin/visualizer

run:
	./bin/visualizer
clean:
	rm -rf ./bin/
