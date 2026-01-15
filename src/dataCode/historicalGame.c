#include "historicalGame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HISTORICAL_GAME_DATA_FILE                                              \
  "data/historicalGame.txt" // peut-être à modifier

#ifdef _WIN32
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

/* gcc \
  YskTANKycklaba/src/dataCode/historicalGame.c \
  -o historicalGame \
  -Iraylib/src \
  raylib/src/libraylib.a \
  -lm -lpthread -ldl -lrt -lX11 -lGL */

HistoricalGame *createHistoricalGame(const char *usernamePlayer1,
                                     const char *usernamePlayer2,
                                     const char *usernamePlayer3,
                                     const char *usernamePlayer4) {
  if (strlen(usernamePlayer1) > 49 || strlen(usernamePlayer2) > 49 ||
      strlen(usernamePlayer3) > 49 || strlen(usernamePlayer4) > 49) {
    printf("username trop long");
    return NULL;
  }

  HistoricalGame *game = malloc(sizeof(HistoricalGame));
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

int incrementScore(HistoricalGame *game, int isPlayer, int value) {
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

int whoWin(HistoricalGame *game, int winner) {
  if (winner < 0 || winner > 4) {
    printf("winner n'est pas entre 0 et 4");
    return EXIT_FAILURE;
  }

  game->winUsername = winner;
  return EXIT_SUCCESS;
}

void J1vsJ2Score(const char *username1, const char *username2,
                 int *nbPartieGagnerJ1, int *nbPartieGagnerJ2) {
  // 1. Initialisation des compteurs à 0 par sécurité
  if (nbPartieGagnerJ1)
    *nbPartieGagnerJ1 = 0;
  if (nbPartieGagnerJ2)
    *nbPartieGagnerJ2 = 0;

  FILE *file = fopen(HISTORICAL_GAME_DATA_FILE, "r");
  if (file == NULL) {
    return; // Pas d'historique, scores restent à 0
  }

  char line[256];

  while (fgets(line, sizeof(line), file)) {
    // Copie de la ligne pour strtok
    char lineCopy[256];
    strcpy(lineCopy, line);

    // -- Extraction des données --
    // Format: id;u1;u2;u3;u4;s1;s2;s3;s4;winP
    // -- Extraction des données --
    // Format: id;u1;u2;u3;u4;s1;s2;s3;s4;winP
    char *tokenID = strtok(lineCopy, ";");
    if (!tokenID)
      continue; // Ligne vide ou sans ID

    // Header check plus robuste : si l'ID est "gameNum", c'est le header
    if (strcmp(tokenID, "gameNum") == 0) {
      continue;
    }

    char *u1 = strtok(NULL, ";"); // Username 1
    char *u2 = strtok(NULL, ";"); // Username 2
    char *u3 = strtok(NULL, ";"); // Username 3
    char *u4 = strtok(NULL, ";"); // Username 4

    // Skip s1, s2, s3, s4
    for (int i = 0; i < 4; i++) {
      if (!strtok(NULL, ";"))
        break;
    }

    char *winToken = strtok(NULL, ";\r\n");

    // -- Vérifications --
    if (u1 && u2 && u3 && u4 && winToken) {

      // CONDITION 1 : Vérifier que c'est bien un 1v1 (u3 et u4 doivent être
      // "NULL")
      int isStrictly1v1 =
          (strcasecmp(u3, "NULL") == 0) && (strcasecmp(u4, "NULL") == 0);

      if (!isStrictly1v1) {
        continue;
      }

      int winner = atoi(winToken);

      // CONDITION 2 : Identifier les joueurs et compter les points

      // CAS A : Ordre J1 vs J2
      if (strcasecmp(u1, username1) == 0 && strcasecmp(u2, username2) == 0) {
        if (winner == 1)
          (*nbPartieGagnerJ1)++;
        else if (winner == 2)
          (*nbPartieGagnerJ2)++;
      }
      // CAS B : Ordre J2 vs J1
      else if (strcasecmp(u1, username2) == 0 &&
               strcasecmp(u2, username1) == 0) {
        // Le winner est l'index dans le fichier (1=u1, 2=u2)
        // u1 est J2, u2 est J1
        if (winner == 1)
          (*nbPartieGagnerJ2)++; // u1 a gagné -> J2 a gagné
        else if (winner == 2)
          (*nbPartieGagnerJ1)++; // u2 a gagné -> J1 a gagné
      }
    }
  }

  fclose(file);
}

int getLastGameId() {
  FILE *file = fopen(HISTORICAL_GAME_DATA_FILE, "r");
  if (file == NULL) {
    return 0;
  }

  char line[256]; // Buffer pour stocker une ligne
  int lastId = 0;

  // On lit le fichier ligne par ligne
  while (fgets(line, sizeof(line), file)) {

    // strtok découpe la chaine au premier ";"
    char *token = strtok(line, ";");

    if (token != NULL) {
      // Vérifie si c'est le header
      if (strcmp(token, "gameNum") == 0)
        continue;

      // On convertit le bout de texte en entier
      lastId = atoi(token);
    }
  }

  fclose(file);
  return lastId;
}

int saveGame(HistoricalGame *game) {
  int newId = getLastGameId() + 1;

  // Check if file is empty/missing to write header
  FILE *checkFile = fopen(HISTORICAL_GAME_DATA_FILE, "r");
  long size = 0;
  if (checkFile) {
    fseek(checkFile, 0, SEEK_END);
    size = ftell(checkFile);
    fclose(checkFile);
  }

  FILE *file = fopen(HISTORICAL_GAME_DATA_FILE, "a");

  if (!file) {
    printf("Erreur : impossible d'ouvrir le fichier pour l'ajout.\n");
    return 0;
  }

  // Write header if file was empty
  if (size == 0) {
    fprintf(file, "gameNum;usernameP1;usernameP2;usernameP3;usernameP4;scoreP1;"
                  "scoreP2;scoreP3;scoreP4;winP\n");
  }

  fprintf(file, "%d;%s;%s;%s;%s;%d;%d;%d;%d;%d\n", newId, game->username1,
          game->username2, game->username3, game->username4, game->score1,
          game->score2, game->score3, game->score4, game->winUsername);
  fclose(file);

  return 1;
}

/*
int main(){
  HistoricalGame* currentGame = createHistoricalGame("Paulin", "Hatem", "NULL",
"NULL"); incrementScore(currentGame, 1, 60); incrementScore(currentGame, 2, 70);

  int nbPartieGagnerJ1 = 0;
  int nbPartieGagnerJ2 = 0;
  J1vsJ2Score(currentGame, &nbPartieGagnerJ1, &nbPartieGagnerJ2);
  printf("Avant sauvegarde : J1 a gagné %d parties, J2 a gagné %d parties\n",
nbPartieGagnerJ1, nbPartieGagnerJ2); whoWin(currentGame, 2);
  saveGame(currentGame);
  J1vsJ2Score(currentGame, &nbPartieGagnerJ1, &nbPartieGagnerJ2);
  printf("Après sauvegarde : J1 a gagné %d parties, J2 a gagné %d parties\n",
nbPartieGagnerJ1, nbPartieGagnerJ2); return 0;
}
*/