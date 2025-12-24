#ifndef HISTORICAL_GAME_H
#define HISTORICAL_GAME_H

/**
 * @struct HistoricalGame
 * @brief Représente l'état d'une partie (scores et identifiants).
 * * Cette structure stocke les noms d'utilisateurs et les scores courants.
 * Elle permet aussi de définir qui a gagné la partie.
 */
typedef struct {
  char username1[50];   /** @brief Nom du premier joueur @warning <50 char */
  char username2[50];   /** @brief Nom du deuxième joueur @warning <50 char */
  int score1;           /** @brief Score actuel du joueur 1 @warning >0 */
  int score2;           /** @brief Score actuel du joueur 2 @warning >0 */
  int winUsername;      /**< 0 si aucun gagnant, 1 pour joueur 1, 2 pour joueur 2 */
} HistoricalGame;

/**
 * @brief Alloue et initialise une nouvelle partie vierge.
 * * Cette fonction alloue dynamiquement de la mémoire pour une structure HistoricalGame.
 * Les scores sont initialisés à 0 et les nom par les username passé en paramètre.
 * * @param usernamePlayer1 le username du player 1, il aura l'identifiant 1
 * @param usernamePlayer2 le username du player 2, il aura l'identifiant 2
 * * @return HistoricalGame* Un pointeur vers la nouvelle partie, ou NULL en cas d'échec d'allocation.
 * * @warning les username doit faire moins de 50 char. 
 */
HistoricalGame* createHistoricalGame(const char* usernamePlayer1, const char* usernamePlayer2);

/**
 * @brief Augmente le score d'un joueur spécifique.
 * * @param game Pointeur vers la partie en cours.
 * @param isPlayer L'identifiant du joueur (1 pour joueur 1, 2 pour joueur 2).
 * @param value Le nombre de points à ajouter au score actuel.
 * * @return int Retourne 0 en cas de succès, ou -1 si les paramètres sont invalides (ex: game est NULL).
 */
int incrementScore(HistoricalGame* game, int isPlayer, int value);

/**
 * @brief mets la valeur de qui a gagné dans game en cours
 * * @param game Pointeur vers la partie en cours
 * @param winner l'id du gagnant (0 : personne, 1 : joueur1, 2 : joueur2)
 * * @return int Retourne 0 en cas de succès, ou -1 si les paramètres sont invalides (ex: game est NULL).
 */
int whoWin(HistoricalGame* game, int winner);

/**
 * @brief récupére le nombre de game jouer
 * * @return le nombre de partie jouer depuis le début de la vie entière
 */
int getLastGameId();

/**
 * @brief sauvegarde la game dans le historicalGame.txt
 * * @param game Pointeur vers la partie en cours qui veut être sauvegarder
 * * @return int Retourne 0 en cas de succès, ou -1 si les paramètres sont invalides (ex: game est NULL).
 */
int saveGame(HistoricalGame* game); 

#endif // HISTORICAL_GAME_H