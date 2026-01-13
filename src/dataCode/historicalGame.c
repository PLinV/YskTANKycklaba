#include "historicalGame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HISTORICAL_GAME_DATA_FILE "data/historicalGame.txt" // peut-être à modifier
// gcc \
  YskTANKycklaba/src/dataCode/historicalGame.c \
  -o historicalGame \
  -Iraylib/src \
  raylib/src/libraylib.a \
  -lm -lpthread -ldl -lrt -lX11 -lGL

HistoricalGame* createHistoricalGame(const char* usernamePlayer1, const char* usernamePlayer2, const char* usernamePlayer3, const char* usernamePlayer4) {
  if (strlen(usernamePlayer1) > 49 || strlen(usernamePlayer2) > 49|| strlen(usernamePlayer3) > 49 || strlen(usernamePlayer4) > 49) {
    printf("username trop long");
    return NULL;
  }
  
  HistoricalGame* game = malloc(sizeof(HistoricalGame));
  strcpy(game->username1, usernamePlayer1);  
  strcpy(game->username2, usernamePlayer2);
  strcpy(game->username3, usernamePlayer3);  
  strcpy(game->username4, usernamePlayer4); 
  game->score1 = 0;
  game->score2 = 0;
  game->score3 = 0;
  game->score4 = 0;
  game->winUsername = 0;  

  return game;
}

int incrementScore(HistoricalGame* game, int isPlayer, int value) {
  if (isPlayer == 1) {
    game->score1 += value;
  } else if (isPlayer == 2) {
    game->score2 += value;
  } else if (isPlayer == 3) {
    game->score3 += value;
  } else if (isPlayer == 4) {
    game->score4 += value;
  } else {
    printf("Game non trouvé");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int whoWin(HistoricalGame* game, int winner) {
  if (winner < 0 || winner > 4) {
    printf("winner n'est pas entre 0 et 4");
    return EXIT_FAILURE;
  } 

  game->winUsername = winner; 
  return EXIT_SUCCESS;
}

void J1vsJ2Score(HistoricalGame* gamen, int* nbPartieGagnerJ1, int* nbPartieGagnerJ2) {
  // 1. Initialisation des compteurs à 0 par sécurité
  if (nbPartieGagnerJ1) *nbPartieGagnerJ1 = 0;
  if (nbPartieGagnerJ2) *nbPartieGagnerJ2 = 0;

  FILE* file = fopen(HISTORICAL_GAME_DATA_FILE, "r");
  if (file == NULL) {
    printf("Erreur de lecture du fichier historique.\n");
    return;
  }

  char line[256];
  int isHeader = 1;

  while (fgets(line, sizeof(line), file)) {
    // Ignorer l'en-tête
    if (isHeader) {
        isHeader = 0;
        continue;
    }

    // Copie de la ligne pour strtok
    char lineCopy[256];
    strcpy(lineCopy, line);

    // -- Extraction des données --
    // Format: id;u1;u2;u3;u4;s1;s2;s3;s4;winP
    char* token = strtok(lineCopy, ";"); // ID
    char* u1 = strtok(NULL, ";");        // Username 1
    char* u2 = strtok(NULL, ";");        // Username 2
    char* u3 = strtok(NULL, ";");        // Username 3
    char* u4 = strtok(NULL, ";");        // Username 4
    
    // On saute les scores
    char* tempToken = strtok(NULL, ";\r\n");
    char* winToken = NULL;

    while (tempToken != NULL) {
      winToken = tempToken; 
      tempToken = strtok(NULL, ";\r\n");
    }
    
    // -- Vérifications --
    if (u1 && u2 && u3 && u4 && winToken) {
      
      // CONDITION 1 : Vérifier que c'est bien un 1v1 (u3 et u4 doivent être "NULL")
      // Note : On compare avec "NULL" car c'est ainsi que c'est écrit dans ta DB exemple.
      int isStrictly1v1 = (strcasecmp(u3, "NULL") == 0) && (strcasecmp(u4, "NULL") == 0);

      if (!isStrictly1v1) {
        continue; // On passe à la ligne suivante si ce n'est pas un 1v1
      }

      int winner = atoi(winToken);

      // CONDITION 2 : Identifier les joueurs et compter les points
      
      // CAS A : Le fichier est dans l'ordre (J1 vs J2)
      if (strcasecmp(u1, gamen->username1) == 0 && strcasecmp(u2, gamen->username2) == 0) {
        if (winner == 1) (*nbPartieGagnerJ1)++;
        if (winner == 2) (*nbPartieGagnerJ2)++;
      }
      // CAS B : Le fichier est inversé (J2 vs J1)
      else if (strcasecmp(u1, gamen->username2) == 0 && strcasecmp(u2, gamen->username1) == 0) {
        // Si u1 (qui est J2 ici) a gagné (winner=1), alors J2 gagne
        if (winner == 1) (*nbPartieGagnerJ2)++; 
        // Si u2 (qui est J1 ici) a gagné (winner=2), alors J1 gagne
        if (winner == 2) (*nbPartieGagnerJ1)++;
      }
    }
  }

  fclose(file);
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

  fprintf(file, "%d;%s;%s;%s;%s;%d;%d;%d;%d;%d\n", newId, game->username1, game->username2, game->username3, game->username4, game->score1, game->score2, game->score3, game->score4,game->winUsername);
  fclose(file);

  return 1;
}

int main(){
  HistoricalGame* currentGame = createHistoricalGame("Paulin", "Hatem", "NULL", "NULL");
  incrementScore(currentGame, 1, 60);
  incrementScore(currentGame, 2, 70);

  int nbPartieGagnerJ1 = 0;
  int nbPartieGagnerJ2 = 0;
  J1vsJ2Score(currentGame, &nbPartieGagnerJ1, &nbPartieGagnerJ2);
  printf("Avant sauvegarde : J1 a gagné %d parties, J2 a gagné %d parties\n", nbPartieGagnerJ1, nbPartieGagnerJ2);
  whoWin(currentGame, 2);
  saveGame(currentGame);
  J1vsJ2Score(currentGame, &nbPartieGagnerJ1, &nbPartieGagnerJ2);
  printf("Après sauvegarde : J1 a gagné %d parties, J2 a gagné %d parties\n", nbPartieGagnerJ1, nbPartieGagnerJ2);
  return 0; 
}
