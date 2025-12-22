CC = gcc
CFLAGS = -I./src -I"C:/Program Files/raylib/w64devkit/x86_64-w64-mingw32/include"  # inclut tous les headers dans src/ et raylib

# Détection de l'OS
ifeq ($(OS),Windows_NT)
    # ----- Windows -----
    RAYLIB_FLAGS = -L"C:/Program Files/raylib/w64devkit/x86_64-w64-mingw32/lib" -lraylib -lopengl32 -lgdi32 -lwinmm
else
    # ----- Linux -----
    RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

LDFLAGS = $(RAYLIB_FLAGS)

EXEC_GAME = prog
EXEC_GAME = prog

SRC_GAME = src/Menu/menu.c \
           src/Game/Game.c \
           src/Game/render.c \
           src/Stat/stat.c \
           src/Graphisme/graphisme.c

OBJ_GAME = $(SRC_GAME:.c=.o)

all: $(EXEC_GAME)

$(EXEC_GAME): $(OBJ_GAME)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_GAME)

cleanexe:
	rm -f $(EXEC_GAME)

mrproper: clean cleanexe
