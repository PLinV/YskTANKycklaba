#ifndef HISTORICAL_GAME_H
#define HISTORICAL_GAME_H

/**
 Représente l'état d'une partie (scores et identifiants).
 Cette structure stocke les noms d'utilisateurs et les scores courants.
 Elle permet aussi de définir qui a gagné la partie.
**/
typedef struct {
  char username1[50];
  char username2[50];
  char username3[50];
  char username4[50];
  int score1;
  int score2;
  int score3;
  int score4;
  int winUsername;
} HistoricalGame;

HistoricalGame *createHistoricalGame(const char *usernamePlayer1,
                                     const char *usernamePlayer2,
                                     const char *usernamePlayer3,
                                     const char *usernamePlayer4);

int incrementScore(HistoricalGame *game, int isPlayer, int value);

int whoWin(HistoricalGame *game, int winner);

int getLastGameId();

void J1vsJ2Score(const char *username1, const char *username2,
                 int *nbPartieGagnerJ1, int *nbPartieGagnerJ2);

int saveGame(HistoricalGame *game);

#endif // HISTORICAL_GAME_H