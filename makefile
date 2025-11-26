CC = gcc
CFLAGS = -I./src  # inclut tous les headers dans src/
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

EXEC = prog

SRC = src/Game/Game.c \
	  src/Game/render.c \
      src/Graphisme/graphisme.c \
      src/Menu/menu.c \
      src/Stat/stat.c

OBJ = $(SRC:.c=.o)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# règle générique : transforme n'importe quel .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXEC)

clean:
	rm -f $(OBJ)

cleanexe:
	rm -f $(EXEC)

mrproper: clean cleanexe
