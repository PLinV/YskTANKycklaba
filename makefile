CC = gcc

EXEC = prog
OBJ = game.o graphisme.o menu.o stat.o

$(EXEC): $(OBJ)
	$(CC) -o $@ $(OBJ) -lraylib -lopengl32 -lgdi32 -lwinmm

game.o : src/Game/game.c src/Game/game.h
	$(CC) -o game.o -c game.c

graphisme.o: src/Graphisme/graphisme.c src/Graphisme/graphisme.h
	$(CC) -o graphisme.o -c graphisme.c

menu.o : src/Menu/menu.c src/Menu/menu.h
	$(CC) -o menu.o -c menu.c

stat.o : src/Stat/stat.c src/Stat/stat.h
	$(CC) -o stat.o -c stat.c


all: $(EXEC)

clean: 
	rm -rf *.o

cleanexe :
	rm -rf *.exe
mrproper: clean cleantxt
	rm -rf $(EXEC)

