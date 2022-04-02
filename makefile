all: tetris

tetris: tetris.o game.o primlib.o
	gcc -fsanitize=undefined -g $^ -o $@  -lSDL2_gfx `sdl2-config --libs`
	./tetris

.c.o: 
	gcc -fsanitize=undefined -g -Wall -pedantic `sdl2-config --cflags` -c  $<

primlib.o: primlib.c primlib.h

tetris.o: tetris.c primlib.h game.h

game.o: game.c primlib.h game.h

clean:
	-rm primlib.o tetris.o game.o tetris