CC = gcc
CFLAGS = -I./src   # inclut tous les headers dans src/

# Détection de l'OS
ifeq ($(OS),Windows_NT)
    # ----- Windows -----
    RAYLIB_FLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
else
    # ----- Linux -----
    RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

LDFLAGS = $(RAYLIB_FLAGS)

EXEC = prog

SRC = src/Game/Game.c \
      src/Game/render.c \
      src/Graphisme/graphisme.c \
      src/Menu/menu.c \
      src/Stat/stat.c

OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

cleanexe:
	rm -f $(EXEC)

mrproper: clean cleanexe
