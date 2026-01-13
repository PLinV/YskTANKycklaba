CC = gcc
# -Iraylib/src permet de trouver raylib.h
CFLAGS = -I./src -Iraylib/src -Wall

# ----- Configuration Linux -----
# Au lieu de -lraylib, on pointe vers le fichier précis que vous avez compilé
RAYLIB_LIB = raylib/src/libraylib.a
# On ajoute les dépendances système obligatoires pour Linux
LDFLAGS = $(RAYLIB_LIB) -lGL -lm -lpthread -ldl -lrt -lX11

# Nom de l'exécutable
EXEC_GAME = prog

# --- LISTE DES FICHIERS SOURCES ---
# J'ai combiné les fichiers de votre commande gcc précédente et ceux de votre exemple.
# Supprimez ou ajoutez les lignes selon les fichiers qui existent vraiment dans votre projet !
SRC_GAME = src/statPage/statPage.c \
           src/statPage/input_box.c \
           src/dataCode/user.c \
           src/Menu/menu.c \
           src/Menu/GameSetup.c \
           src/Game/Game.c \
           src/Game/render.c \
           src/Graphisme/graphisme.c

# Transformation automatique .c -> .o
OBJ_GAME = $(SRC_GAME:.c=.o)

# Règle par défaut
all: $(EXEC_GAME)

# Édition de lien (Création de l'exécutable)
$(EXEC_GAME): $(OBJ_GAME)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compilation des fichiers objets (.c -> .o)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJ_GAME)

cleanexe:
	rm -f $(EXEC_GAME)

mrproper: clean cleanexe