INC=include
LIB=lib

CC=gcc
CFLGAS=-Wall -Werror -I $(INC)
LDFLAGS=-L $(LIB) -lSDL2main -lSDL2

main.x: main.o events.o cells_grid.o player.o bomb.o animated_sprite.o datafile.o sprite.o hud.o powerups.o
	$(CC) $(CFLGAS) -o $@ main.o events.o cells_grid.o player.o bomb.o animated_sprite.o datafile.o sprite.o hud.o powerups.o $(LDFLAGS)

main.o : src/main.c src/events.c src/cells_grid.c src/player.c src/bomb.c src/animated_sprite.c src/datafile.c src/sprite.c src/hud.c src/powerups.c
	$(CC) $(CFLGAS) -c src/main.c src/events.c src/cells_grid.c src/player.c src/bomb.c src/animated_sprite.c src/datafile.c src/sprite.c src/hud.c src/powerups.c

clean:
	rm -f *~ *.o

realclean: clean
	rm -f *.x