#include "historicalGame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HISTORICAL_GAME_DATA_FILE "YskTANKycklaba/data/historicalGame.txt" // peut-être à modifier
// gcc \
  YskTANKycklaba/src/dataCode/historicalGame.c \
  -o historicalGame \
  -Iraylib/src \
  raylib/src/libraylib.a \
  -lm -lpthread -ldl -lrt -lX11 -lGL

HistoricalGame* createHistoricalGame(const char* usernamePlayer1, const char* usernamePlayer2) {
  if (strlen(usernamePlayer1) > 49 ||strlen(usernamePlayer2) > 49 ) {
    printf("username trop long");
    return NULL;
  }
  
  HistoricalGame* game = malloc(sizeof(HistoricalGame));
  strcpy(game->username1, usernamePlayer1);  
  strcpy(game->username2, usernamePlayer2);
  game->score1 = 0;
  game->score2 = 0;
  game->winUsername = 0;  

  return game;
}

int incrementScore(HistoricalGame* game, int isPlayer, int value) {
  if (isPlayer == 1) {
    game->score1 += value;
  } else if (isPlayer == 2) {
    game->score2 += value;
  } else {
    printf("Game non trouvé");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int whoWin(HistoricalGame* game, int winner) {
  if (winner < 0 || winner > 2) {
    printf("winner n'est pas entre 0 et 2");
    return EXIT_FAILURE;
  } 

  game->winUsername = winner; 
  return EXIT_SUCCESS;
}

int getLastGameId() {
  FILE* file = fopen(HISTORICAL_GAME_DATA_FILE, "r");
  if (file == NULL) {
    return 0;
  }

  char line[256]; // Buffer pour stocker une ligne
  int lastId = 0;
  int isHeader = 1; // Indicateur pour savoir si on est sur la 1ere ligne

  // On lit le fichier ligne par ligne
  while (fgets(line, sizeof(line), file)) {
      
      // On saute la première ligne (l'en-tête)
      if (isHeader) {
        isHeader = 0;
        continue;
      }

      // strtok découpe la chaine au premier ";"
      char* token = strtok(line, ";");
      
      if (token != NULL) {
        // On convertit le bout de texte en entier
        lastId = atoi(token);
      }
  }

  fclose(file);
  return lastId;
}

int saveGame(HistoricalGame* game) {
  int newId = getLastGameId() + 1;

  FILE* file = fopen(HISTORICAL_GAME_DATA_FILE, "a");

  if (!file) {
    printf("Erreur : impossible de lire le fichier source.\n");
    return 0;
  }

  fprintf(file, "%d;%s;%s;%d;%d;%d\n", newId, game->username1, game->username2, game->score1, game->score2, game->winUsername);
  fclose(file);

  return 1;
}

/*
int main(){
  HistoricalGame* currentGame = createHistoricalGame("Paulin", "Hatem");
  incrementScore(currentGame, 1, 60);
  incrementScore(currentGame, 2, 70);
  whoWin(currentGame, 2); 
  saveGame(currentGame); 
}
*/